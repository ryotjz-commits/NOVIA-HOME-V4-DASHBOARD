#pragma once

// Local PIN authentication. Existing NVS namespace and keys are preserved;
// authentication adds only authSalt/authHash keys.
#include <mbedtls/sha256.h>
#include <esp_system.h>

static String noviaAuthSalt = "";
static String noviaAuthHash = "";
static String noviaAuthSession = "";
static unsigned long noviaAuthSessionExpiry = 0;
static uint8_t noviaAuthFailures = 0;
static unsigned long noviaAuthLockUntil = 0;
static const unsigned long NOVIA_AUTH_SESSION_MS = 12UL * 60UL * 60UL * 1000UL;
static const unsigned long NOVIA_AUTH_LOCK_MS = 60000UL;

String noviaRandomHex(size_t bytes) {
  const char *hex = "0123456789abcdef";
  String out;
  out.reserve(bytes * 2);
  for (size_t i = 0; i < bytes; i++) {
    uint8_t v = (uint8_t)(esp_random() & 0xFF);
    out += hex[v >> 4];
    out += hex[v & 0x0F];
  }
  return out;
}

String noviaPinDigest(const String &salt, const String &pin) {
  String input = salt + ":" + pin;
  uint8_t digest[32];
  mbedtls_sha256((const unsigned char *)input.c_str(), input.length(), digest, 0);
  const char *hex = "0123456789abcdef";
  String out;
  out.reserve(64);
  for (uint8_t b : digest) {
    out += hex[b >> 4];
    out += hex[b & 0x0F];
  }
  return out;
}

void loadNoviaAuth() {
  preferences.begin("novia", true);
  noviaAuthSalt = preferences.getString("authSalt", "");
  noviaAuthHash = preferences.getString("authHash", "");
  preferences.end();
  Serial.printf("[AUTH] Local PIN: %s\n", noviaAuthHash.length() == 64 ? "CONFIGURED" : "SETUP REQUIRED");
}

bool noviaPinValid(const String &pin) {
  if (pin.length() < 6 || pin.length() > 8) return false;
  for (size_t i = 0; i < pin.length(); i++) if (!isDigit(pin[i])) return false;
  return true;
}

bool noviaSessionValid() {
  if (noviaAuthSession.length() != 32) return false;
  if ((long)(millis() - noviaAuthSessionExpiry) >= 0) {
    noviaAuthSession = "";
    return false;
  }
  if (!server.hasHeader("X-NOVIA-SESSION")) return false;
  return server.header("X-NOVIA-SESSION") == noviaAuthSession;
}

void handleAuthStatus() {
  bool configured = noviaAuthHash.length() == 64;
  bool authenticated = configured && noviaSessionValid();
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", String("{\"configured\":") + (configured ? "true" : "false") + ",\"authenticated\":" + (authenticated ? "true" : "false") + "}");
}

void handleAuthSetup() {
  if (noviaAuthHash.length() == 64) {
    server.send(409, "application/json", "{\"ok\":false,\"error\":\"PIN_ALREADY_CONFIGURED\"}");
    return;
  }
  String pin = server.arg("pin");
  if (!noviaPinValid(pin)) {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"PIN_MUST_BE_4_TO_8_DIGITS\"}");
    return;
  }
  noviaAuthSalt = noviaRandomHex(16);
  noviaAuthHash = noviaPinDigest(noviaAuthSalt, pin);
  preferences.begin("novia", false);
  preferences.putString("authSalt", noviaAuthSalt);
  preferences.putString("authHash", noviaAuthHash);
  preferences.end();
  noviaAuthSession = noviaRandomHex(16);
  noviaAuthSessionExpiry = millis() + NOVIA_AUTH_SESSION_MS;
  noviaAuthFailures = 0;
  server.send(200, "application/json", String("{\"ok\":true,\"token\":\"") + noviaAuthSession + "\"}");
}

void handleAuthLogin() {
  if (noviaAuthHash.length() != 64) {
    server.send(428, "application/json", "{\"ok\":false,\"error\":\"PIN_SETUP_REQUIRED\"}");
    return;
  }
  if (noviaAuthLockUntil && (long)(millis() - noviaAuthLockUntil) < 0) {
    server.send(429, "application/json", "{\"ok\":false,\"error\":\"TOO_MANY_ATTEMPTS\"}");
    return;
  }
  String pin = server.arg("pin");
  if (!noviaPinValid(pin) || noviaPinDigest(noviaAuthSalt, pin) != noviaAuthHash) {
    if (++noviaAuthFailures >= 5) {
      noviaAuthFailures = 0;
      noviaAuthLockUntil = millis() + NOVIA_AUTH_LOCK_MS;
    }
    server.send(401, "application/json", "{\"ok\":false,\"error\":\"INVALID_PIN\"}");
    return;
  }
  noviaAuthFailures = 0;
  noviaAuthLockUntil = 0;
  noviaAuthSession = noviaRandomHex(16);
  noviaAuthSessionExpiry = millis() + NOVIA_AUTH_SESSION_MS;
  server.send(200, "application/json", String("{\"ok\":true,\"token\":\"") + noviaAuthSession + "\"}");
}

void handleAuthChange() {
  String oldPin = server.arg("oldpin");
  String newPin = server.arg("newpin");
  if (!noviaSessionValid()) {
    server.send(401, "application/json", "{\"ok\":false,\"error\":\"AUTH_REQUIRED\"}");
    return;
  }
  if (!noviaPinValid(oldPin) || noviaPinDigest(noviaAuthSalt, oldPin) != noviaAuthHash) {
    server.send(401, "application/json", "{\"ok\":false,\"error\":\"INVALID_OLD_PIN\"}");
    return;
  }
  if (!noviaPinValid(newPin)) {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"PIN_MUST_BE_4_TO_8_DIGITS\"}");
    return;
  }
  noviaAuthSalt = noviaRandomHex(16);
  noviaAuthHash = noviaPinDigest(noviaAuthSalt, newPin);
  preferences.begin("novia", false);
  preferences.putString("authSalt", noviaAuthSalt);
  preferences.putString("authHash", noviaAuthHash);
  preferences.end();
  noviaAuthSession = noviaRandomHex(16);
  noviaAuthSessionExpiry = millis() + NOVIA_AUTH_SESSION_MS;
  server.send(200, "application/json", String("{\"ok\":true,\"token\":\"") + noviaAuthSession + "\"}");
}

void handleAuthLogout() {
  noviaAuthSession = "";
  noviaAuthSessionExpiry = 0;
  server.send(200, "application/json", "{\"ok\":true}");
}

bool requireAuthenticatedClient() {
  if (!requireLocalClient()) return false;
  if (noviaSessionValid()) return true;
  server.sendHeader("Cache-Control", "no-store");
  server.send(401, "application/json", "{\"ok\":false,\"error\":\"AUTH_REQUIRED\"}");
  return false;
}
