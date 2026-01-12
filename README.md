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

**Sentinel** is a personal blood glucose (BG) monitoring and alert system built for Type 1 Diabetes care.  
It retrieves data from **Nightscout** and presents it locally on dedicated hardware with clear visual and audio alerts.

Sentinel prioritizes:

- Local reliability
- Clear failure states
- Clarity over complexity
- No phone dependency for visibility

> ⚠️ **This is a personal project and is NOT a medical device.**  
> It is provided for educational and personal use only.

---

## Current Release

### 🧪 V0.1.0 – Hardware & Framework Baseline

This release establishes a stable hardware and firmware foundation:

- Display, audio, buttons, WiFi, and system structure are functional
- Modular firmware architecture is in place
- Intended for hardware bring-up, testing, and learning
- No Nightscout polling yet (next milestone)

This version is **not intended for daily medical use**.

---

## Features

### Implemented (V0.1.0)
- Modular firmware architecture
- TFT display initialization & placeholder UI
- Audio output via I2S (MAX98357)
- Button input with debounce
- Hardware test mode
- WiFi subsystem scaffold
- Clean boot sequence
- Touchscreen wired but intentionally unused

### Planned / In Progress
- Nightscout API polling
- Real-time BG display
- Trend arrows and delta display
- High / low BG alerts (audio + visual)
- Offline-safe behavior
- Configurable thresholds
- Simple UI layouts
- Preset visual and audio themes
- Optional touchscreen interaction

---

## Project Status

This project is in **active development**.

The current focus is:
1. Reliable Nightscout data ingestion
2. Clear on-device presentation
3. Maintainable, understandable firmware

This is intentionally a learning-focused and iterative project.

---

## Getting Started (WIP)

1. Install **PlatformIO** in VS Code
2. Clone the repository
3. Copy: example.platformio.ini and rename to platformio.ini
4. Edit `platformio.ini` and fill in:
- WiFi SSID
- WiFi password
- Nightscout URL
- Nightscout API token
5. Build and upload to the ESP32

> `platformio.ini` is ignored by git to prevent secrets from being committed.

---

## Security & Secrets

- Secrets are provided via PlatformIO build flags
- Credentials are not stored in source files
- `platformio.ini` should not be committed with real credentials
- No secrets are hard-coded in firmware

---

## Tech Stack

- ESP32 (Arduino framework)
- PlatformIO
- Nightscout REST API
- Dexcom G7 (via Nightscout)
- TFT display (ILI9488)
- I2S audio alerts (MAX98357)

---

## Hardware Used

### Core Components
- **ESP32 Dev Board**
- ESP-WROOM-32

- **TFT Display**
- 3.5" 480×320 SPI TFT
- ILI9488 driver

- **Audio**
- MAX98357 I2S Class-D amplifier
- 3W 8Ω speaker

- **Inputs**
- Two momentary push buttons (Snooze / Mute)

---

## Enclosure (Planned)

A custom 3D-printed enclosure is planned with:

- Desk or nightstand-friendly form factor
- Front-mounted display
- Accessible buttons
- Rear or bottom speaker vents
- Passive airflow
- USB access for firmware updates

Enclosure files will be added once the design stabilizes.

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

### TFT Display (SPI)
| ESP32 Pin | TFT Pin | Function | Wire Color |
|---------|--------|---------|-----------|
| GPIO 2 | DC / RS | Data / Command | Purple |
| GPIO 4 | RESET | Display Reset | Gray |
| GPIO 15 | CS | Chip Select | Brown |
| GPIO 18 | SCK | SPI Clock | Yellow |
| GPIO 23 | MOSI | SPI Data | Blue |
| — | MISO | Not Used | — |

### Touch Controller (SPI)
| ESP32 Pin | Touch Pin | Function | Wire Color |
|---------|-----------|---------|-----------|
| GPIO 18 | TOUCH_CLK | SPI Clock | Yellow |
| GPIO 19 | TOUCH_DO | Data Out | Green |
| GPIO 23 | TOUCH_DIN | Data In | Blue |
| GPIO 33 | TOUCH_CS | Chip Select | Brown |
| — | IRQ | Not Used | — |

### Audio (I2S – MAX98357)
| ESP32 Pin | MAX98357 Pin | Function | Wire Color |
|---------|---------------|---------|-----------|
| GPIO 26 | BCLK | Bit Clock | Gray |
| GPIO 25 | LRC | Left/Right Clock | Purple |
| GPIO 22 | DIN | Audio Data | White |
| GPIO 21 | SD (Shutdown / Enable) | Orange |

The speaker connects directly to the MAX98357 output terminals.

### Buttons (Internal Pull-Ups Enabled)
| ESP32 Pin | Button | Purpose | Wiring |
|---------|--------|--------|--------|
| GPIO 32 | Yellow Button | Snooze | Button → GND |
| GPIO 27 | Red Button | Mute | Button → GND |

Pressed = LOW  
Released = HIGH

---

## Roadmap (Short-Term)

- [ ] Implement Nightscout polling module
- [ ] Parse and cache BG data
- [ ] Display current BG and timestamp
- [ ] Add alert thresholds
- [ ] First usable on-device screen

---

Built with care.  
Learning-focused.  
Not a medical device.
