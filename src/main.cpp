#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.h"

// Sleep configuration (15 min)
#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP  900

#define I2C_SDA 6
#define I2C_SCL 4

Adafruit_BME280 bme;

void setup() {
    // Initiate communication
    Serial.begin(115200);
    Serial.println("\n--- TYTO System Booting ---");

    // I2C and sensor initialization
    Wire.begin(I2C_SDA, I2C_SCL);
    if (!bme.begin(0x77, &Wire)) {
        Serial.println("Error: BME280 sensor not found.");
        esp_deep_sleep_start(); // Sleep if sensor does not respond
    }
    Serial.println("BME280 sensor initialized successfully.");
    Serial.println("-------------------------");

    // Wi-Fi connexion
    Serial.print("Connecting to network: ");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWi-Fi connected successfully");
    Serial.print("ESP32 Local IP: ");
    Serial.println(WiFi.localIP());
    Serial.println("-------------------------");

    // Read sensors
    const float temp = bme.readTemperature();
    const float hum = bme.readHumidity();
    const float pres = bme.readPressure() / 100.0F;

    Serial.printf("Temp: %.2f *C | Hum: %.2f %% | Pres: %.2f hPa\n", temp, hum, pres);

    // Send data if Wi-Fi is connecter
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
    }

    // Deep sleep configuration
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    Serial.println("Going to deep sleep for 15 minutes...");
    Serial.flush();
    esp_deep_sleep_start();
}

void loop() {
    // Nothing to do here as dee sleep only executes setup.
}