#pragma once

// ============================================================
// 21. ACCESS CONTROL: LOCAL AP + HOME ROUTER CLIENTS
// Dashboard can be opened from either NOVIA AP or the same LAN
// as the router connection (STA). No access from unrelated networks.
// ============================================================

bool isInSubnet(IPAddress address, IPAddress network, IPAddress mask) {
  for (uint8_t i = 0; i < 4; i++) {
    if ((address[i] & mask[i]) != (network[i] & mask[i])) return false;
  }
  return true;
}

bool requireLocalClient() {
  IPAddress remote = server.client().remoteIP();

  // Clients connected directly to NOVIA HOME AP.
  if (isInSubnet(remote, apIP, apSubnet)) return true;

  // Clients connected to the same router/LAN as ESP32 (STA).
  if (WiFi.status() == WL_CONNECTED) {
    IPAddress staIP = WiFi.localIP();
    IPAddress staMask = WiFi.subnetMask();
    if (isInSubnet(remote, staIP, staMask)) return true;
  }

  server.send(403, "text/plain", "ACCESS ALLOWED FROM NOVIA AP OR HOME LAN ONLY");
  return false;
}


// ============================================================
// 22. JSON ESCAPE
// ============================================================

String jsonEscape(String value) {

  value.replace("\\", "\\\\");
  value.replace("\"", "\\\"");
  value.replace("\n", "\\n");
  value.replace("\r", "\\r");
  value.replace("\t", "\\t");

  return value;
}

// ============================================================
// 23. DEVICE STATE API
// ============================================================

void handleState() {

  String json = "{";

  json += "\"wifi\":";

  json += WiFi.status() == WL_CONNECTED
          ? "true" : "false";

  json += ",\"blynk\":";

  json += Blynk.connected()
          ? "true" : "false";

  json += ",\"ip\":\"";

  if (WiFi.status() == WL_CONNECTED) {

    json += WiFi.localIP().toString();

  } else {

    json += WiFi.softAPIP().toString();
  }

  json += "\",\"rssi\":";

  json += WiFi.status() == WL_CONNECTED
          ? String(WiFi.RSSI()) : "0";

  json += ",\"apSSID\":\"";
  json += jsonEscape(apSSID);
  json += "\",\"apIP\":\"";
  json += WiFi.softAPIP().toString();
  json += "\"";

  uint8_t active = 0;

  for (uint8_t i = 0; i < RELAY_COUNT; i++) {

    if (channelState[i]) active++;
  }

  json += ",\"active\":";
  json += String(active);

  json += ",\"channels\":[";

  for (uint8_t i = 0; i < RELAY_COUNT; i++) {

    json += channelState[i] ? "1" : "0";

    if (i < RELAY_COUNT - 1) {
      json += ",";
    }
  }

  json += "],\"mode\":\"";
  json += autoMode ? "AUTO" : "MANUAL";
  json += "\",\"timeSynced\":";
  struct tm timeinfo;
  json += getLocalTime(&timeinfo, 5) ? "true" : "false";
  json += ",\"timers\":[";
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    uint32_t remaining = timerDeadline[i] == 0 ? 0 : (long)(timerDeadline[i] - millis()) > 0 ? (uint32_t)((timerDeadline[i] - millis()) / 1000UL) : 0;
    json += String(remaining);
    if (i < RELAY_COUNT - 1) json += ",";
  }
  json += "],\"usageSeconds\":[";
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    uint32_t used = totalOnSeconds[i];
    if (channelState[i]) used += (uint32_t)((millis() - onStartedAt[i]) / 1000UL);
    json += String(used);
    if (i < RELAY_COUNT - 1) json += ",";
  }
  json += "],\"scheduleEnabled\":[";
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    json += scheduleEnabled[i] ? "true" : "false";
    if (i < RELAY_COUNT - 1) json += ",";
  }
  json += "],\"scheduleOn\":[";
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    json += String(scheduleOnMinute[i]);
    if (i < RELAY_COUNT - 1) json += ",";
  }
  json += "],\"scheduleOff\":[";
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    json += String(scheduleOffMinute[i]);
    if (i < RELAY_COUNT - 1) json += ",";
  }
  json += "]}";

  server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate");
  server.send(
    200,
    "application/json",
    json
  );
}

// ============================================================
// 24. CHANNEL CONTROL API
// ============================================================

void handleSetChannel() {

  if (!server.hasArg("ch") ||
      !server.hasArg("state")) {

    server.send(
      400,
      "application/json",
      "{\"ok\":false,\"error\":\"INVALID REQUEST\"}"
    );

    return;
  }

  String chArg = server.arg("ch");
  String stateArg = server.arg("state");

  if (chArg.length() != 1 ||
      stateArg.length() != 1) {

    server.send(
      400,
      "application/json",
      "{\"ok\":false,\"error\":\"INVALID VALUE\"}"
    );

    return;
  }

  if ((chArg != "1" && chArg != "2" && chArg != "3" && chArg != "4") ||
      (stateArg != "0" && stateArg != "1")) {
    server.send(400, "application/json",
                "{\" ok\":false,\" error\":\" INVALID VALUE\"}");
    return;
  }

  int ch = chArg.toInt();
  int state = stateArg.toInt();

  if (ch < 1 || ch > RELAY_COUNT ||
      (state != 0 && state != 1)) {

    server.send(
      400,
      "application/json",
      "{\"ok\":false,\"error\":\"OUT OF RANGE\"}"
    );

    return;
  }

  applyChannel(ch - 1, state == 1, true);

  server.send(
    200,
    "application/json",
    "{\"ok\":true}"
  );
}

// ============================================================
// 25. ASYNCHRONOUS WIFI SCAN START
// ============================================================

void handleScanStart() {

  if (scanRunning) {
    server.send(409, "application/json",
                "{\"ok\":false,\"message\":\"SCAN RUNNING\"}");
    return;
  }

  // Keep the AP available for the phone while enabling STA scanning.
  WiFi.mode(WIFI_AP_STA);
  WiFi.scanDelete();
  delay(30);

  // async=true, show_hidden=true, passive=false, 300 ms/channel.
  // Showing hidden entries is useful for diagnostics; empty SSIDs are
  // marked as hidden by the dashboard and cannot be auto-selected.
  int result = WiFi.scanNetworks(true, true, false, 300, 0);

  Serial.printf("[WIFI SCAN] start result=%d\n", result);

  if (result == WIFI_SCAN_FAILED ||
      (result < 0 && result != WIFI_SCAN_RUNNING)) {
    scanRunning = false;
    server.send(503, "application/json",
                "{\"ok\":false,\"message\":\"SCAN START FAILED\"}");
    return;
  }

  // Even if the core returns an already-completed count, let the status
  // endpoint consume the result through scanComplete().
  scanRunning = true;
  server.send(202, "application/json",
              "{\"ok\":true,\"scanning\":true}");
}

// ============================================================
// 26. ASYNCHRONOUS WIFI SCAN STATUS
// ============================================================

void handleScanStatus() {

  if (!scanRunning) {
    server.send(200, "application/json",
                "{\"scanning\":false,\"networks\":[],\"count\":0}");
    return;
  }

  int result = WiFi.scanComplete();

  if (result == WIFI_SCAN_RUNNING) {
    server.send(200, "application/json",
                "{\"scanning\":true,\"networks\":[]}");
    return;
  }

  if (result < 0) {
    Serial.printf("[WIFI SCAN] failed result=%d\n", result);
    scanRunning = false;
    WiFi.scanDelete();
    server.send(503, "application/json",
                "{\"scanning\":false,\"error\":\"SCAN FAILED\"}");
    return;
  }

  String json;
  json.reserve(128 + (size_t)result * 100);
  json = "{\"scanning\":false,\"count\":";
  json += String(result);
  json += ",\"networks\":[";

  for (int i = 0; i < result; i++) {
    if (i > 0) json += ",";

    String ssid = WiFi.SSID(i);
    json += "{\"ssid\":\"";
    json += jsonEscape(ssid);
    json += "\",\"hidden\":";
    json += ssid.length() == 0 ? "true" : "false";
    json += ",\"rssi\":";
    json += String(WiFi.RSSI(i));
    json += ",\"channel\":";
    json += String(WiFi.channel(i));
    json += ",\"secure\":";
    json += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)
              ? "false" : "true";
    json += "}";
  }

  json += "]}";

  Serial.printf("[WIFI SCAN] completed: %d network(s)\n", result);
  WiFi.scanDelete();
  scanRunning = false;

  server.send(200, "application/json", json);
}

// ============================================================
// 27. WIFI SAVE HANDLER
// ============================================================

void handleSaveWiFi() {

  String ssid = server.arg("ssid");
  String pass = server.arg("pass");
  ssid.trim();

  if (ssid.length() == 0 ||
      ssid.length() > 32 ||
      pass.length() > 63) {

    server.send(
      400,
      "text/plain",
      "INVALID WIFI CONFIGURATION"
    );

    return;
  }

  saveWiFiConfig(ssid, pass);

  server.send(
    200,
    "text/html",
    "<!DOCTYPE html><html lang='id'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<body style='background:#151a1d;color:white;font-family:Arial;padding:30px'>"
    "<h2>WiFi Berhasil Disimpan</h2>"
    "<p>ESP32 akan mencoba menyambungkan jaringan.</p>"
    "<a style='color:#8ed2a7' href='/'>Kembali ke NOVIA HOME</a>"
    "</body></html>"
  );
}


// ============================================================
// 28. ACCESS POINT CONFIGURATION SAVE
// New credentials are stored in Preferences, then ESP32 restarts.
// ============================================================

void handleSaveAP() {
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");
  ssid.trim();

  if (ssid.length() == 0 || ssid.length() > 32 ||
      pass.length() < 8 || pass.length() > 63) {
    server.send(400, "application/json",
                "{\"ok\":false,\"error\":\"SSID 1-32 karakter; password 8-63 karakter\"}");
    return;
  }

  saveAPConfig(ssid, pass);
  server.send(200, "application/json",
              "{\"ok\":true,\"message\":\"AP credentials saved; restarting\"}");
  delay(1200);
  ESP.restart();
}

// ============================================================
// 29. WEB ROOT HANDLER
// ============================================================

void handleRoot() {

  server.send_P(
    200,
    "text/html",
    INDEX_HTML
  );
}

// ============================================================
// 30. WEB SERVER ROUTES
// ============================================================

void setupWebServer() {

  server.on("/", HTTP_GET, []() {
    if (requireLocalClient()) handleRoot();
  });

  server.on("/api/state", HTTP_GET, []() {
    if (requireLocalClient()) handleState();
  });

  server.on("/api/set", HTTP_POST, []() {
    if (requireLocalClient()) handleSetChannel();
  });
  server.on("/api/timer", HTTP_POST, []() {
    if (requireLocalClient()) handleTimer();
  });
  server.on("/api/schedule", HTTP_POST, []() {
    if (requireLocalClient()) handleSchedule();
  });
  server.on("/api/mode", HTTP_POST, []() {
    if (requireLocalClient()) handleMode();
  });

  server.on("/api/scan", HTTP_GET, []() {
    if (requireLocalClient()) handleScanStart();
  });
  server.on("/api/scan/status", HTTP_GET, []() {
    if (requireLocalClient()) handleScanStatus();
  });
  server.on("/savewifi", HTTP_POST, []() {
    if (requireLocalClient()) handleSaveWiFi();
  });
  server.on("/saveap", HTTP_POST, []() {
    if (requireLocalClient()) handleSaveAP();
  });

  // Browser-only mode: do not intercept OS connectivity probes and do not
  // redirect unknown URLs. Open the AP IP or the ESP32 STA IP from the router LAN.
  server.onNotFound([]() {
    if (requireLocalClient()) {
      server.send(404, "text/plain", "NOVIA HOME: PAGE NOT FOUND. OPEN http://192.168.4.1/");
    }
  });

  server.begin();
  Serial.println("[NOVIA] WEB SERVER STARTED - BROWSER MODE - LOCAL ACCESS");
  Serial.println("[NOVIA] AP URL: http://192.168.4.1/");
  Serial.println("[NOVIA] When connected to router, use the ESP32 STA IP shown below.");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("[NOVIA] ROUTER URL: http://");
    Serial.println(WiFi.localIP());
  }
}
