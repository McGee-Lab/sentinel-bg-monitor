#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include "../system/touchCalibration.h"

namespace HardwareTest {
  void begin(TFT_eSPI &tft);

  // Call frequently from loop()
  void tick(
    TFT_eSPI &tft,
    XPT2046_Touchscreen &ts,
    TouchCal &cal,
    int touchZMin,
    bool btn1PressedEdge,
    bool btn2PressedEdge,
    void (*playBtn1Sound)(),
    void (*playBtn2Sound)()
  );
}
