#pragma once
#include <stdint.h>

namespace Nightscout {
  void begin();
  void tick();                 // call frequently (non-blocking scheduler)
  bool hasValue();
  int  sgv();                  // mg/dL

  const char* direction();   // trend text from Nightscout (e.g. "Flat", "SingleUp")

  // "New reading arrived" time (millis), only changes when the entry is new
  uint32_t lastUpdateMs();

  // Optional but very useful:
  uint32_t lastFetchMs();          // millis() when HTTP succeeded (even if same entry)
  uint64_t lastEntryDateMs();      // epoch ms from Nightscout "date"
  bool didUpdate();               // true once right after a new entry arrives
}
