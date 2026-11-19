#pragma once

// ============================================================
// ACCESS POINT CONFIGURATION LOAD / SAVE
// ============================================================

void loadAPConfig() {
  preferences.begin("novia", true);
  apSSID = preferences.getString("ap_ssid", DEFAULT_AP_SSID);
  apPASS = preferences.getString("ap_pass", DEFAULT_AP_PASS);
  preferences.end();

  // Recover safely if stored values are invalid.
  if (apSSID.length() == 0 || apSSID.length() > 32) apSSID = DEFAULT_AP_SSID;
  if (apPASS.length() < 8 || apPASS.length() > 63) apPASS = DEFAULT_AP_PASS;

  Serial.print("[NOVIA] AP SSID LOADED: ");
  Serial.println(apSSID);
}

void saveAPConfig(String ssid, String pass) {
  preferences.begin("novia", false);
  preferences.putString("ap_ssid", ssid);
  preferences.putString("ap_pass", pass);
  preferences.end();

  apSSID = ssid;
  apPASS = pass;
  Serial.println("[NOVIA] AP CONFIG SAVED TO NVS");
}

// ============================================================
// ACCESS POINT FOR MANUAL BROWSER ACCESS
// ============================================================

void startAccessPoint() {
  // Keep both interfaces enabled. Starting/restarting STA must never
  // switch the ESP32 to WIFI_STA-only mode.
  WiFi.mode(WIFI_AP_STA);

  // Configure the NOVIA SoftAP on its fixed local subnet.
  WiFi.softAPConfig(apIP, apGateway, apSubnet);

  bool started = WiFi.softAP(apSSID.c_str(), apPASS.c_str());

  if (started) {
    Serial.println("[NOVIA] AP STARTED - AP + STA MODE");
    Serial.print("[NOVIA] AP SSID: ");
    Serial.println(apSSID);
    Serial.print("[NOVIA] AP BROWSER URL: http://");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("[NOVIA] ERROR: SOFT AP START FAILED");
  }
}



// ============================================================
// 16. WIFI CONFIGURATION LOAD
// ============================================================

void loadWiFiConfig() {

  preferences.begin("novia", true);

  wifiSSID = preferences.getString("ssid", "");
  wifiPASS = preferences.getString("pass", "");

  preferences.end();

  wifiConfigured = wifiSSID.length() > 0;

  Serial.println("[NOVIA] WIFI CONFIG LOADED");

  if (wifiConfigured) {

    Serial.print("[NOVIA] SSID: ");
    Serial.println(wifiSSID);
  }
}

// ============================================================
// 17. WIFI CONFIGURATION SAVE
// ============================================================

void saveWiFiConfig(String ssid, String pass) {

  preferences.begin("novia", false);

  preferences.putString("ssid", ssid);
  preferences.putString("pass", pass);

  preferences.end();

  wifiSSID = ssid;
  wifiPASS = pass;

  wifiConfigured = true;

  // Stop an existing scan before reconnecting.

  if (scanRunning) {

    WiFi.scanDelete();

    scanRunning = false;
  }

  // Disconnect only the station interface; keep the NOVIA SoftAP running.
  WiFi.disconnect(false, false);

  lastWiFiAttempt = 0;

  Serial.println("[NOVIA] WIFI CONFIG SAVED");
}

// ============================================================
// 18. WIFI CONNECTION
// ============================================================

void connectWiFi() {
  if (!wifiConfigured) return;

  // Explicitly preserve AP + STA mode before starting the router connection.
  WiFi.mode(WIFI_AP_STA);

  Serial.println("[NOVIA] CONNECTING ROUTER WIFI (AP REMAINS ACTIVE)");

  // WiFi.begin() starts/updates only the station connection.
  // It does not intentionally disable the NOVIA SoftAP.
  WiFi.begin(wifiSSID.c_str(), wifiPASS.c_str());

  lastWiFiAttempt = millis();
}

// ============================================================
// 19. WIFI AUTO RECONNECT
// ============================================================

void handleWiFi() {
  static bool staConnectedLogged = false;
  static unsigned long lastAPCheck = 0;

  // Safety check: recover the NOVIA AP if it was stopped unexpectedly.
  // Do this infrequently so the AP is not needlessly restarted.
  if (millis() - lastAPCheck >= 5000UL) {
    lastAPCheck = millis();

    if (WiFi.getMode() != WIFI_AP_STA) {
      WiFi.mode(WIFI_AP_STA);
    }

    if (WiFi.softAPIP() != apIP) {
      Serial.println("[NOVIA] AP NOT AVAILABLE - RESTARTING SOFTAP");
      startAccessPoint();
    }
  }

  if (!wifiConfigured) return;

  if (WiFi.status() == WL_CONNECTED) {
    if (!staConnectedLogged) {
      Serial.println("[NOVIA] ROUTER WIFI CONNECTED");
      Serial.print("[NOVIA] ROUTER IP / BROWSER URL: http://");
      Serial.println(WiFi.localIP());
      Serial.print("[NOVIA] AP REMAINS ACTIVE - AP URL: http://");
      Serial.println(WiFi.softAPIP());
      Serial.print("[NOVIA] GATEWAY: ");
      Serial.println(WiFi.gatewayIP());
      staConnectedLogged = true;
    }
    return;
  }

  staConnectedLogged = false;

  // Avoid reconnect during scan; AP remains enabled in AP+STA mode.
  if (scanRunning) return;

  if (millis() - lastWiFiAttempt >= WIFI_INTERVAL) {
    Serial.println("[NOVIA] WIFI RECONNECTING (SOFTAP KEPT ACTIVE)");

    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(wifiSSID.c_str(), wifiPASS.c_str());

    lastWiFiAttempt = millis();
  }
}

// ============================================================
// 20. BLYNK AUTO RECONNECT
// ============================================================

void handleBlynk() {

  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  if (Blynk.connected()) {

    Blynk.run();

    return;
  }

  if (millis() - lastBlynkAttempt >= BLYNK_INTERVAL) {

    Serial.println("[NOVIA] CONNECTING BLYNK");

    lastBlynkAttempt = millis();

    // Short connection timeout.

    Blynk.connect(100);
  }
}

