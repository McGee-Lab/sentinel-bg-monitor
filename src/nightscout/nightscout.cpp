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
  static uint32_t s_lastUpdateMs = 0;
  static uint32_t s_lastAttemptMs = 0;
  static const uint32_t kPollMs = 5000;

  void begin() {}

  bool hasValue() { return s_sgv >= 0; }
  int sgv() { return s_sgv; }
  uint32_t lastUpdateMs() { return s_lastUpdateMs; }

  static void applyAuthHeaders(HTTPClient& http) {
    // Nightscout auth differs by setup; try the common ones.
    // If your server uses "api-secret", this should work.
    if (strlen(NIGHTSCOUT_TOKEN) > 0) {
      http.addHeader("api-secret", NIGHTSCOUT_TOKEN);
      // Some setups use bearer token:
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
      // JSON is an array with one object: [{ "sgv": 123, ... }]
      String payload = http.getString();

      StaticJsonDocument<1024> doc;
      DeserializationError err = deserializeJson(doc, payload);
      if (!err && doc.is<JsonArray>() && doc.size() > 0) {
        JsonObject obj = doc[0];
        if (obj.containsKey("sgv")) {
          s_sgv = obj["sgv"].as<int>();
          s_lastUpdateMs = now;
        }
      }
    }

    http.end();
  }

} // namespace Nightscout
