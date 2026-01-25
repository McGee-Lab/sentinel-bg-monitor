#include "nightscout/nightscout.h"

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#ifndef NIGHTSCOUT_URL
  #define NIGHTSCOUT_URL ""
#endif

#ifndef NIGHTSCOUT_TOKEN
  #define NIGHTSCOUT_TOKEN ""
#endif

namespace Nightscout {

  static int s_sgv = -1;

  // Only updates when we detect a NEW entry (date changes)
  static uint32_t s_lastUpdateMs = 0;

  // Updates whenever HTTP 200 + valid JSON parsed (even if same entry)
  static uint32_t s_lastFetchMs = 0;

  // Nightscout entry timestamp (epoch ms)
  static uint64_t s_lastEntryDateMs = 0;

  static bool s_didUpdate = false;

  static uint32_t s_lastAttemptMs = 0;
  static const uint32_t kPollMs = 5000;

  static char s_direction[24] = "Unknown";
  const char* direction() { return s_direction; }


  void begin() {}

  bool hasValue() { return s_sgv >= 0; }
  int sgv() { return s_sgv; }

  uint32_t lastUpdateMs() { return s_lastUpdateMs; }
  uint32_t lastFetchMs() { return s_lastFetchMs; }
  uint64_t lastEntryDateMs() { return s_lastEntryDateMs; }

  bool didUpdate() {
    bool v = s_didUpdate;
    s_didUpdate = false;
    return v;
  }

  static void applyAuthHeaders(HTTPClient& http) {
    if (strlen(NIGHTSCOUT_TOKEN) > 0) {
      http.addHeader("api-secret", NIGHTSCOUT_TOKEN);
      // If your setup uses bearer:
      // http.addHeader("Authorization", String("Bearer ") + NIGHTSCOUT_TOKEN);
    }
  }

  void tick() {
    if (WiFi.status() != WL_CONNECTED) return;
    if (NIGHTSCOUT_URL[0] == '\0') return;

    const uint32_t now = millis();
    if (now - s_lastAttemptMs < kPollMs) return;
    s_lastAttemptMs = now;

    String url = String(NIGHTSCOUT_URL);
    if (url.endsWith("/")) url.remove(url.length() - 1);
    url += "/api/v1/entries.json?count=1";

    HTTPClient http;
    http.setTimeout(2500);
    http.begin(url);
    applyAuthHeaders(http);

    int code = http.GET();
    if (code == 200) {
      String payload = http.getString();

      JsonDocument doc;
      DeserializationError err = deserializeJson(doc, payload);

      if (!err && doc.is<JsonArray>() && doc.size() > 0) {
        JsonObject obj = doc[0];

        // Nightscout usually provides "date" as epoch ms
        uint64_t entryDateMs = 0;
        if (obj["date"].is<uint64_t>()) {
          entryDateMs = obj["date"].as<uint64_t>();
        } else if (obj["date"].is<long long>()) {
          entryDateMs = (uint64_t)obj["date"].as<long long>();
        }

        if (obj["sgv"].is<int>()) {
          int newSgv = obj["sgv"].as<int>();

          // We successfully fetched + parsed
          s_lastFetchMs = now;

          // Only count as "updated" when a NEW entry arrives
          // (date changes). This stops your UI from resetting every poll.
          if (entryDateMs != 0 && entryDateMs > s_lastEntryDateMs) {
            s_lastEntryDateMs = entryDateMs;
            s_lastUpdateMs = now;
            s_didUpdate = true;
          }

          // Always store the newest sgv we saw
          s_sgv = newSgv;
          const char* dir = obj["direction"] | "Unknown";
          strncpy(s_direction, dir, sizeof(s_direction) - 1);
          s_direction[sizeof(s_direction) - 1] = '\0';
        }
      }
    }

    http.end();
  }

} // namespace Nightscout
