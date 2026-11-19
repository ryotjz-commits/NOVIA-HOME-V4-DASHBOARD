#pragma once

// ============================================================
// 11. LOOP TASK WATCHDOG
// ============================================================

void setupLoopWatchdog() {

#if ESP_IDF_VERSION_MAJOR >= 5
  esp_task_wdt_config_t wdtConfig = {};
  wdtConfig.timeout_ms = LOOP_WDT_TIMEOUT_MS;
  wdtConfig.idle_core_mask = 0;   // Subscribe only this loop task.
  wdtConfig.trigger_panic = true;

  esp_err_t initResult = esp_task_wdt_init(&wdtConfig);
#else
  esp_err_t initResult = esp_task_wdt_init(
    LOOP_WDT_TIMEOUT_MS / 1000,
    true
  );
#endif

  // INVALID_STATE means the framework already initialized the WDT.
  if (initResult != ESP_OK && initResult != ESP_ERR_INVALID_STATE) {
    Serial.printf("[WDT] INIT FAILED: %d\\n", (int)initResult);
    return;
  }

  esp_err_t addResult = esp_task_wdt_add(NULL);

  if (addResult == ESP_OK || addResult == ESP_ERR_INVALID_STATE) {
    loopWatchdogEnabled = true;
    Serial.println("[WDT] LOOP TASK WATCHDOG ENABLED (10s)");
  } else {
    Serial.printf("[WDT] TASK SUBSCRIBE FAILED: %d\\n", (int)addResult);
  }
}

// ============================================================
// 12. CENTRAL CHANNEL CONTROLLER
// ============================================================

void applyChannel(uint8_t ch, bool state,
                  bool updateBlynk) {

  if (ch >= RELAY_COUNT) return;

  if (channelState[ch] != state) {
    unsigned long now = millis();
    if (channelState[ch]) {
      totalOnSeconds[ch] += (uint32_t)((now - onStartedAt[ch]) / 1000UL);
      onStartedAt[ch] = 0;
    } else if (state) {
      onStartedAt[ch] = now;
    }
    channelState[ch] = state;
    digitalWrite(relayPins[ch], state ? RELAY_ON : RELAY_OFF);
  }

  Serial.print("[NOVIA] ");
  Serial.print(channelNames[ch]);
  Serial.print(" : ");
  Serial.println(state ? "ON" : "OFF");

  // Blynk synchronization

  if (updateBlynk && Blynk.connected()) {

    switch (ch) {

      case 0:
        Blynk.virtualWrite(V0, state ? 1 : 0);
        break;

      case 1:
        Blynk.virtualWrite(V1, state ? 1 : 0);
        break;

      case 2:
        Blynk.virtualWrite(V2, state ? 1 : 0);
        break;

      case 3:
        Blynk.virtualWrite(V3, state ? 1 : 0);
        break;
    }
  }
}

// ============================================================
// 12. SYNCHRONIZE ALL CHANNELS
// ============================================================

void syncAllChannels() {

  if (!Blynk.connected()) return;

  Blynk.virtualWrite(V0, channelState[0] ? 1 : 0);
  Blynk.virtualWrite(V1, channelState[1] ? 1 : 0);
  Blynk.virtualWrite(V2, channelState[2] ? 1 : 0);
  Blynk.virtualWrite(V3, channelState[3] ? 1 : 0);

  Serial.println("[NOVIA] ALL CHANNELS SYNCHRONIZED");
}

// ============================================================
// 13. PHYSICAL BUTTON HANDLER
// ============================================================

void handleButtons() {

  unsigned long now = millis();

  for (uint8_t i = 0; i < RELAY_COUNT; i++) {

    bool reading = digitalRead(buttonPins[i]);

    if (reading != buttonLast[i]) {

      buttonTimer[i] = now;

      buttonLast[i] = reading;
    }

    if (now - buttonTimer[i] >= DEBOUNCE_TIME) {

      if (reading != buttonStable[i]) {

        buttonStable[i] = reading;

        // Button pressed

        if (buttonStable[i] == LOW) {

          bool nextState = !channelState[i];

          Serial.print("[BUTTON] CH");
          Serial.print(i + 1);
          Serial.println(" PRESSED");

          applyChannel(i, nextState, true);
        }
      }
    }
  }
}

// ============================================================
// 14. BLYNK CALLBACKS
// ============================================================

BLYNK_WRITE(V0) {

  applyChannel(0, param.asInt() == 1, false);
}

BLYNK_WRITE(V1) {

  applyChannel(1, param.asInt() == 1, false);
}

BLYNK_WRITE(V2) {

  applyChannel(2, param.asInt() == 1, false);
}

BLYNK_WRITE(V3) {

  applyChannel(3, param.asInt() == 1, false);
}

// ============================================================
// 15. BLYNK CONNECTED
// ============================================================

BLYNK_CONNECTED() {

  Serial.println("[NOVIA] BLYNK CONNECTED");

  // Device is authoritative after reconnection.

  syncAllChannels();
}

