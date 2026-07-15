#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

// GPIO pins
#define I2C_SDA 6
#define I2C_SCL 4

Adafruit_BME280 bme;

void setup() {
    // Initiate communication
    Serial.begin(115200);
    delay(2000); // Let the port open

    Serial.println("\n--- Connexion start ---");

    // Start I2C bus on the pins
    Wire.begin(I2C_SDA, I2C_SCL);

    // Sensor initialization
    if (!bme.begin(0x77, &Wire)) {
        Serial.println("Error : BME280 not found.");
        while (1); // If error, stop
    }

    Serial.println("Sensor detected.");
}

void loop() {
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" °C");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.print("Pressure = ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.println("-------------------------");

    delay(2000); // Wait 2 seconds until next read
}