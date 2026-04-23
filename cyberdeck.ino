#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include "time.h"
#include <WiFiClientSecure.h>

// ==========================================================
// 1. NETWORK & LOCATION SETTINGS
// ==========================================================
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Find your Lat/Lon at latlong.net for accurate weather
String LAT = "-24.24"; 
String LON = "151.88";

// TIMEZONE CONFIGURATION:
// gmtOffset_sec: Your offset from UTC in seconds. 
// Brisbane/QLD is UTC+10, so 10 * 3600 = 36000.
// daylightOffset_sec: Set to 3600 if your area uses DST, otherwise 0.
const long  gmtOffset_sec = 36000; 
const int   daylightOffset_sec = 0; 

// ==========================================================
// 2. API KEYS & USER SPECIFIC DATA
// ==========================================================

// --- QLD FUEL SETTINGS ---
// Get your key at: https://www.fuelpricesqld.com.au/
const char* fuelApiKey = "YOUR_QLD_GOV_SUBSCRIPTION_KEY"; 
String myPostcode = "4677"; // Primary postcode for the fuel screen

// --- ENPHASE SOLAR SETTINGS ---
// Get your local Envoy IP from your router settings or Fing app
const char* envoyIP = "192.168.1.XX"; 
// Get your 1-year Token from: https://entrez.enphaseenergy.com/
const char* enphaseToken = "YOUR_LONG_LIVED_ENPHASE_TOKEN"; 
// Optional Cloud settings (Site ID and API Key from Enphase Developer Portal)
const char* enphaseSystemId = "YOUR_SITE_ID";
const char* enphaseApiKey = "YOUR_ENPHASE_DEVELOPER_API_KEY";

// ==========================================================
// 3. HARDWARE & SCREEN CONFIGURATION
// ==========================================================
#define XPT2046_IRQ  36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK  25
#define XPT2046_CS   33
#define BACKLIGHT_PIN 21 

TFT_eSPI tft = TFT_eSPI();
SPIClass touchSPI = SPIClass(VSPI);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);

// Logic Variables
int currentMode = 0; 
bool isAutoCycle = true; 
unsigned long modeTimer = 0;
const unsigned long interval = 60000; // Time per screen in milliseconds
bool modeChanged = true;

// Data Storage
float cTemp = 0, cHum = 0, cWind = 0;
unsigned long lastDataFetch = 0; 
unsigned long lastCryptoFetch = 0;
unsigned long lastFuelFetch = 0;
unsigned long lastEnphaseUpdate = 0;
const long enphaseRefreshInterval = 15000; // Refresh solar data every 15s

// Visualizer Config
#define GRID_W 48
#define GRID_H 60
uint8_t grid[GRID_W][GRID_H], nextGrid[GRID_W][GRID_H];
uint16_t hueShift = 0;
int lastCellCount = 0;
int sameCountTimer = 0;

#define MAX_STREAMS 14
struct MatrixColumn { int x; float y; float speed; int length; char lastChar; };
MatrixColumn rain[MAX_STREAMS];

// Currency Configuration (AUD Base)
const char* cur_symbols[] = {"USD", "EUR", "GBP", "JPY", "CZK", "CAD", "SEK", "SGD", "HKD", "CLP"};

void setup() {
  tft.init();
  tft.setRotation(0); 
  pinMode(BACKLIGHT_PIN, OUTPUT); digitalWrite(BACKLIGHT_PIN, HIGH);
  touchSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  ts.begin(touchSPI); ts.setRotation(0);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(0x07FF);
  tft.drawCentreString("CONNECTING TO WIFI...", 120, 160, 2);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  
  tft.fillScreen(0x07E0); tft.setTextColor(TFT_BLACK);
  tft.drawCentreString("CONNECTED!", 120, 160, 4);
  
  // Connect to NTP servers to get the time automatically
  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov");
  
  delay(1000); tft.fillScreen(TFT_BLACK);
  modeTimer = millis();
}

void loop() {
  // Touch detection to change modes or stop auto-cycle
  if (ts.touched()) { 
    isAutoCycle = false; 
    nextMode(); 
    delay(400); 
  }
  
  // Auto-cycle logic
  if (isAutoCycle && (millis() - modeTimer > interval)) nextMode();

  // Periodic Weather Update
  if (millis() - lastDataFetch > 60000 || lastDataFetch == 0) {
    updateWeather(); lastDataFetch = millis();
  }

  // Live Refresh Logic for specific screens
  if (currentMode == 2 && (millis() - lastCryptoFetch > 60000)) modeChanged = true; 

  switch (currentMode) {
    case 0: runClock(); break;
    case 1: runWeather(); break;
    case 2: runCurrency(); break;
    case 3: runMatrix(); break;
    case 4: runLife(); break;
    case 5: // Enphase Live Refresh
      if (millis() - lastEnphaseUpdate > enphaseRefreshInterval) {
        modeChanged = true;
        lastEnphaseUpdate = millis();
      }
      runEnphase(); 
      break;
    case 6: runFuel(); break;
  }
}

void nextMode() { 
  currentMode = (currentMode + 1) % 7; 
  modeChanged = true; 
  modeTimer = millis(); 
  tft.fillScreen(TFT_BLACK); 
}

// ==========================================================
// DATA FETCH FUNCTIONS
// ==========================================================

void updateWeather() {
  HTTPClient http;
  // This API is free and does not require a key
  http.begin("https://api.open-meteo.com/v1/forecast?latitude="+LAT+"&longitude="+LON+"&current=temperature_2m,relative_humidity_2m,wind_speed_10m&timezone=auto");
  if (http.GET() == 200) {
    JsonDocument doc; 
    deserializeJson(doc, http.getString());
    cTemp = doc["current"]["temperature_2m"]; 
    cHum = doc["current"]["relative_humidity_2m"]; 
    cWind = doc["current"]["wind_speed_10m"];
  }
  http.end();
}

// ... rest of visualizer functions (runMatrix, runLife, spawnLife, runClock) ...

void runEnphase() {
  static bool firstLoad = true;
  if (modeChanged && (millis() - lastEnphaseUpdate < 500)) { 
    tft.fillScreen(TFT_BLACK);
    firstLoad = true;
  }

  WiFiClientSecure client;
  client.setInsecure(); // Needed for Envoy box's internal SSL certificate
  HTTPClient http;
  http.setTimeout(8000); 

  String url = "https://" + String(envoyIP) + "/production.json?details=1";
  http.begin(client, url);
  http.addHeader("Authorization", "Bearer " + String(enphaseToken));

  int httpCode = http.GET();

  if (httpCode == 200) {
    DynamicJsonDocument doc(4096);
    deserializeJson(doc, http.getString());

    if (firstLoad) {
      tft.setTextColor(0xFDA0); 
      tft.drawCentreString("SOLAR & GRID STATUS", 120, 5, 2);
      tft.drawFastHLine(0, 25, 240, 0x07FF);
      tft.setTextColor(0xFFFF); 
      tft.drawString("PRODUCING:", 10, 40, 2);
      tft.drawString("CONSUMING:", 10, 80, 2);
      tft.drawFastHLine(20, 115, 200, 0x2104);
      tft.drawFastHLine(0, 210, 240, 0x07FF);
      firstLoad = false;
    }

    float prodW = doc["production"][1]["wNow"];      
    float consW = doc["consumption"][0]["wNow"];     
    float netW = prodW - consW; 

    tft.setTextColor(0x07E0, TFT_BLACK); 
    tft.drawRightString(String(prodW / 1000, 2) + " kW ", 230, 40, 4);

    tft.setTextColor(0xF800, TFT_BLACK); 
    tft.drawRightString(String(consW / 1000, 2) + " kW ", 230, 80, 4);

    tft.fillRect(0, 130, 240, 70, TFT_BLACK); 
    int netY = 135;
    if (netW >= 0) {
      tft.setTextColor(0x07FF); 
      tft.drawCentreString("EXPORTING TO GRID", 120, netY, 2);
      tft.drawCentreString(String(netW / 1000, 2) + " kW", 120, netY + 25, 6);
    } else {
      tft.setTextColor(0xF81F); 
      tft.drawCentreString("IMPORTING FROM GRID", 120, netY, 2);
      tft.drawCentreString(String(abs(netW) / 1000, 2) + " kW", 120, netY + 25, 6);
    }

    float todayProdKwh = (float)doc["production"][1]["whToday"] / 1000;
    float todayConsKwh = (float)doc["consumption"][0]["whToday"] / 1000;
    tft.setTextColor(0x7BEF, TFT_BLACK);
    tft.drawString("Yield: " + String(todayProdKwh, 1) + "kWh ", 10, 215, 2);
    tft.drawRightString("Used: " + String(todayConsKwh, 1) + "kWh ", 230, 215, 2);

    modeChanged = false;
    lastEnphaseUpdate = millis(); 
  } 
  else {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED);
    tft.drawCentreString("ENVOY ERROR", 120, 100, 4);
    tft.setTextColor(0xFFFF);
    tft.drawCentreString("HTTP: " + String(httpCode), 120, 160, 2);
    delay(5000);
    nextMode();
  }
  http.end();
}
