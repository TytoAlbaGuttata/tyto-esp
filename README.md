# TYTO ESP32 Sensor Node

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white) ![PlatformIO](https://img.shields.io/badge/PlatformIO-F6822B?style=for-the-badge&logo=platformio&logoColor=white) ![ESP32](https://img.shields.io/badge/ESP32-000000?style=for-the-badge&logo=espressif&logoColor=red) ![BME280](https://img.shields.io/badge/BME280-808080?style=for-the-badge)

> **⚠️ IMPORTANT:** This is the second part of the TYTO project. Please ensure you have set up the [TYTO API](https://github.com/TytoAlbaGuttata/tyto-api) on your Raspberry Pi before proceeding.

## Hardware & Wiring
This node uses an ESP32 (`esp32-c6-devkitc-1`) and a BME280 sensor to collect environmental data.

Connect the sensor's white connector to the ESP32 following this pinout:
* **+ (Red):** 3v3
* **- (Black):** GND
* **C (Blue):** Pin 4
* **D (Green):** Pin 6

![Wiring diagram](image/wiring.png)

## Prerequisites
You need an IDE compatible with PlatformIO. The recommended setup for this project is **CLion** with the PlatformIO plugin installed.

## Firmware Configuration
Before flashing the board, you must provide your network and API details. 
> **Note on Wi-Fi:** The ESP32 only supports 2.4 GHz networks. Ensure your SSID corresponds to a 2.4 GHz network, as it will not be able to connect to a 5.0 GHz one.

Create a `src/secrets.h` file with the following content:
```cpp
const char* WIFI_SSID = "WIFI_NAME";
const char* WIFI_PASSWORD = "WIFI_PASSWORD";
const char* API_URL = "http://<YOUR_PI_IP>:8000/api/measurements";
```

## Flashing & Usage

1. Connect the ESP32 to your computer using a USB-C cable. Make sure to plug it into the port labeled **UART**.
2. Using CLion:
* Click the **Hammer icon** in the toolbar to build the project.
* Click the **Green play arrow** to flash the firmware to your connected ESP32.

Once connected to the Wi-Fi, the ESP32 will read the BME280 sensor and send a POST request with the data to the API every 10 seconds.

## Contributing

This is a completed personal project. Contributions, issues, and feature requests are not accepted. However, you are completely free to fork the repository and modify it for your own personal use.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
