#pragma once
#include <stdint.h>

namespace WiFiMgr {

  // Call once at boot
  void begin();

  // Call in loop (non-blocking)
  void tick();

  // Optional helpers
  bool isConnected();
  const char* statusString();
  int8_t rssi();

} // namespace WiFiMgr
