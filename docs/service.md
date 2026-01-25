# Sentinel BG Monitor — Service Overview

## What this is
ESP32-based glucose monitor display that pulls BG data from Nightscout,
shows it on a TFT display, and triggers audio/visual alerts for high/low events.

## Core Flow (happy path)
1. Boot ESP32
2. Initialize display, audio, Wi-Fi
3. Connect to Wi-Fi
4. Poll Nightscout for latest BG
5. Update UI if values changed
6. Evaluate alert state
7. Play sound / show indicators if needed

Main loop lives in `main.cpp` and calls into modules below.

## Folder Responsibilities

### `main.cpp`
- Boot + main loop orchestration
- Calls alert update, audio, and display logic
- No heavy logic here

### `nightscout/`
- Fetches BG data
- Parses response
- Exposes current BG + trend + timestamp

### `alerts/`
- Decides if high/low alert is active
- Manages snooze and mute timers
- Determines *when* an alert should fire (not how)

### `audio/`
- Owns I2S + MAX98357 setup
- Plays tones / sound effects
- No alert logic here

### `display/`
- All TFT drawing
- Caches last-drawn values to avoid redraws
- Responsible for layout, icons, history display

### `wifi/`
- Manages Wi-Fi connectivity
- Handles reconnects and saved networks

### `system/`
- Pins, settings, compile-time constants
- Version info
- No runtime logic

## Timing & Performance Rules
- No blocking delays in main loop
- Use millis()-based timing
- Avoid heap allocation during loop()

## Common Changes

### Add a new alert
- Add logic in `alerts/`
- Expose a flag or timing signal
- Trigger audio from `main.cpp`
- Update UI indicator in `display/`

### Add a new UI element
- Implement entirely in `display/`
- Cache last values
- Do not pull data directly from Wi-Fi/Nightscout

### Change pins or hardware
- Update `system/settings_pins.h`
- Never hardcode pins elsewhere

## Gotchas
- Wi-Fi reconnects can stall if blocking calls are added
- Audio should never block the loop
- Display redraws are expensive; redraw only when needed
