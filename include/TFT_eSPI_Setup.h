// ---- TFT_eSPI project setup (ESP32 + ILI9488 + XPT2046) ----

#define ILI9488_DRIVER

// Native portrait resolution
#define TFT_WIDTH  320
#define TFT_HEIGHT 480

// SPI pins (your wiring)
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   15
#define TFT_DC   2
#define TFT_RST  4

// Touch
#define TOUCH_CS 33
#define TFT_MISO 19   // Shared with touch DO

// Safe SPI speeds
#define SPI_FREQUENCY       16000000
#define SPI_READ_FREQUENCY   8000000
#define SPI_TOUCH_FREQUENCY 2500000

// Fonts
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
