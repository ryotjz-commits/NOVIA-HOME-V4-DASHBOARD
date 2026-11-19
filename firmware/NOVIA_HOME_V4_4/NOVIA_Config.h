
/*
 ============================================================
                    NOVIA HOME V4.4
                 PREMIUM SMART HOME SYSTEM
 ============================================================

 BOARD       : ESP32 DEVKIT V1
 FLASH       : 4 MB
 IDE         : ArduinoDroid
 CONTROLLER  : BLYNK IoT
 WEB SERVER  : HTTP PORT 80
 WIFI        : AP + STA
 STORAGE     : Preferences
 RELAY       : ACTIVE HIGH

 FEATURES:
 - 4 Channel Relay
 - 4 Physical Toggle Buttons
 - Blynk IoT V0-V3
 - Web Dashboard Premium
 - Hybrid Dark / Light Theme
 - Digital Clock and Date
 - Relay Statistics
 - WiFi Scanner Asynchronous
 - AP + STA
 - WiFi Auto Reconnect
 - Blynk Auto Reconnect
 - WiFi Preferences
 - Real-time Device Status
 - Responsive Mobile Dashboard
 - All Relay OFF After Restart (ACTIVE HIGH)
 - Safe relay output-latch initialization
 - Physical / Web / Blynk state synchronization
 - Loop task watchdog recovery

 RELAY:
 CH1 = GPIO26
 CH2 = GPIO27
 CH3 = GPIO13
 CH4 = GPIO14

 BUTTON:
 CH1 = GPIO32
 CH2 = GPIO33
 CH3 = GPIO25
 CH4 = GPIO23

 BLYNK:
 V0 = CH1
 V1 = CH2
 V2 = CH3
 V3 = CH4

 ============================================================
*/


#pragma once

// ============================================================
// 1. BLYNK CONFIGURATION
// ============================================================

#define BLYNK_TEMPLATE_ID "TMPL6Yk8Hycr-"
#define BLYNK_TEMPLATE_NAME "Smart Home"
#if __has_include("secrets.h")
  #include "secrets.h"
#endif

#ifndef BLYNK_AUTH_TOKEN
  #define BLYNK_AUTH_TOKEN ""
#endif

#define BLYNK_PRINT Serial

// ============================================================
// 2. LIBRARIES
// ============================================================

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <BlynkSimpleEsp32.h>
#include <esp_task_wdt.h>
#include <esp_idf_version.h>
#include <time.h>

// ============================================================
// 3. HARDWARE CONFIGURATION
// ============================================================

#define RELAY_ON  HIGH
#define RELAY_OFF LOW

#define RELAY_COUNT 4

const uint8_t relayPins[4] = {
  26, 27, 13, 14
};

const uint8_t buttonPins[4] = {
  32, 33, 25, 23
};

const char* channelNames[4] = {
  "Lampu Depan",
  "Lampu Tengah",
  "Lampu Kamar",
  "Kipas Angin"
};

// ============================================================
// 4. NETWORK CONFIGURATION
// ============================================================

// AP credentials are loaded from Preferences (NVS). Defaults are used
// only on first boot or after an explicit NVS/factory erase.
static const char DEFAULT_AP_SSID[] = "NOVIA-HOME";
static const char DEFAULT_AP_PASS[] = "Noke0710"; // Change after first login
String apSSID = DEFAULT_AP_SSID;
String apPASS = DEFAULT_AP_PASS;

// Local browser dashboard uses the standard ESP32 SoftAP subnet.
IPAddress apIP(192, 168, 4, 1);
IPAddress apGateway(192, 168, 4, 1);
IPAddress apSubnet(255, 255, 255, 0);
WebServer server(80);
Preferences preferences;

// ============================================================
// 5. TIMING CONFIGURATION
// ============================================================

const unsigned long DEBOUNCE_TIME = 50;

const unsigned long WIFI_INTERVAL = 15000;

const unsigned long BLYNK_INTERVAL = 5000;

const unsigned long STATUS_INTERVAL = 1000;
const unsigned long STATS_SAVE_INTERVAL = 3600000UL; // Save accumulated usage hourly to limit NVS wear
const long GMT_OFFSET_SECONDS = 7L * 3600L; // WIB (Bogor/Jakarta)
const int DAYLIGHT_OFFSET_SECONDS = 0;

unsigned long lastWiFiAttempt = 0;
unsigned long lastBlynkAttempt = 0;
unsigned long lastStatusLog = 0;
unsigned long lastStatsSave = 0;
unsigned long lastTimerPersist = 0;
unsigned long timerDeadline[4] = {0,0,0,0};
uint32_t timerSavedSeconds[4] = {0,0,0,0};
uint32_t timerEpochDeadline[4] = {0,0,0,0};
bool timerEpochRecovered[4] = {false,false,false,false};
unsigned long timerEpochPendingSince[4] = {0,0,0,0};
unsigned long onStartedAt[4] = {0,0,0,0};
uint32_t totalOnSeconds[4] = {0,0,0,0};
bool scheduleEnabled[4] = {false,false,false,false};
uint16_t scheduleOnMinute[4] = {0,0,0,0};
uint16_t scheduleOffMinute[4] = {0,0,0,0};
bool autoMode = false;
int lastScheduleMinute = -1;

// Loop-task watchdog. Compatible with ESP-IDF 4.x and 5.x.
static const uint32_t LOOP_WDT_TIMEOUT_MS = 10000;
bool loopWatchdogEnabled = false;

// ============================================================
// 6. CHANNEL VARIABLES
// ============================================================

bool channelState[4] = {
  false, false, false, false
};

// ============================================================
// 7. BUTTON VARIABLES
// ============================================================

bool buttonStable[4] = {
  HIGH, HIGH, HIGH, HIGH
};

bool buttonLast[4] = {
  HIGH, HIGH, HIGH, HIGH
};

unsigned long buttonTimer[4] = {
  0, 0, 0, 0
};

// ============================================================
// 8. WIFI VARIABLES
// ============================================================

String wifiSSID = "";
String wifiPASS = "";

bool wifiConfigured = false;

// ============================================================
// 9. SCANNER VARIABLES
// ============================================================

bool scanRunning = false;

// ============================================================
// 10. FUNCTION DECLARATIONS
// ============================================================

void applyChannel(uint8_t ch, bool state,
                  bool updateBlynk = true);

void syncAllChannels();

void handleButtons();

void loadWiFiConfig();
void saveWiFiConfig(String ssid, String pass);
void loadAPConfig();
void saveAPConfig(String ssid, String pass);
void startAccessPoint();

void connectWiFi();

void handleWiFi();

void handleBlynk();

void setupWebServer();

void handleState();

void handleSetChannel();

void handleScanStart();

void handleScanStatus();

void handleSaveWiFi();

void handleRoot();
bool requireLocalClient();
void handleSaveAP();

void handleStatusLog();
void setupLoopWatchdog();
void loadAutomationConfig();
void saveAutomationConfig();
void handleAutomation();
void handleTimer();
void handleSchedule();
void handleMode();
void loadPersistentTimers();
void handlePersistentTimerRecovery();
void savePersistentTimer(uint8_t index, uint32_t seconds, uint32_t epochDeadline);
void saveUsageStats();
void startTimeSync();

String jsonEscape(String value);

