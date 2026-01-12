#include "wifi/wifi.h"

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi_types.h>

#ifndef WIFI_SSID
  #define WIFI_SSID ""
#endif

#ifndef WIFI_PASS
  #define WIFI_PASS ""
#endif

namespace WiFiMgr {

  static const char* wifiEventName(WiFiEvent_t event) {
    switch (event) {
      case SYSTEM_EVENT_WIFI_READY:           return "WIFI_READY";
      case SYSTEM_EVENT_SCAN_DONE:            return "SCAN_DONE";
      case SYSTEM_EVENT_STA_START:            return "STA_START";
      case SYSTEM_EVENT_STA_STOP:             return "STA_STOP";
      case SYSTEM_EVENT_STA_CONNECTED:        return "STA_CONNECTED";
      case SYSTEM_EVENT_STA_DISCONNECTED:     return "STA_DISCONNECTED";
      case SYSTEM_EVENT_STA_GOT_IP:            return "STA_GOT_IP";
      case SYSTEM_EVENT_STA_LOST_IP:           return "STA_LOST_IP";
      case SYSTEM_EVENT_AP_START:              return "AP_START";
      case SYSTEM_EVENT_AP_STOP:               return "AP_STOP";
      case SYSTEM_EVENT_AP_STACONNECTED:       return "AP_STACONNECTED";
      case SYSTEM_EVENT_AP_STADISCONNECTED:    return "AP_STADISCONNECTED";
      default:                                return "UNKNOWN_EVENT";
    }
  }

  static const char* disconnectReasonName(uint8_t reason) {
      switch (reason) {
    #ifdef WIFI_REASON_AUTH_EXPIRE
        case WIFI_REASON_AUTH_EXPIRE:        return "AUTH_EXPIRE";
    #endif
    #ifdef WIFI_REASON_AUTH_FAIL
        case WIFI_REASON_AUTH_FAIL:          return "AUTH_FAIL";
    #endif
    #ifdef WIFI_REASON_NO_AP_FOUND
        case WIFI_REASON_NO_AP_FOUND:        return "NO_AP_FOUND";
    #endif
    #ifdef WIFI_REASON_ASSOC_FAIL
        case WIFI_REASON_ASSOC_FAIL:         return "ASSOC_FAIL";
    #endif
    #ifdef WIFI_REASON_HANDSHAKE_TIMEOUT
        case WIFI_REASON_HANDSHAKE_TIMEOUT: return "HANDSHAKE_TIMEOUT";
    #endif
    #ifdef WIFI_REASON_BEACON_TIMEOUT
        case WIFI_REASON_BEACON_TIMEOUT:     return "BEACON_TIMEOUT";
    #endif
    #ifdef WIFI_REASON_CONNECTION_FAIL
        case WIFI_REASON_CONNECTION_FAIL:    return "CONNECTION_FAIL";
    #endif
    #ifdef WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT
        case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT: return "4WAY_TIMEOUT";
    #endif
    #ifdef WIFI_REASON_ASSOC_TOOMANY
        case WIFI_REASON_ASSOC_TOOMANY:      return "ASSOC_TOOMANY";
    #endif
        default:                             return "OTHER";
      }
    }

  static bool s_started = false;

  static wl_status_t s_lastStatus = WL_IDLE_STATUS;

  static uint32_t s_lastAttemptMs = 0;
  static uint32_t s_attemptIntervalMs = 3000;

  // Smart heartbeat
  static uint32_t s_lastHeartbeatMs = 0;
  static int8_t   s_lastHeartbeatRssi = -127;

  static const uint32_t kAttemptIntervalMin = 3000;
  static const uint32_t kAttemptIntervalMax = 30000;

  static const uint32_t kHeartbeatMinMs = 60UL * 1000UL;       // never more than 1/min
  static const uint32_t kHeartbeatMaxMs = 5UL * 60UL * 1000UL; // at least every 5 min
  static const int8_t   kRssiDeltaDb    = 4;                   // print if RSSI moves >= 4 dB

  static bool credsPresent() {
    return (WIFI_SSID[0] != '\0');
  }

  void begin() {
    s_started = true;

    if (!credsPresent()) {
      Serial.println("[wifi] WIFI_SSID not set; WiFi disabled");
      WiFi.mode(WIFI_OFF);
      return;
    }

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(false);

    Serial.printf("[wifi] Starting STA. SSID: %s\n", WIFI_SSID);

    WiFi.onEvent([](arduino_event_id_t event, arduino_event_info_t info) {
      if (event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {
        Serial.printf(
          "[wifi] event: STA_DISCONNECTED (%d) reason=%d\n",
          (int)event,
          (int)info.wifi_sta_disconnected.reason
        );
      } else {
        Serial.printf(
          "[wifi] event: STA_DISCONNECTED reason=%s (%d)\n",
          disconnectReasonName(info.wifi_sta_disconnected.reason),
          (int)info.wifi_sta_disconnected.reason
        );
      }
    });

    // attempt ASAP
    s_lastAttemptMs = 0;
    s_attemptIntervalMs = kAttemptIntervalMin;
  }

  static void tryConnect(uint32_t now) {
    if (!credsPresent()) return;

    if (WiFi.status() == WL_CONNECTED) return;

    if (now - s_lastAttemptMs < s_attemptIntervalMs) return;
    s_lastAttemptMs = now;

    Serial.printf("[wifi] Connecting... (interval=%lums)\n", (unsigned long)s_attemptIntervalMs);

    WiFi.disconnect(false, true);
    delay(10);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
  }

  void tick() {
    if (!s_started) return;
    if (!credsPresent()) return;

    uint32_t now = millis();

    tryConnect(now);

    wl_status_t st = WiFi.status();

    // Log only when status changes
    if (st != s_lastStatus) {
      s_lastStatus = st;

      if (st == WL_CONNECTED) {
        Serial.printf("[wifi] Connected. IP: %s RSSI: %ddBm\n",
                      WiFi.localIP().toString().c_str(),
                      (int)WiFi.RSSI());
        s_attemptIntervalMs = kAttemptIntervalMin;

        // baseline heartbeat
        s_lastHeartbeatMs = now;
        s_lastHeartbeatRssi = (int8_t)WiFi.RSSI();
      } else {
        Serial.printf("[wifi] Status changed: %s\n", statusString());

        if (s_attemptIntervalMs < kAttemptIntervalMax) {
          s_attemptIntervalMs = constrain(
            s_attemptIntervalMs + 2000UL,
            kAttemptIntervalMin,
            kAttemptIntervalMax
          );
        }
      }
    }

    // Smart heartbeat (runs even when status doesn't change)
    if (st == WL_CONNECTED) {
      int8_t rssiNow = (int8_t)WiFi.RSSI();
      uint32_t age = now - s_lastHeartbeatMs;

      bool allow = age >= kHeartbeatMinMs;
      bool rssiMoved = (abs((int)rssiNow - (int)s_lastHeartbeatRssi) >= kRssiDeltaDb);
      bool tooOld = age >= kHeartbeatMaxMs;

      if (allow && (rssiMoved || tooOld)) {
        s_lastHeartbeatMs = now;
        s_lastHeartbeatRssi = rssiNow;
        Serial.printf("[wifi] OK (%ddBm)\n", (int)rssiNow);
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
