#pragma once

// Automation: schedules, persistent reboot-resume timers, mode, NTP, and usage statistics.

// Timer persistence is additive: existing Preferences keys remain untouched.
// On restart, the saved remaining duration resumes (downtime is paused).
void loadPersistentTimers() {
  preferences.begin("novia", true);
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    char keyRemaining[8], keyEpoch[8];
    snprintf(keyRemaining, sizeof(keyRemaining), "tmR%u", i);
    snprintf(keyEpoch, sizeof(keyEpoch), "tmX%u", i);
    timerSavedSeconds[i] = preferences.getUInt(keyRemaining, 0);
    timerEpochDeadline[i] = preferences.getUInt(keyEpoch, 0);
    if (timerEpochDeadline[i]) {
      // Wait briefly for NTP so elapsed power-off time can be accounted for.
      timerDeadline[i] = 0;
      timerEpochRecovered[i] = false;
      timerEpochPendingSince[i] = millis();
    } else {
      // No valid wall-clock when configured: resume from the last saved remainder.
      timerDeadline[i] = timerSavedSeconds[i] ? millis() + (unsigned long)timerSavedSeconds[i] * 1000UL : 0;
    }
  }
  preferences.end();
  Serial.println("[TIMER] Persistent timers loaded; waiting for NTP where available");
}

void handlePersistentTimerRecovery() {
  struct tm timeinfo;
  bool timeReady = getLocalTime(&timeinfo, 5);
  uint32_t nowEpoch = timeReady ? (uint32_t)mktime(&timeinfo) : 0;
  unsigned long nowMs = millis();
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    if (!timerEpochDeadline[i] || timerEpochRecovered[i]) continue;
    if (timeReady) {
      if (nowEpoch >= timerEpochDeadline[i]) {
        timerEpochDeadline[i] = 0;
        timerEpochRecovered[i] = false;
        timerDeadline[i] = 0;
        savePersistentTimer(i, 0, 0);
        applyChannel(i, false, true);
        Serial.printf("[TIMER] CH%u expired while ESP32 was restarting/off\n", i + 1);
      } else {
        uint32_t remaining = timerEpochDeadline[i] - nowEpoch;
        timerSavedSeconds[i] = remaining;
        timerDeadline[i] = nowMs + (unsigned long)remaining * 1000UL;
        timerEpochRecovered[i] = true;
        timerEpochPendingSince[i] = 0;
      }
    } else if (nowMs - timerEpochPendingSince[i] >= 90000UL) {
      // NTP unavailable: fail safe by resuming the last stored remainder (paused downtime).
      timerEpochDeadline[i] = 0;
      timerEpochRecovered[i] = false;
      timerDeadline[i] = timerSavedSeconds[i] ? nowMs + (unsigned long)timerSavedSeconds[i] * 1000UL : 0;
      savePersistentTimer(i, timerSavedSeconds[i], 0);
      timerEpochPendingSince[i] = 0;
      Serial.printf("[TIMER] CH%u resumed from saved remainder; NTP unavailable\n", i + 1);
    }
  }
}

void persistActiveTimerRemainders() {
  unsigned long now = millis();
  if (now - lastTimerPersist < 60000UL) return;
  bool hasActive = false;
  for (uint8_t i = 0; i < RELAY_COUNT; i++) if (timerDeadline[i]) hasActive = true;
  if (!hasActive) { lastTimerPersist = now; return; }
  preferences.begin("novia", false);
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    char key[8]; snprintf(key, sizeof(key), "tmR%u", i);
    uint32_t remaining = timerDeadline[i] && (long)(timerDeadline[i] - now) > 0
      ? (uint32_t)((timerDeadline[i] - now + 999UL) / 1000UL) : 0;
    if (remaining) preferences.putUInt(key, remaining); else preferences.remove(key);
    timerSavedSeconds[i] = remaining;
  }
  preferences.end();
  lastTimerPersist = now;
}

void savePersistentTimer(uint8_t index, uint32_t seconds, uint32_t epochDeadline) {
  char keyRemaining[8], keyEpoch[8];
  snprintf(keyRemaining, sizeof(keyRemaining), "tmR%u", index);
  snprintf(keyEpoch, sizeof(keyEpoch), "tmX%u", index);
  preferences.begin("novia", false);
  if (seconds) preferences.putUInt(keyRemaining, seconds); else preferences.remove(keyRemaining);
  if (epochDeadline) preferences.putUInt(keyEpoch, epochDeadline); else preferences.remove(keyEpoch);
  preferences.end();
  timerSavedSeconds[index] = seconds;
  timerEpochDeadline[index] = epochDeadline;
  timerEpochRecovered[index] = epochDeadline != 0;
}

void loadAutomationConfig() {
  preferences.begin("novia", true);
  autoMode = preferences.getBool("auto", false);
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    char enKey[8], onKey[8], offKey[8], statKey[8];
    snprintf(enKey, sizeof(enKey), "scE%u", i);
    snprintf(onKey, sizeof(onKey), "scO%u", i);
    snprintf(offKey, sizeof(offKey), "scF%u", i);
    snprintf(statKey, sizeof(statKey), "onS%u", i);
    scheduleEnabled[i] = preferences.getBool(enKey, false);
    scheduleOnMinute[i] = preferences.getUShort(onKey, 0);
    scheduleOffMinute[i] = preferences.getUShort(offKey, 0);
    totalOnSeconds[i] = preferences.getUInt(statKey, 0);
  }
  preferences.end();
  Serial.printf("[NOVIA] AUTOMATION CONFIG LOADED; MODE=%s\n", autoMode ? "AUTO" : "MANUAL");
}

void saveAutomationConfig() {
  preferences.begin("novia", false);
  preferences.putBool("auto", autoMode);
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    char enKey[8], onKey[8], offKey[8];
    snprintf(enKey, sizeof(enKey), "scE%u", i);
    snprintf(onKey, sizeof(onKey), "scO%u", i);
    snprintf(offKey, sizeof(offKey), "scF%u", i);
    preferences.putBool(enKey, scheduleEnabled[i]);
    preferences.putUShort(onKey, scheduleOnMinute[i]);
    preferences.putUShort(offKey, scheduleOffMinute[i]);
  }
  preferences.end();
}

void saveUsageStats() {
  preferences.begin("novia", false);
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    uint32_t value = totalOnSeconds[i];
    if (channelState[i]) value += (uint32_t)((millis() - onStartedAt[i]) / 1000UL);
    char key[8];
    snprintf(key, sizeof(key), "onS%u", i);
    preferences.putUInt(key, value);
    if (channelState[i]) onStartedAt[i] = millis();
    totalOnSeconds[i] = value;
  }
  preferences.end();
  lastStatsSave = millis();
}

void startTimeSync() {
  configTime(GMT_OFFSET_SECONDS, DAYLIGHT_OFFSET_SECONDS,
             "pool.ntp.org", "time.google.com", "time.nist.gov");
  Serial.println("[NOVIA] NTP TIME SYNC STARTED (WIB)");
}

void handleAutomation() {
  unsigned long now = millis();
  handlePersistentTimerRecovery();
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    if (timerDeadline[i] != 0 && (long)(now - timerDeadline[i]) >= 0) {
      timerDeadline[i] = 0;
      savePersistentTimer(i, 0, 0);
      applyChannel(i, false, true);
      Serial.printf("[TIMER] CH%u AUTO OFF\n", i + 1);
    }
  }

  if (autoMode) {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 5)) {
      int minuteOfDay = timeinfo.tm_hour * 60 + timeinfo.tm_min;
      if (minuteOfDay != lastScheduleMinute) {
        lastScheduleMinute = minuteOfDay;
        for (uint8_t i = 0; i < RELAY_COUNT; i++) {
          if (!scheduleEnabled[i]) continue;
          if (scheduleOnMinute[i] == minuteOfDay) applyChannel(i, true, true);
          if (scheduleOffMinute[i] == minuteOfDay) applyChannel(i, false, true);
        }
      }
    }
  }

  persistActiveTimerRemainders();
  if (now - lastStatsSave >= STATS_SAVE_INTERVAL) saveUsageStats();
}

void handleTimer() {
  if (!server.hasArg("ch") || !server.hasArg("minutes")) {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"MISSING_ARGUMENT\"}"); return;
  }
  int ch = server.arg("ch").toInt();
  long minutes = server.arg("minutes").toInt();
  if (ch < 1 || ch > RELAY_COUNT || minutes < 0 || minutes > 1440) {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"OUT_OF_RANGE\"}"); return;
  }
  uint32_t seconds = minutes == 0 ? 0 : (uint32_t)minutes * 60UL;
  uint32_t epochDeadline = 0;
  struct tm timeinfo;
  if (seconds && getLocalTime(&timeinfo, 5)) epochDeadline = (uint32_t)mktime(&timeinfo) + seconds;
  timerDeadline[ch - 1] = seconds == 0 ? 0 : millis() + (unsigned long)seconds * 1000UL;
  savePersistentTimer(ch - 1, seconds, epochDeadline);
  server.send(200, "application/json", "{\"ok\":true}");
}

void handleSchedule() {
  if (!server.hasArg("ch") || !server.hasArg("enabled") || !server.hasArg("on") || !server.hasArg("off")) {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"MISSING_ARGUMENT\"}"); return;
  }
  int ch = server.arg("ch").toInt();
  int onMinute = server.arg("on").toInt();
  int offMinute = server.arg("off").toInt();
  String enabledArg = server.arg("enabled");
  if (ch < 1 || ch > RELAY_COUNT || onMinute < 0 || onMinute > 1439 || offMinute < 0 || offMinute > 1439 || onMinute == offMinute || (enabledArg != "0" && enabledArg != "1")) {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"INVALID_VALUE\"}"); return;
  }
  uint8_t idx = ch - 1;
  scheduleEnabled[idx] = enabledArg == "1";
  scheduleOnMinute[idx] = (uint16_t)onMinute;
  scheduleOffMinute[idx] = (uint16_t)offMinute;
  saveAutomationConfig();
  server.send(200, "application/json", "{\"ok\":true}");
}

void handleMode() {
  if (!server.hasArg("mode")) { server.send(400, "application/json", "{\"ok\":false}"); return; }
  String mode = server.arg("mode");
  if (mode != "AUTO" && mode != "MANUAL") { server.send(400, "application/json", "{\"ok\":false}"); return; }
  autoMode = mode == "AUTO";
  lastScheduleMinute = -1;
  saveAutomationConfig();
  server.send(200, "application/json", "{\"ok\":true}");
}

