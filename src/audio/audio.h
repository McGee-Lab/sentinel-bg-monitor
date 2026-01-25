#pragma once
#include <stdint.h>

namespace AudioOut {

  // Call once in setup()
  void begin();

  // Basic tone player
  void playToneHz(float hz, uint16_t ms);

  // Patterns you already use
  void playBtn1Sound();
  void playBtn2Sound();

  // BG Sound Alerts
  void playHighAlert();

} // namespace AudioOut
