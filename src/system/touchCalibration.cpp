#include "touchCalibration.h"
#include <Preferences.h>
#include "esp_system.h"

static Preferences prefs;

// ---------- internal helpers ----------
static inline void sort9(int16_t *a) {
  for (int i = 1; i < 9; i++) {
    int16_t key = a[i];
    int j = i - 1;
    while (j >= 0 && a[j] > key) { a[j + 1] = a[j]; j--; }
    a[j + 1] = key;
  }
}

static bool getRawMedian(XPT2046_Touchscreen &ts, int zMin, TS_Point &out) {
  if (!ts.touched()) return false;

  int16_t xs[9], ys[9], zs[9];
  int got = 0;

  uint32_t start = millis();
  while (got < 9 && (millis() - start) < 1500) {
    if (!ts.touched()) { delay(5); continue; }
    TS_Point p = ts.getPoint();
    if (p.z < zMin) { delay(5); continue; }

    xs[got] = p.x;
    ys[got] = p.y;
    zs[got] = p.z;
    got++;
    delay(8);
  }

  if (got < 9) return false;

  sort9(xs); sort9(ys); sort9(zs);
  out.x = xs[4];
  out.y = ys[4];
  out.z = zs[4];
  return true;
}

static void drawTarget(TFT_eSPI &tft, int16_t x, int16_t y, const char *label) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.drawString("Touch Calibration", tft.width()/2, 30);

  tft.setTextSize(1);
  tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
  tft.drawString(label, tft.width()/2, 55);

  tft.drawLine(x - 18, y, x + 18, y, TFT_GREENYELLOW);
  tft.drawLine(x, y - 18, x, y + 18, TFT_GREENYELLOW);
  tft.drawCircle(x, y, 10, TFT_GREENYELLOW);

  tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
  tft.drawString("Tap and release", tft.width()/2, tft.height() - 25);
}

static TS_Point waitForTap(XPT2046_Touchscreen &ts, int zMin) {
  TS_Point p = {0,0,0};
  while (!ts.touched()) delay(5);
  while (!getRawMedian(ts, zMin, p)) delay(10);
  while (ts.touched()) delay(5);
  return p;
}

// ---------- public API ----------
namespace TouchCalibration {

bool load(TouchCal &cal) {
  if (!prefs.begin("touch", true)) return false;
  bool valid = prefs.getBool("valid", false);
  if (!valid) { prefs.end(); return false; }

  cal.minX = prefs.getShort("minX", cal.minX);
  cal.maxX = prefs.getShort("maxX", cal.maxX);
  cal.minY = prefs.getShort("minY", cal.minY);
  cal.maxY = prefs.getShort("maxY", cal.maxY);
  cal.swapXY = prefs.getBool("swapXY", cal.swapXY);
  cal.flipX  = prefs.getBool("flipX",  cal.flipX);
  cal.flipY  = prefs.getBool("flipY",  cal.flipY);
  cal.tftRotation = prefs.getUChar("rot", cal.tftRotation);
  cal.valid = true;

  prefs.end();
  return true;
}

void save(const TouchCal &cal) {
  if (!prefs.begin("touch", false)) return;

  prefs.putBool("valid", true);
  prefs.putShort("minX", cal.minX);
  prefs.putShort("maxX", cal.maxX);
  prefs.putShort("minY", cal.minY);
  prefs.putShort("maxY", cal.maxY);
  prefs.putBool("swapXY", cal.swapXY);
  prefs.putBool("flipX", cal.flipX);
  prefs.putBool("flipY", cal.flipY);
  prefs.putUChar("rot", cal.tftRotation);

  prefs.end();
}

bool mapRawToPixel(const TouchCal &cal, const TS_Point &raw, TFT_eSPI &tft, int16_t &x, int16_t &y) {
  if (!cal.valid) return false;

  int32_t rx = cal.swapXY ? raw.y : raw.x;
  int32_t ry = cal.swapXY ? raw.x : raw.y;

  int16_t px = (int16_t)map(rx, cal.minX, cal.maxX, 0, tft.width());
  int16_t py = (int16_t)map(ry, cal.minY, cal.maxY, 0, tft.height());

  if (cal.flipX) px = (tft.width() - 1) - px;
  if (cal.flipY) py = (tft.height() - 1) - py;

  px = constrain(px, 0, tft.width() - 1);
  py = constrain(py, 0, tft.height() - 1);

  x = px;
  y = py;
  return true;
}

bool readTouchPixel(TouchCal &cal, TFT_eSPI &tft, XPT2046_Touchscreen &ts, int zMin, int16_t &x, int16_t &y) {
  if (!ts.touched()) return false;

  TS_Point p = ts.getPoint();
  if (p.z < zMin) return false;

  return mapRawToPixel(cal, p, tft, x, y);
}

void runCalibration(TouchCal &cal, TFT_eSPI &tft, XPT2046_Touchscreen &ts, uint8_t tftRotation, int zMin) {
  Serial.println("=== Touch calibration mode ===");

  const int16_t margin = 25;
  int16_t tlx = margin, tly = margin;
  int16_t trx = tft.width() - 1 - margin, try_ = margin;
  int16_t brx = tft.width() - 1 - margin, bry = tft.height() - 1 - margin;
  int16_t blx = margin, bly = tft.height() - 1 - margin;

  drawTarget(tft, tlx, tly, "Tap TOP-LEFT target");
  TS_Point pTL = waitForTap(ts, zMin);
  Serial.printf("TL raw: x=%d y=%d z=%d\n", pTL.x, pTL.y, pTL.z);

  drawTarget(tft, trx, try_, "Tap TOP-RIGHT target");
  TS_Point pTR = waitForTap(ts, zMin);
  Serial.printf("TR raw: x=%d y=%d z=%d\n", pTR.x, pTR.y, pTR.z);

  drawTarget(tft, brx, bry, "Tap BOTTOM-RIGHT target");
  TS_Point pBR = waitForTap(ts, zMin);
  Serial.printf("BR raw: x=%d y=%d z=%d\n", pBR.x, pBR.y, pBR.z);

  drawTarget(tft, blx, bly, "Tap BOTTOM-LEFT target");
  TS_Point pBL = waitForTap(ts, zMin);
  Serial.printf("BL raw: x=%d y=%d z=%d\n", pBL.x, pBL.y, pBL.z);

  // Detect swap by comparing raw delta along top edge
  int32_t dx_x = abs((int32_t)pTR.x - (int32_t)pTL.x);
  int32_t dx_y = abs((int32_t)pTR.y - (int32_t)pTL.y);
  cal.swapXY = (dx_y > dx_x);

  auto rawX = [&](const TS_Point &p)->int32_t { return cal.swapXY ? p.y : p.x; };
  auto rawY = [&](const TS_Point &p)->int32_t { return cal.swapXY ? p.x : p.y; };

  int32_t leftRawX  = (rawX(pTL) + rawX(pBL)) / 2;
  int32_t rightRawX = (rawX(pTR) + rawX(pBR)) / 2;
  int32_t topRawY   = (rawY(pTL) + rawY(pTR)) / 2;
  int32_t botRawY   = (rawY(pBL) + rawY(pBR)) / 2;

  cal.flipX = (leftRawX > rightRawX);
  cal.flipY = (topRawY > botRawY);

  int32_t minX = min(leftRawX, rightRawX);
  int32_t maxX = max(leftRawX, rightRawX);
  int32_t minY = min(topRawY, botRawY);
  int32_t maxY = max(topRawY, botRawY);

  // Small padding reduces edge dead-zones
  int32_t padX = (maxX - minX) / 40;
  int32_t padY = (maxY - minY) / 40;

  cal.minX = (int16_t)(minX - padX);
  cal.maxX = (int16_t)(maxX + padX);
  cal.minY = (int16_t)(minY - padY);
  cal.maxY = (int16_t)(maxY + padY);

  cal.tftRotation = tftRotation;
  cal.valid = true;

  Serial.printf("Cal saved: minX=%d maxX=%d minY=%d maxY=%d swap=%d flipX=%d flipY=%d\n",
                cal.minX, cal.maxX, cal.minY, cal.maxY,
                cal.swapXY, cal.flipX, cal.flipY);

  save(cal);

  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_GREENYELLOW, TFT_BLACK);
  tft.setTextSize(2);
  tft.drawString("Calibration Saved!", tft.width()/2, tft.height()/2 - 10);
  tft.setTextSize(1);
  tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
  tft.drawString("Rebooting...", tft.width()/2, tft.height()/2 + 18);

  delay(800);
  ESP.restart();
}

} // namespace TouchCalibration
