# Sentinel BG Monitor

Sentinel is a personal blood glucose (BG) monitoring and alert system built for Type 1 Diabetes care.

It uses the Nightscout API to retrieve Dexcom G7 data and provides local display and alerts via embedded hardware.

> ⚠️ This is a personal project and **not a medical device**.

---

## Features (Planned)
- Nightscout API integration
- Real-time BG display
- High / Low BG alerts
- Offline-safe behavior
- Configurable thresholds

---

## Tech Stack
- ESP32 (Arduino framework)
- Nightscout REST API
- Dexcom G7 (via Nightscout)
- Local TFT display + audio alerts

---

## Hardware Used

The current Sentinel prototype uses the following hardware components.

### Core Components
- **ESP32 Dev Board**  
  -ESP-WROOM-32 ESP32 ESP-32S (WiFi + Bluetooth)
    https://a.co/d/jlhULrt

- **TFT Display + Touch**
  - Hosyond 3.5" TFT LCD Touch Screen  
    Resolution: 480×320  
    Driver: ILI9488 (SPI)
    https://a.co/d/iaZIYXb

- **Audio Output**
  - Teyleten Robot MAX98357 I2S 3W Class-D Audio Amplifier
    https://a.co/d/8e25bS1
  - DWEII 3W 8Ω Mini Speaker (JST-PH 2.0)
    https://a.co/d/5FKaton

- **Inputs**
  - Waterproof 12mm momentary push buttons
    https://a.co/d/51VKgty

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

---

### TFT Display (SPI)
| ESP32 Pin | TFT Pin | Function | Wire Color |
|---------|--------|---------|-----------|
| GPIO 2 | DC / RS | Data / Command | Purple |
| GPIO 4 | RESET | Display Reset | Gray |
| GPIO 15 | CS | Chip Select | Brown |
| GPIO 18 | SCK | SPI Clock | Yellow |
| GPIO 23 | MOSI | SPI Data | Blue |
| — | MISO | **Not Used** | — |

---

### Touch Controller (SPI)
| ESP32 Pin | Touch Pin | Function | Wire Color |
|---------|-----------|---------|-----------|
| GPIO 18 | CLK | SPI Clock | Yellow |
| GPIO 19 | DO | Data Out | Green |
| GPIO 23 | DIN | Data In | Blue |
| GPIO 33 | CS | Chip Select | Brown |
| — | IRQ | **Not Used** | — |

---

### Audio (I2S – MAX98357)
| ESP32 Pin | MAX98357 Pin | Function | Wire Color |
|---------|---------------|---------|-----------|
| GPIO 26 | BCLK | Bit Clock | Gray |
| GPIO 25 | LRC | Left/Right Clock | Purple |
| GPIO 22 | DIN | Audio Data | White |

The speaker connects directly to the MAX98357 output terminals.

---

### Buttons (Internal Pull-Ups Enabled)
| ESP32 Pin | Button | Purpose | Wiring |
|---------|--------|--------|--------|
| GPIO 32 | Yellow Button | Snooze | Button → GND |
| GPIO 27 | Red Button | Mute | Button → GND |

Buttons use the ESP32’s internal pull-up resistors.  
Pressed = **LOW**, Released = **HIGH**.

---

## Security
- All credentials are stored locally
- Secrets are excluded from source control
- Example configuration files are provided

---

## Status
🚧 In active development

---

Built with care.
