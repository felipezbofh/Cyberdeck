# 📟 CYD Cyberdeck: Queensland IoT Dashboard

A multi-functional IoT dashboard built for the **ESP32-2432S028** (Cheap Yellow Display). This project features a series of "Cyber" themed screens providing real-time data for local weather, global finance, energy monitoring, and QLD fuel prices.

---

## 🚀 Features

* **Cyber Clock:** Rainbow progress bar with local AEST time.
* **Weather HUD:** Live temperature, humidity, and wind speed using local coordinates.
* **Forex Terminal:** 10+ Global currencies including **CLP** and **JPY** relative to **AUD**.
* **Enphase Solar Monitor:** Real-time production, house consumption, and grid status (updates every 15s).
* **QLD Fuel Watch:** Live Unleaded 91 and Diesel prices for your specific postcode.
* **Visualizers:** Matrix Digital Rain and Conway's Game of Life.
* **Auto-Cycle:** Rotates screens every 60 seconds; touch to freeze/manual cycle.

---

## 🛠️ Hardware Requirements

1.  **ESP32-2432S028** (The "Cheap Yellow Display" / CYD).
2.  **MicroUSB Cable** for power and programming.
3.  **2.4GHz WiFi Connection.**

---

## ⚙️ Software & Setup

### 1. Arduino IDE Configuration
* Add the ESP32 Board URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
* Install the **ESP32 by Espressif Systems** board package.
* **Board:** `ESP32 Dev Module`.
* **Libraries:** `TFT_eSPI`, `ArduinoJson`, `XPT2046_Touchscreen`.

### 2. Localization (Lat/Long & Time)
Update these in the code to match your home:
* **Coordinates:** Go to [Google Maps](https://www.google.com/maps), right-click your home, and copy the numbers.
    * `String LAT = "-27.46";`
    * `String LON = "153.02";`
* **Timezone:** Calculate your GMT offset (Hours * 3600).
    * *QLD (UTC+10):* `36000`.
* **DST:** Set `daylightOffset_sec` to `3600` if you have daylight savings, or `0` if you don't.

---

## 🔑 API & Token Configuration

### ⛽ QLD Fuel API
1.  Register at [Fuel Price QLD](https://www.fuelpricesqld.com.au/).
2.  In your profile, copy the **Primary Key** (Ocp-Apim-Subscription-Key).
3.  Set `myPostcode` to your 4-digit code (e.g., `"4000"`).

### ☀️ Enphase Solar (Local API)
1.  **Gateway IP:** Find your Envoy/IQ Gateway IP on your network (e.g., `192.168.1.XX`).
2.  **Entrez Portal:** Go to [entrez.enphaseenergy.com](https://entrez.enphaseenergy.com/).
3.  **The "Select System" Trick:** * Do **NOT** enter your Site ID number.
    * Find your **System Name** in the Enphase App under *Menu > System > Site Details* (e.g., "Smith Residence").
    * Type the first 3 letters of that name into the box and **wait for the dropdown**.
    * You **MUST click the name** from the dropdown list for the "Gateway" box to appear.
4.  **Create Token:** Copy the massive string into the `enphaseToken` variable.

---

## 🖱️ Navigation & Error Handling
* **Touch Screen:** Tapping the screen stops the auto-cycle and enters **Manual Mode**. Each tap then advances the screen.
* **Auto-Advance on Error:** If the Fuel or Enphase API fails (e.g., internet out), the screen will display the error for 5 seconds and then automatically skip to the next screen so your dashboard doesn't get stuck.

---

## 📝 Configuration Summary

| Variable | Description |
| :--- | :--- |
| `ssid` / `password` | Your 2.4GHz WiFi credentials |
| `gmtOffset_sec` | UTC offset in seconds (e.g., 36000 for AEST) |
| `fuelApiKey` | Your QLD Government Subscriber Key |
| `enphaseToken` | Your 1-Year JWT Token from Entrez |
| `envoyIP` | Local IP of your Enphase Gateway |
| `cur_symbols` | Array of 3-letter currency codes (e.g., "USD", "EUR") |

---

## ⚖️ License
This project is open-source. Build it, track your power, and enjoy the matrix rain.
