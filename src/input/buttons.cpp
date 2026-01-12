#include "input/buttons.h"

#include <Arduino.h>
#include "system/settings_pins.h"

namespace {

  struct ButtonState {
    uint8_t pin;
    bool stable;
    bool lastRead;
    uint32_t lastChangeMs;
  };

  constexpr uint32_t DEBOUNCE_MS = 30;

  ButtonState buttons[] = {
    { Pins::BTN1, true, true, 0 },
    { Pins::BTN2, true, true, 0 }
  };

}

namespace Buttons {

  void begin() {
    for (auto &b : buttons) {
      pinMode(b.pin, INPUT_PULLUP);
      b.stable = digitalRead(b.pin);
      b.lastRead = b.stable;
      b.lastChangeMs = millis();
    }
  }

  void update() {
    uint32_t now = millis();

    for (auto &b : buttons) {
      bool raw = digitalRead(b.pin);

      if (raw != b.lastRead) {
        b.lastRead = raw;
        b.lastChangeMs = now;
      }

      if ((now - b.lastChangeMs) > DEBOUNCE_MS && raw != b.stable) {
        b.stable = raw;
      }
    }
  }

  bool pressed(Id id) {
    static bool lastStable[COUNT] = { true, true };

    bool curr = buttons[id].stable;
    bool was  = lastStable[id];
    lastStable[id] = curr;

    return (was == HIGH && curr == LOW);
  }

  bool isDown(Id id) {
    return buttons[id].stable == LOW;
  }

} // namespace Buttons
