# Sentinel BG Monitor

[![License](https://img.shields.io/github/license/McGee-Lab/sentinel-bg-monitor)](LICENSE)
[![ESP32](https://img.shields.io/badge/ESP32-Microcontroller-informational)](https://www.espressif.com/en/products/socs/esp32)
[![Arduino](https://img.shields.io/badge/Arduino-Compatible-informational)](https://www.arduino.cc/)
[![C++](https://img.shields.io/badge/C%2B%2B-Firmware-informational)](https://isocpp.org/)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Recommended-informational)](https://platformio.org/)
[![TFT](https://img.shields.io/badge/TFT-Display-informational)](https://en.wikipedia.org/wiki/Thin-film_transistor)
[![ILI9488](https://img.shields.io/badge/ILI9488-Display%20Driver-informational)](https://www.displayfuture.com/Display/datasheet/controller/ILI9488.pdf)
[![WiFi](https://img.shields.io/badge/WiFi-Enabled-informational)](https://en.wikipedia.org/wiki/Wi-Fi)
[![API](https://img.shields.io/badge/API-Nightscout%20Compatible-informational)](https://nightscout.github.io/)

Sentinel is a personal blood glucose (BG) monitoring and alert system built for Type 1 Diabetes care.

It uses the Nightscout API to retrieve Dexcom or Libre data and provides local display and alerts via embedded hardware.

> Sentinel prioritizes a **local-first interface**, **explicit network dependency**, and **clarity over complexity**.
> ⚠️ This is a personal project and **not a medical device**.

---

## Features (Planned)
- Nightscout API integration
- Real-time BG display
- High / Low BG alerts
- Offline-safe behavior
- Configurable thresholds
- Simple Screen Layout with Info
- Simple Alerts
- 3-5 Preset Themes
- Touchscreen Optional Version

## Current Prototype Status
- ⏳ TFT display wiring in progress
- ❌ Audio output not wired
- ❌ Buttons not wired
- ❌ Nightscout polling not implemented

## Next Steps
- Solder display to ESP32 and test
- Solder speaker + amp to ESP32 and test
- Solder buttons to ESP32 and test
- Implement Nightscout polling + parsing
- Add alert logic (high / low thresholds)
- Initial UI layout

---

## Getting Started (WIP)
- Install PlatformIO in VS Code
- Copy `config.example.h` → `config.h`
- Fill in Nightscout URL + API token
- Build & upload to ESP32

---

## Secrets
- `config.h` is ignored by git
- Use `config.example.h` as a template

---

## Security
- All credentials are stored locally
- Secrets are excluded from source control
- Example configuration files are provided

---

## Tech Stack
- ESP32 (Arduino framework)
- Nightscout REST API
- Dexcom G7 (via Nightscout)
- Local TFT display + audio alerts
- Libre (via Nightscout) *(not used here, but can be set up via Nightscout)*

---

## Hardware Used

The current Sentinel prototype uses the following hardware components.

### Core Components
- **ESP32 Dev Board**  
  - [ESP-WROOM-32 Dev Board (Amazon)](https://a.co/d/jlhULrt)

- **TFT Display + Touch**
  - [Hosyond 3.5" TFT LCD Touch Screen](https://a.co/d/iaZIYXb)  
     - Resolution: 480×320  
     - Driver: ILI9488 (SPI)

- **Audio Output**
  - [Teyleten Robot MAX98357 I2S 3W Class-D Audio Amplifier](https://a.co/d/8e25bS1)
  - [DWEII 3W 8Ω Mini Speaker (JST-PH 2.0)](https://a.co/d/5FKaton)

- **Inputs**
  - [Waterproof 12mm momentary push buttons](https://a.co/d/51VKgty)

---

## Enclosure (Planned)

Sentinel will include a custom 3D-printed enclosure designed to house all electronics securely and safely.

### Enclosure Goals
- Desk or nightstand friendly form factor
- Front-mounted TFT display
- Accessible buttons (Snooze / Mute)
- Rear or bottom audio vents
- Passive airflow for ESP32 cooling
- Easy access for firmware updates (USB)

### Design Approach
- CAD designed specifically for the selected hardware
- Modular shell (front + back or clamshell)
- Mounting points for:
  - ESP32 dev board
  - TFT display
  - MAX98357 amplifier
  - Speaker
- No glue required (screws or snap-fit where possible)

### 3D Printing
- Designed for FDM printers
- PLA for early prototypes
- PETG recommended for final prints

> Enclosure files will be included in this repository once the design stabilizes.

---

## Wiring Setup

### Power
| ESP32 | Device | Wire Color |
|-----|------|-----------|
| 3.3V | TFT VCC | Orange |
| 5V | TFT LED | Red |
| GND | TFT GND | Black |
| 5V | MAX98357 VIN | Red |
| GND | MAX98357 GND | Black |
| GND | MAX98357 SD (Shutdown) | Black |


### TFT Display (SPI)
| ESP32 Pin | TFT Pin | Function | Wire Color |
|---------|--------|---------|-----------|
| GPIO 2 | DC / RS | Data / Command | Purple |
| GPIO 4 | RESET | Display Reset | Gray |
| GPIO 15 | CS | Chip Select | Brown |
| GPIO 18 | SCK | SPI Clock | Yellow |
| GPIO 23 | MOSI | SPI Data | Blue |
| — | MISO | **Not Used** | — |


### Touch Controller (SPI) (Currently not wired/not being used)
| ESP32 Pin | Touch Pin | Function | Wire Color |
|---------|-----------|---------|-----------|
| GPIO 18 | TOUCH_CLK | SPI Clock | Yellow |
| GPIO 19 | TOUCH_DO | Data Out | Green |
| GPIO 23 | TOUCH_DIN | Data In | Blue |
| GPIO 33 | TOUCH_CS | Chip Select | Brown |
| — | IRQ | **Not Used** | — |


### Audio (I2S – MAX98357)
| ESP32 Pin | MAX98357 Pin | Function | Wire Color |
|---------|---------------|---------|-----------|
| GPIO 26 | BCLK | Bit Clock | Gray |
| GPIO 25 | LRC | Left/Right Clock | Purple |
| GPIO 22 | DIN | Audio Data | White |

The speaker connects directly to the MAX98357 output terminals.


### Buttons (Internal Pull-Ups Enabled)
| ESP32 Pin | Button | Purpose | Wiring |
|---------|--------|--------|--------|
| GPIO 32 | Yellow Button | Snooze | Button → GND |
| GPIO 27 | Red Button | Mute | Button → GND |

Buttons use the ESP32’s internal pull-up resistors.  
Pressed = **LOW**, Released = **HIGH**.


---


Built with care.
