#pragma once
#include <stdint.h>

namespace Nightscout {
  void begin();
  void tick();                 // call frequently (non-blocking scheduler)
  bool hasValue();
  int  sgv();                  // mg/dL
  uint32_t lastUpdateMs();
}
