#pragma once
#include <TFT_eSPI.h>

namespace Display {

  void begin();
  void tick();

  // Access to the shared TFT instance
  TFT_eSPI& tft();

  // Simple placeholder boot screen
  void showBootScreen();

} // namespace Display
