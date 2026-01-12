#pragma once
#include <stdint.h>

namespace Buttons {

  enum Id {
    BTN1 = 0,
    BTN2 = 1,
    COUNT
  };

  // Call once in setup()
  void begin();

  // Call every loop()
  void update();

  // True only on the transition to pressed
  bool pressed(Id id);

  // Optional: current stable state
  bool isDown(Id id);

} // namespace Buttons
