#pragma once

namespace BG {

  // Units
  inline constexpr bool UNITS_MGDL = true;   // false = mmol/L

  // Thresholds (mg/dL)
  inline constexpr int URGENT_LOW  = 55;
  inline constexpr int LOW         = 70;
  inline constexpr int HIGH        = 180;
  inline constexpr int URGENT_HIGH = 250;

}
