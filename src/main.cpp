#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.h"

#define I2C_SDA 6
#define I2C_SCL 4

Adafruit_BME280 bme;

void setup() {
    // Initiate communication
    Serial.begin(115200);
    delay(2000);

    Serial.println("\n--- TYTO System Booting ---");

    // Connect to Wi-fi
    Serial.print("Connecting to network: ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWi-Fi connected successfully");
    Serial.print("ESP32 Local IP: ");
    Serial.println(WiFi.localIP());
    Serial.println("-------------------------");

    // Start I2C bus on the pins
    Wire.begin(I2C_SDA, I2C_SCL);

    // Sensor initialization
    if (!bme.begin(0x77, &Wire)) {
        Serial.println("Error: BME280 sensor not found.");
        while (1);
    }
    Serial.println("BME280 sensor initialized successfully.");
    Serial.println("-------------------------");
}

void loop() {
    // 1. Read sensor values
    float temp = bme.readTemperature();
    float hum = bme.readHumidity();
    float pres = bme.readPressure() / 100.0F;

    Serial.printf("Temp: %.2f *C | Hum: %.2f %% | Pres: %.2f hPa\n", temp, hum, pres);

    // Check Wi-Fi connection before sending
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        // Initialize the connection to the API
        http.begin(API_URL);
        http.addHeader("Content-Type", "application/json");

        // Format the data as a JSON string
        String jsonPayload = "{\"temperature\":" + String(temp) +
                             ", \"humidity\":" + String(hum) +
                             ", \"pressure\":" + String(pres) + "}";

        // Send the HTTP POST request
        int httpResponseCode = http.POST(jsonPayload);

        // Display the result
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
        Serial.println("Error: Wi-Fi disconnected");
    }

    Serial.println("-------------------------");
    delay(10000); // Send data every 10 seconds to avoid spamming the DB
}