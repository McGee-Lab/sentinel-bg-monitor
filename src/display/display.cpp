#include "display/display.h"
#include <Arduino.h>
#include "system/version.h"

static TFT_eSPI s_tft;

namespace Display {

  void begin() {
    s_tft.init();
    s_tft.setRotation(1);
    s_tft.fillScreen(TFT_BLACK);
  }

  void tick() {
    // placeholder for future UI updates
  }

  TFT_eSPI& tft() {
    return s_tft;
  }

void showBootScreen() {
  s_tft.fillScreen(TFT_BLACK);

  // Title
  s_tft.setTextColor(TFT_WHITE, TFT_BLACK);
  s_tft.setTextDatum(TC_DATUM);
  s_tft.setTextSize(2);
  s_tft.drawString("McGee Lab", s_tft.width() / 2, 10);

  // Placeholder box
  int boxW = min(240, s_tft.width() - 40);
  int boxH = boxW;
  int boxX = (s_tft.width()  - boxW) / 2;
  int boxY = (s_tft.height() - boxH) / 2;

  s_tft.drawRect(boxX, boxY, boxW, boxH, TFT_DARKGREY);

  s_tft.setTextDatum(MC_DATUM);
  s_tft.setTextSize(1);
  s_tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  s_tft.drawString("LOGO / SPLASH", s_tft.width() / 2, boxY + boxH / 2 - 10);
  s_tft.drawString("placeholder", s_tft.width() / 2, boxY + boxH / 2 + 10);

  // Footer + Version
  s_tft.setTextDatum(BC_DATUM);
  s_tft.setTextColor(TFT_WHITE, TFT_BLACK);
  s_tft.setTextSize(1);

  // Bottom-left: version
  s_tft.setTextDatum(BL_DATUM);
  s_tft.drawString(SENTINEL_VERSION_TAG, 6, s_tft.height() - 6);

  // Bottom-center: project name
  s_tft.setTextDatum(BC_DATUM);
  s_tft.drawString("Sentinel BG Monitor (WIP)", s_tft.width() / 2, s_tft.height() - 6);
}

} // namespace Display
