#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

struct TouchCal {
  int16_t minX, maxX, minY, maxY;
  bool swapXY;
  bool flipX;
  bool flipY;
  uint8_t tftRotation;
  bool valid;
};

namespace TouchCalibration {
  // Load/save calibration from ESP32 Preferences (NVS)
  bool load(TouchCal &cal);
  void save(const TouchCal &cal);

  // Convert RAW touch -> screen pixels using calibration
  bool mapRawToPixel(const TouchCal &cal, const TS_Point &raw, TFT_eSPI &tft, int16_t &x, int16_t &y);

  // Read touch, apply pressure filter, map to pixels
  bool readTouchPixel(TouchCal &cal, TFT_eSPI &tft, XPT2046_Touchscreen &ts, int zMin, int16_t &x, int16_t &y);

  // Interactive calibration UI (4 targets). Saves + restarts.
  void runCalibration(TouchCal &cal, TFT_eSPI &tft, XPT2046_Touchscreen &ts, uint8_t tftRotation, int zMin);
}
