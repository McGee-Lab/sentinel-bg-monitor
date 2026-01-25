#include "display/display.h"
#include <Arduino.h>
#include "system/version.h"
#include "nightscout/nightscout.h"
#include "audio/audio.h"

static TFT_eSPI s_tft;

static uint32_t s_lastFreshnessDrawMs = 0;
static int s_lastAgeMinDrawn = -1;

static uint32_t s_borderAnimMs = 0;
static uint16_t s_lastBorderColor = 0;


namespace Display {

  // Triangle Helpers
  // size = "half height" basically. Try 10–14 for your UI.
  static void drawUpArrow(int cx, int cy, int size, uint16_t color) {
    s_tft.fillTriangle(
      cx,         cy - size,   // tip
      cx - size,  cy + size,   // left base
      cx + size,  cy + size,   // right base
      color
    );
  }

  static void drawDownArrow(int cx, int cy, int size, uint16_t color) {
    s_tft.fillTriangle(
      cx,         cy + size,   // tip
      cx - size,  cy - size,   // left base
      cx + size,  cy - size,   // right base
      color
    );
  }

  static void drawRightArrow(int cx, int cy, int size, uint16_t color) {
    s_tft.fillTriangle(
      cx + size,  cy,          // tip
      cx - size,  cy - size,   // top base
      cx - size,  cy + size,   // bottom base
      color
    );
  }

  static void drawDiagUpRightArrow(int cx, int cy, int size, uint16_t color) {
    // Tip is up-right; base is down-left-ish
    s_tft.fillTriangle(
      cx + size,  cy - size,   // tip
      cx - size,  cy - size,   // base 1
      cx - size,  cy + size,   // base 2
      color
    );
  }

  static void drawDiagDownRightArrow(int cx, int cy, int size, uint16_t color) {
    // Tip is down-right; base is up-left-ish
    s_tft.fillTriangle(
      cx + size,  cy + size,   // tip
      cx - size,  cy - size,   // base 1
      cx - size,  cy + size,   // base 2
      color
    );
  }

  // Optional: double arrows (two triangles with spacing)
  static void drawDoubleUp(int cx, int cy, int size, int gap, uint16_t color) {
    drawUpArrow(cx, cy - (size + gap), size, color);
    drawUpArrow(cx, cy + (size + gap), size, color);
  }

  static void drawDoubleDown(int cx, int cy, int size, int gap, uint16_t color) {
    drawDownArrow(cx, cy - (size + gap), size, color);
    drawDownArrow(cx, cy + (size + gap), size, color);
  }

  // cache last drawn BG so we don't redraw constantly
  static int s_lastBgDrawn = -9999;

  void begin() {
    s_tft.init();
    s_tft.setRotation(1);
    s_tft.fillScreen(TFT_BLACK);
  }

  TFT_eSPI& tft() { return s_tft; }

  static void drawTrendIcon(int cx, int cy, int size, uint16_t color, const char* dir) {
    if (!dir) return;

    if (!strcmp(dir, "DoubleUp"))        { drawDoubleUp(cx, cy, size, 2, color); return; }
    if (!strcmp(dir, "SingleUp"))        { drawUpArrow(cx, cy, size, color); return; }
    if (!strcmp(dir, "FortyFiveUp"))     { drawDiagUpRightArrow(cx, cy, size, color); return; }

    if (!strcmp(dir, "Flat")) {
      // A simple "→" vibe: small bar + triangle
      s_tft.fillRect(cx - size, cy - 2, size, 4, color);  // shaft
      drawRightArrow(cx + (size/2), cy, size/2, color);   // head
      return;
    }

    if (!strcmp(dir, "FortyFiveDown"))   { drawDiagDownRightArrow(cx, cy, size, color); return; }
    if (!strcmp(dir, "SingleDown"))      { drawDownArrow(cx, cy, size, color); return; }
    if (!strcmp(dir, "DoubleDown"))      { drawDoubleDown(cx, cy, size, 2, color); return; }

    // NOT COMPUTABLE / RATE OUT OF RANGE → draw a small "dot" or nothing
    s_tft.fillCircle(cx, cy, 3, color);
  }

  // Helper to draw BG box contents
  static uint16_t blend565(uint16_t c1, uint16_t c2, float t) {
  uint8_t r1 = (c1 >> 11) & 0x1F;
  uint8_t g1 = (c1 >> 5)  & 0x3F;
  uint8_t b1 =  c1        & 0x1F;

  uint8_t r2 = (c2 >> 11) & 0x1F;
  uint8_t g2 = (c2 >> 5)  & 0x3F;
  uint8_t b2 =  c2        & 0x1F;

  uint8_t r = r1 + (r2 - r1) * t;
  uint8_t g = g1 + (g2 - g1) * t;
  uint8_t b = b1 + (b2 - b1) * t;

  return (r << 11) | (g << 5) | b;
}

static uint16_t computePulsedBorderColor() {
  uint16_t baseColor = TFT_WHITE;
  bool pulse = false;
  static uint32_t pulseTime = 2000;

  if (Nightscout::hasValue()) {
    int bg = Nightscout::sgv();

    if (bg >= 250) { baseColor = TFT_ORANGE; pulse = true;  pulseTime = 10000;}
    else if (bg >= 181) { baseColor = TFT_YELLOW;  pulse = false; }
    else if (bg >= 71)  { baseColor = TFT_GREEN;  pulse = false; }
    else if (bg >= 55)  { baseColor = TFT_YELLOW;  pulse = true; pulseTime = 2500; }
    else { baseColor = TFT_RED; pulse = true;  pulseTime = 1000;}
  }

  if (!pulse) return baseColor;

  const uint32_t periodMs = pulseTime;
  float phase = (millis() % periodMs) / (float)periodMs;
  float t = (phase < 0.5f) ? (phase * 2.0f) : ((1.0f - phase) * 2.0f);

  return blend565(baseColor, TFT_WHITE, t);
}

static void drawBorderOnly(int boxX, int boxY, int boxW, int boxH) {
  uint16_t borderColor = computePulsedBorderColor();
  if (borderColor == s_lastBorderColor) return;   // avoid redundant draws
  s_lastBorderColor = borderColor;

  s_tft.drawRect(boxX, boxY, boxW, boxH, borderColor);
}

  static void drawBgInBox(int boxX, int boxY, int boxW, int boxH) {
    drawBorderOnly(boxX, boxY, boxW, boxH);

    // Clear inside of box only
    s_tft.fillRect(boxX + 1, boxY + 1, boxW - 2, boxH - 2, TFT_BLACK);

    s_tft.setTextDatum(MC_DATUM);

    if (Nightscout::hasValue()) {
      int bg = Nightscout::sgv();

      // Big BG number
      uint16_t bgColor =
        (bg >= 250) ? TFT_ORANGE :   // very high
        (bg >= 181) ? TFT_YELLOW :   // high
        (bg >= 71)  ? TFT_GREEN  :   // in range
        (bg >= 55)  ? TFT_YELLOW :   // low
                      TFT_RED;       // urgent low

      s_tft.setTextColor(bgColor, TFT_BLACK);
      s_tft.setTextSize(5);
      s_tft.drawString(String(bg), s_tft.width() / 2, boxY + boxH / 2 - 10);


      // draw arrow slightly to the right of the number
      s_tft.setTextSize(3);
      s_tft.setTextColor(bgColor, TFT_BLACK);
      int trendX = (s_tft.width() / 2) + 85;
      int trendY = boxY + boxH / 2 - 14;

      drawTrendIcon(trendX, trendY, 10, bgColor, Nightscout::direction());

      // Units / freshness
      s_tft.setTextSize(2);
      s_tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);

      uint32_t ageMin = (millis() - Nightscout::lastUpdateMs()) / 60000UL;
      if (ageMin >= 15) {
        bgColor = TFT_DARKGREY;
      }

      // mg/dl Label
      s_tft.drawString("mg/dL", s_tft.width() / 2, boxY + boxH / 2 + 35);

      uint16_t freshnessColor =
      (ageMin >= 15) ? TFT_ORANGE :
      (ageMin >= 10) ? TFT_YELLOW :
                      TFT_LIGHTGREY;

      s_tft.setTextColor(freshnessColor, TFT_BLACK);
      
      if (Nightscout::lastUpdateMs() == 0) {
        s_tft.setTextSize(1);
        s_tft.drawString("updated --", s_tft.width() / 2, boxY + boxH / 2 + 55);
      } else if (ageMin == 0) {
        s_tft.setTextSize(1);
        s_tft.drawString("updated just now", s_tft.width() / 2, boxY + boxH / 2 + 55);
      } else {
        s_tft.setTextSize(1);
        s_tft.drawString(String("updated ") + ageMin + "m ago",
                        s_tft.width() / 2,
                        boxY + boxH / 2 + 55);
      }
    } else {
      // Loading state
      s_tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
      s_tft.setTextSize(1);
      s_tft.drawString("Fetching BG...", s_tft.width() / 2, boxY + boxH / 2);
    }
  }

  void showBootScreen() {
    s_tft.fillScreen(TFT_BLACK);

    // Title
    s_tft.setTextColor(TFT_CYAN, TFT_BLACK);
    s_tft.setTextDatum(TC_DATUM);
    s_tft.setTextSize(2);
    s_tft.drawString("McGee Lab", s_tft.width() / 2, 10);

    // Placeholder box (we’ll reuse for BG)
    int boxW = min(240, s_tft.width() - 40);
    int boxH = boxW;
    int boxX = (s_tft.width()  - boxW) / 2;
    int boxY = (s_tft.height() - boxH) / 2;

    s_tft.drawRect(boxX, boxY, boxW, boxH, TFT_WHITE);

    // Draw initial "loading" content
    drawBgInBox(boxX, boxY, boxW, boxH);

    // Footer + Version
    s_tft.setTextSize(1);
    s_tft.setTextColor(TFT_DARKCYAN, TFT_BLACK);

    s_tft.setTextDatum(BL_DATUM);
    s_tft.drawString(SENTINEL_VERSION_TAG, 6, s_tft.height() - 6);

    s_tft.setTextDatum(BC_DATUM);
    s_tft.drawString("Sentinel BG Monitor (WIP)", s_tft.width() / 2, s_tft.height() - 6);
  }

  void tick() {
    // poll Nightscout in the background
    Nightscout::tick();

    // Sound Alerts
    static bool wasHigh = false;

    bool isHigh = Nightscout::hasValue() && Nightscout::sgv() >= 181;

    if (isHigh && !wasHigh) {
      AudioOut::playHighAlert();
    }

    wasHigh = isHigh;

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

    int ageMin = -1;
    if (Nightscout::hasValue() && Nightscout::lastUpdateMs() != 0) {
      ageMin = (int)((uint32_t)(millis() - Nightscout::lastUpdateMs()) / 60000UL);
    }

    // redraw when BG changes OR the displayed minute changes
    if (bg != s_lastBgDrawn || ageMin != s_lastAgeMinDrawn) {
      drawBgInBox(boxX, boxY, boxW, boxH);
      s_lastBgDrawn = bg;
      s_lastAgeMinDrawn = ageMin;
    }

    uint32_t now = millis();
    if ((uint32_t)(now - s_borderAnimMs) >= 50) { // ~20 FPS
      s_borderAnimMs = now;

      // redraw only if we're in a pulsing state
      int bg = Nightscout::hasValue() ? Nightscout::sgv() : -1;
      if (bg >= 250 || bg <= 54) {
        drawBorderOnly(boxX, boxY, boxW, boxH);
      }
    }
  }
} // namespace Display
