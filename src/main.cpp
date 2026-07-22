#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    (-1)
#define SCREEN_ADDRESS 0x3C

#define I2C_SDA 6
#define I2C_SCL 4

Adafruit_BME280 bme;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
    Serial.begin(115200);
    Serial.println("\n--- TYTO System Booting ---");

    // I2C initialization
    Wire.begin(I2C_SDA, I2C_SCL);

    // OLED screen initialization
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("Error: OLED screen not found."));
    }

    Wire.setClock(100000);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Starting...");
    display.display();

    // Sensor initialization
    if (!bme.begin(0x77, &Wire)) {
        Serial.println("Error: BME280 sensor not found.");
    }

    Serial.println("Sensor and screen initialized.");
    Serial.println("-------------------------");

    // Wi-Fi connexion
    Serial.print("Connecting to network: ");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi connexion sucessfull.");
}

void loop() {
    // Restart wire I2C
    Wire.end();
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(100000);

    // Give back parameters to sensor
    bme.begin(0x77, &Wire);

    // Reading sensor
    const float temp = bme.readTemperature();
    const float hum = bme.readHumidity();
    const float pres = bme.readPressure() / 100.0F;

    Serial.printf("Temp: %.2f *C | Hum: %.2f %% | Pres: %.2f hPa\n", temp, hum, pres);

    // Update display
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("Current temperature");

    display.setCursor(0, 16);
    display.setTextSize(2);
    display.printf("%.1f C", temp);

    display.setCursor(0, 40);
    display.setTextSize(1);
    display.printf("Humidity : %.0f %%", hum);
    display.setCursor(0, 52);
    display.printf("Pressure : %.0f hPa", pres);

    display.display();

    // Send data if Wi-Fi is activated
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        // Initialize the connection to the API
        http.begin(API_URL);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("X-API-Key", API_SECRET);

        // Format the data as a JSON string
        String jsonPayload = "{\"temperature\":" + String(temp) +
                             ", \"humidity\":" + String(hum) +
                             ", \"pressure\":" + String(pres) + "}";

        // Send the HTTP POST request
        const int httpResponseCode = http.POST(jsonPayload);
        if (httpResponseCode > 0) {
            Serial.print("API Response Code: ");
            Serial.println(httpResponseCode);
        } else {
            Serial.print("Error sending POST request. Error code: ");
            Serial.println(httpResponseCode);
        }

        // Free resources
        http.end();
    } else {
        // Try reconnect if connexion is lost
        WiFi.reconnect();
    }

    // Wait for 15 min, no deep sleep as screen is lit
    delay(900000);
}