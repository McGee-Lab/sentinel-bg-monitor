#include <Arduino.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include "esp_system.h"

// Nightscout
#include "nightscout/nightscout.h"

// Subsystems
#include "audio/audio.h"
#include "input/buttons.h"
#include "display/display.h"
#include "wifi/wifi.h"

// System
#include "system/settings_pins.h"
#include "system/settings_touch.h"
#include "system/touchCalibration.h"

// Test
#include "test/hardwareTest.h"

// Version
#include "system/version.h"

// =======================
// GLOBAL OBJECTS
// =======================
XPT2046_Touchscreen ts(Pins::XPT_CS, Pins::XPT_IRQ);
TouchCal gCal = { 200, 3900, 200, 3900, false, false, false, 1, false };

// =======================
// MODE FLAGS
// =======================
bool gTestMode = false;

constexpr int AMP_SD_PIN = 21;

// =======================
// SETUP
// =======================
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.printf("Sentinel %s\n", SENTINEL_VERSION_TAG);
  delay(500);
  Serial.printf("Reset reason: %d\n", (int)esp_reset_reason());

  // Amp enable pin
  pinMode(AMP_SD_PIN, OUTPUT);
  digitalWrite(AMP_SD_PIN, LOW);   // keep amp OFF during init (optional)

  // --- Init subsystems ---
  Buttons::begin();
  Display::begin();
  AudioOut::begin();               // installs I2S driver
  WiFiMgr::begin();
  Nightscout::begin();

  // Now enable amp (reduces pops/crackle)
  digitalWrite(AMP_SD_PIN, HIGH);
  delay(50);

  ts.begin();

  // --- Touch calibration ---
  if (Buttons::isDown(Buttons::BTN1)) {
    TouchCalibration::runCalibration(
      gCal,
      Display::tft(),
      ts,
      1,
      Touch::Z_MIN
    );
  }

  bool ok = TouchCalibration::load(gCal);
  Serial.printf("Touch cal loaded: %s\n", ok ? "YES" : "NO");

  // --- Hardware test mode ---
  gTestMode = Buttons::isDown(Buttons::BTN2);
  if (gTestMode) {
    Serial.println("Hardware Test Mode ON");
    HardwareTest::begin(Display::tft());
  } else {
    Display::showBootScreen();
  }
}


// =======================
// LOOP
// =======================
void loop() {
  Buttons::update();

  bool b1 = Buttons::pressed(Buttons::BTN1);
  bool b2 = Buttons::pressed(Buttons::BTN2);

  if (gTestMode) {
    HardwareTest::tick(
      Display::tft(),
      ts,
      gCal,
      Touch::Z_MIN,
      b1,
      b2,
      AudioOut::playBtn1Sound,
      AudioOut::playBtn2Sound
    );
    delay(5);
    return;
  }

  // --- Normal firmware loop ---
  Display::tick();
  WiFiMgr::tick();

  delay(10);
}
