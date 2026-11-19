/*
  NOVIA HOME V4.4 - MODULAR EDITION
  Refactor of the user-provided V4.3 source.
  Board: ESP32 DevKit V1
  Relay logic: ACTIVE HIGH
  Web access: AP password + manual browser URL (no captive redirect)
*/
#include "NOVIA_Config.h"
#include "NOVIA_Core.h"
#include "NOVIA_Automation.h"
#include "NOVIA_Network.h"
#include "NOVIA_Dashboard.h"
#include "NOVIA_Web.h"

// ============================================================
// 31. SETUP
// ============================================================

void setup() {

  Serial.begin(115200);

  delay(300);

  Serial.println();

  Serial.println("==================================");
  Serial.println("       NOVIA HOME V4.4");
  Serial.println("     PREMIUM SMART HOME SYSTEM");
  Serial.println("==================================");

  // ------------------------------------------
  // INITIALIZE RELAY
  // ------------------------------------------

  for (uint8_t i = 0; i < RELAY_COUNT; i++) {

    // ACTIVE HIGH relay: preload  before enabling OUTPUT.
    // This reduces software-induced ON pulses during initialization.
    digitalWrite(relayPins[i], RELAY_OFF);
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], RELAY_OFF);

    channelState[i] = false;
  }

  Serial.println("[NOVIA] RELAYS INITIALIZED OFF");

  // ------------------------------------------
  // INITIALIZE BUTTON
  // ------------------------------------------

  for(uint8_t i=0;i<RELAY_COUNT;i++) {

    pinMode(
      buttonPins[i],
      INPUT_PULLUP
    );

    buttonStable[i] = digitalRead(buttonPins[i]);

    buttonLast[i] = buttonStable[i];

    buttonTimer[i] = millis();
  }

  Serial.println("[NOVIA] BUTTONS INITIALIZED");

  // ------------------------------------------
  // LOAD WIFI CONFIGURATION
  // ------------------------------------------

  loadWiFiConfig();
  loadAPConfig();
  loadAutomationConfig();
  loadPersistentTimers();

  // ------------------------------------------
  // AP + STA MODE
  // ------------------------------------------

  WiFi.mode(WIFI_AP_STA);

  WiFi.setAutoReconnect(true);
  startTimeSync();

  startAccessPoint();

  // ------------------------------------------
  // START WEB SERVER
  // ------------------------------------------

  setupWebServer();

  // ------------------------------------------
  // BLYNK CONFIGURATION
  // ------------------------------------------

  Blynk.config(BLYNK_AUTH_TOKEN);

  // ------------------------------------------
  // CONNECT WIFI
  // ------------------------------------------

  if(wifiConfigured) {

    connectWiFi();

  } else {

    Serial.println("[NOVIA] WIFI NOT CONFIGURED");
    Serial.println("[NOVIA] USE AP TO CONFIGURE WIFI");

  }

  Serial.println("==================================");
  Serial.println("       NOVIA HOME V4.4 READY");
  Serial.println("==================================");

  // Enable last so setup/network initialization cannot starve the WDT.
  setupLoopWatchdog();
}

// ============================================================
// 32. MAIN LOOP
// ============================================================

void loop() {

  // Feed only while the main loop is responsive.
  if (loopWatchdogEnabled) {
    esp_task_wdt_reset();
  }

  // Browser-only local dashboard (no captive DNS or automatic redirect).
  server.handleClient();

  // Physical buttons

  handleButtons();

  // WiFi manager

  handleWiFi();

  // Blynk manager

  handleBlynk();

  // Timers, daily schedules, and persistent usage statistics
  handleAutomation();

  // Status serial monitoring

  if(millis()-lastStatusLog >= STATUS_INTERVAL) {

    lastStatusLog = millis();

    // Keep loop responsive.
  }

  yield();
}

