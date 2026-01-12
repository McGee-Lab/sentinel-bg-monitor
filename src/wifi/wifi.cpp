#include "wifi/wifi.h"

#include <Arduino.h>
#include <WiFi.h>

// =======================
// CONFIG (placeholder)
// =======================
// Recommended: store these in platformio.ini build_flags as -DWIFI_SSID="..." etc.
// For now, you can fill them here or leave empty to disable WiFi attempts.

#ifndef WIFI_SSID
  #define WIFI_SSID ""
#endif

#ifndef WIFI_PASS
  #define WIFI_PASS ""
#endif

static_assert(sizeof(WIFI_SSID) > 0, "WIFI_SSID must be a string");
static_assert(sizeof(WIFI_PASS) > 0, "WIFI_PASS must be a string");

namespace WiFiMgr {

  // Connection state
  static bool s_started = false;
  static uint32_t s_lastAttemptMs = 0;
  static uint32_t s_attemptIntervalMs = 3000; // increases on failure up to max
  static uint32_t s_lastStatusPrintMs = 0;

  static const uint32_t kAttemptIntervalMin = 3000;
  static const uint32_t kAttemptIntervalMax = 30000;

  static bool credsPresent() {
    return (WIFI_SSID[0] != '\0');
  }

  void begin() {
    s_started = true;

    if (!credsPresent()) {
      Serial.println("[wifi] WIFI_SSID not set; WiFi disabled (placeholder)");
      WiFi.mode(WIFI_OFF);
      return;
    }

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(false); // don't write credentials to flash repeatedly

    Serial.printf("[wifi] Starting STA. SSID: %s\n", WIFI_SSID);

    // Kick off first attempt immediately
    s_lastAttemptMs = 0;
    s_attemptIntervalMs = kAttemptIntervalMin;
  }

  static void tryConnect(uint32_t now) {
    if (!credsPresent()) return;

    // If already connected, nothing to do
    if (WiFi.status() == WL_CONNECTED) return;

    // Respect interval
    if (now - s_lastAttemptMs < s_attemptIntervalMs) return;

    s_lastAttemptMs = now;

    Serial.printf("[wifi] Connecting... (interval=%lums)\n", (unsigned long)s_attemptIntervalMs);

    // Start/restart connect
    WiFi.disconnect(true, true);
    delay(10);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
  }

  void tick() {
    if (!s_started) return;
    if (!credsPresent()) return;

    uint32_t now = millis();

    // Attempt connect if needed
    tryConnect(now);

    // Print status occasionally
    if (now - s_lastStatusPrintMs > 2000) {
      s_lastStatusPrintMs = now;

      wl_status_t st = WiFi.status();
      if (st == WL_CONNECTED) {
        Serial.printf("[wifi] Connected. IP: %s RSSI: %ddBm\n",
                      WiFi.localIP().toString().c_str(),
                      (int)WiFi.RSSI());
        // Once connected, keep interval small (not used anyway)
        s_attemptIntervalMs = kAttemptIntervalMin;
      } else {
        Serial.printf("[wifi] Status: %s\n", statusString());

        // Exponential-ish backoff while failing, capped
        if (s_attemptIntervalMs < kAttemptIntervalMax) {
          s_attemptIntervalMs = constrain(
          s_attemptIntervalMs + 2000UL,
          kAttemptIntervalMin,
          kAttemptIntervalMax
         );
        }
      }
    }
  }

  bool isConnected() {
    return WiFi.status() == WL_CONNECTED;
  }

  int8_t rssi() {
    if (!isConnected()) return -127;
    return (int8_t)WiFi.RSSI();
  }

  const char* statusString() {
    switch (WiFi.status()) {
      case WL_IDLE_STATUS:      return "IDLE";
      case WL_NO_SSID_AVAIL:    return "NO_SSID";
      case WL_SCAN_COMPLETED:   return "SCAN_DONE";
      case WL_CONNECTED:        return "CONNECTED";
      case WL_CONNECT_FAILED:   return "CONNECT_FAILED";
      case WL_CONNECTION_LOST:  return "CONNECTION_LOST";
      case WL_DISCONNECTED:     return "DISCONNECTED";
      default:                  return "UNKNOWN";
    }
  }

} // namespace WiFiMgr
