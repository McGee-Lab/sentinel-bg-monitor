#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft;

void setup() {
  Serial.begin(115200);
  delay(1000);

  tft.init();
  tft.setRotation(1); // landscape

  tft.fillScreen(TFT_RED);   delay(1500);
  tft.fillScreen(TFT_GREEN); delay(1500);
  tft.fillScreen(TFT_BLUE);  delay(1500);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(20, 20);
  tft.println("Sentinel");
  tft.println("ILI9488 OK");
}

void loop() {}
