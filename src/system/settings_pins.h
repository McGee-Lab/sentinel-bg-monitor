#pragma once

namespace Pins {

  // =======================
  // I2S (MAX98357)
  // =======================
  inline constexpr int I2S_BCLK = 26;
  inline constexpr int I2S_LRC  = 25;
  inline constexpr int I2S_DOUT = 22;

  // =======================
  // Buttons (Active LOW)
  // =======================
  inline constexpr int BTN1 = 27;
  inline constexpr int BTN2 = 32;

  // =======================
  // Touch (XPT2046)
  // =======================
  inline constexpr int XPT_CS  = 33;
  inline constexpr int XPT_IRQ = 255; // not used

} // namespace Pins
