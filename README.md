# 📟 CYD Cyberdeck: Gladstone & Agnes Water Edition

A multi-functional IoT dashboard built for the **ESP32-2432S028** (Cheap Yellow Display). This project features a series of "Cyber" themed screens providing real-time data for local weather, global finance, energy monitoring, and Gladstone region fuel prices.

---

## 🚀 Features

* **Cyber Clock:** Rainbow progress bar with local AEST time.
* **Weather HUD:** Live temperature, humidity, and wind speed for Agnes Water/Gladstone.
* **Forex Terminal:** 10+ Global currencies including **CLP** and the **Chilean UF** index.
* **Crypto Terminal:** Live tracking of major cryptocurrencies.
* **Enphase Solar Monitor:** Real-time production, consumption, and grid import/export data (updates every 15s).
* **QLD Fuel Watch:** Live Unleaded 91 and Diesel prices for postcode **4680** (Gladstone) or **4677** (Agnes Water).
* **Visualizers:** Matrix Digital Rain and Conway's Game of Life.

---

## 🛠️ Hardware Requirements

1.  **ESP32-2432S028** (The "Cheap Yellow Display" / CYD).
2.  **MicroUSB Cable** for power and programming.
3.  **WiFi Connection** (2.4GHz).

---

## ⚙️ Software Setup

### 1. Arduino IDE Configuration
* Install **Arduino IDE** (v2.0+ recommended).
* Add the ESP32 Board URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
* Install the **ESP32 by Espressif Systems** board package in Board Manager.
* **Select Board:** `ESP32 Dev Module`.

### 2. Required Libraries
Install these via the Arduino Library Manager:
* `TFT_eSPI` (Note: You must configure `User_Setup.h` for the CYD pinout).
* `ArduinoJson` (v6 or v7).
* `XPT2046_Touchscreen`.
* `WiFiClientSecure` (Built-in).

### 3. Pin Configuration (`TFT_eSPI`)
Ensure your `User_Setup.h` matches these pins:
```cpp
#define ILI9341_DRIVER
#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST  12
#define TFT_BL   21
