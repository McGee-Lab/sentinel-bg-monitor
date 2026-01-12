#include "display/display.h"
#include <Arduino.h>
#include "system/version.h"
#include "nightscout/nightscout.h"

static TFT_eSPI s_tft;

namespace Display {

  // cache last drawn BG so we don't redraw constantly
  static int s_lastBgDrawn = -9999;

  void begin() {
    s_tft.init();
    s_tft.setRotation(1);
    s_tft.fillScreen(TFT_BLACK);
  }

  TFT_eSPI& tft() { return s_tft; }

  static void drawBgInBox(int boxX, int boxY, int boxW, int boxH) {
    // Clear inside of box only
    s_tft.fillRect(boxX + 1, boxY + 1, boxW - 2, boxH - 2, TFT_BLACK);

    s_tft.setTextDatum(MC_DATUM);

    if (Nightscout::hasValue()) {
      int bg = Nightscout::sgv();

      // Big BG number
      s_tft.setTextColor(TFT_WHITE, TFT_BLACK);
      s_tft.setTextSize(4);
      s_tft.drawString(String(bg), s_tft.width() / 2, boxY + boxH / 2 - 10);

      // Units / freshness
      s_tft.setTextSize(1);
      s_tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);

      uint32_t ageSec = (millis() - Nightscout::lastUpdateMs()) / 1000;
      s_tft.drawString("mg/dL", s_tft.width() / 2, boxY + boxH / 2 + 30);
      s_tft.drawString(String("updated ") + ageSec + "s ago", s_tft.width() / 2, boxY + boxH / 2 + 48);
    } else {
      // Loading state
      s_tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
      s_tft.setTextSize(2);
      s_tft.drawString("Fetching BG...", s_tft.width() / 2, boxY + boxH / 2);
    }
  }

  void showBootScreen() {
    s_tft.fillScreen(TFT_BLACK);

    // Title
    s_tft.setTextColor(TFT_WHITE, TFT_BLACK);
    s_tft.setTextDatum(TC_DATUM);
    s_tft.setTextSize(2);
    s_tft.drawString("McGee Lab", s_tft.width() / 2, 10);

    // Placeholder box (we’ll reuse for BG)
    int boxW = min(240, s_tft.width() - 40);
    int boxH = boxW;
    int boxX = (s_tft.width()  - boxW) / 2;
    int boxY = (s_tft.height() - boxH) / 2;

    s_tft.drawRect(boxX, boxY, boxW, boxH, TFT_DARKGREY);

    // Draw initial "loading" content
    drawBgInBox(boxX, boxY, boxW, boxH);

    // Footer + Version
    s_tft.setTextSize(1);
    s_tft.setTextColor(TFT_WHITE, TFT_BLACK);

    s_tft.setTextDatum(BL_DATUM);
    s_tft.drawString(SENTINEL_VERSION_TAG, 6, s_tft.height() - 6);

    s_tft.setTextDatum(BC_DATUM);
    s_tft.drawString("Sentinel BG Monitor (WIP)", s_tft.width() / 2, s_tft.height() - 6);
  }

  void tick() {
    // poll Nightscout in the background
    Nightscout::tick();

    // same box math as boot screen
    int boxW = min(240, s_tft.width() - 40);
    int boxH = boxW;
    int boxX = (s_tft.width()  - boxW) / 2;
    int boxY = (s_tft.height() - boxH) / 2;

    int bg = Nightscout::hasValue() ? Nightscout::sgv() : -1;

    // redraw only when value changes (or first time loaded)
    if (bg != s_lastBgDrawn) {
      drawBgInBox(boxX, boxY, boxW, boxH);
      s_lastBgDrawn = bg;
    }
  }

} // namespace Display
