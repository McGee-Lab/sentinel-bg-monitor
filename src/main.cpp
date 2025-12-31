// Project: Sentinel BG Monitor
// Repo: sentinel-bg-monitor
// Purpose: Nightscout-based BG display & alerts

#include <Arduino.h>
#include "config.h"

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("Sentinel BG Monitor booting...");
  Serial.println(NIGHTSCOUT_HOST);
}

void loop() {
  delay(1000);
}
