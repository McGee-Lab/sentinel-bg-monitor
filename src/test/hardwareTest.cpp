#include "hardwareTest.h"
#include "../system/touchCalibration.h"

namespace {
  bool touchEdgeTrigger(TFT_eSPI &tft, XPT2046_Touchscreen &ts, TouchCal &cal, int zMin, int16_t &x, int16_t &y) {
    static bool wasTouched = false;

    bool nowTouched = TouchCalibration::readTouchPixel(cal, tft, ts, zMin, x, y);
    if (nowTouched && !wasTouched) {
      wasTouched = true;
      return true;
    }
    if (!nowTouched) wasTouched = false;
    return false;
  }

  void drawTouchTestButtons(TFT_eSPI &tft) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.drawString("READY", tft.width() / 2, 30);

    tft.setTextSize(1);
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.drawString("Tap boxes or press buttons", tft.width() / 2, 55);

    int w = (tft.width() - 60) / 2;
    int h = 100;
    int y = tft.height() / 2 - h / 2;
    int x1 = 20;
    int x2 = 40 + w;

    tft.drawRoundRect(x1, y, w, h, 12, TFT_DARKGREY);
    tft.drawRoundRect(x2, y, w, h, 12, TFT_DARKGREY);

    tft.setTextColor(TFT_GREENYELLOW, TFT_BLACK);
    tft.setTextSize(2);
    tft.drawString("BEEP 1", x1 + w / 2, y + h / 2);

    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString("BEEP 2", x2 + w / 2, y + h / 2);

    tft.setTextSize(1);
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.drawString("Hold BTN1 boot = Calibrate", tft.width() / 2, tft.height() - 34);
    tft.drawString("Hold BTN2 boot = Test Mode",  tft.width() / 2, tft.height() - 20);
  }

  void handleTouchActions(
    TFT_eSPI &tft,
    XPT2046_Touchscreen &ts,
    TouchCal &cal,
    int zMin,
    void (*playBtn1Sound)(),
    void (*playBtn2Sound)()
  ) {
    int16_t x, y;
    if (!touchEdgeTrigger(tft, ts, cal, zMin, x, y)) return;

    Serial.printf("Touch pixel: x=%d y=%d\n", x, y);

    int w = (tft.width() - 60) / 2;
    int h = 100;
    int by = tft.height() / 2 - h / 2;
    int bx1 = 20;
    int bx2 = 40 + w;

    bool inBox1 = (x >= bx1 && x <= (bx1 + w) && y >= by && y <= (by + h));
    bool inBox2 = (x >= bx2 && x <= (bx2 + w) && y >= by && y <= (by + h));

    if (inBox1) {
      Serial.println("Touch -> BEEP 1");
      if (playBtn1Sound) playBtn1Sound();
    } else if (inBox2) {
      Serial.println("Touch -> BEEP 2");
      if (playBtn2Sound) playBtn2Sound();
    }
  }
}

namespace HardwareTest {

void begin(TFT_eSPI &tft) {
  drawTouchTestButtons(tft);
}

void tick(
  TFT_eSPI &tft,
  XPT2046_Touchscreen &ts,
  TouchCal &cal,
  int touchZMin,
  bool btn1PressedEdge,
  bool btn2PressedEdge,
  void (*playBtn1Sound)(),
  void (*playBtn2Sound)()
) {
  if (btn1PressedEdge) {
    Serial.println("BTN1 pressed");
    if (playBtn1Sound) playBtn1Sound();
  }
  if (btn2PressedEdge) {
    Serial.println("BTN2 pressed");
    if (playBtn2Sound) playBtn2Sound();
  }

  handleTouchActions(tft, ts, cal, touchZMin, playBtn1Sound, playBtn2Sound);
}

} // namespace HardwareTest
