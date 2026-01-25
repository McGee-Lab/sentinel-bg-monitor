# Copilot Instructions — Sentinel BG Monitor

You are helping on **Sentinel BG Monitor**, an ESP32 firmware project.
Your job is to propose **small, safe, compile-friendly changes** that fit the existing module boundaries.

## Tech & Environment
- Target: **ESP32**
- Build: **PlatformIO** (preferred). Keep changes compatible with typical PIO + Arduino core.
- Display: **TFT_eSPI** driving SPI TFT (ILI9488) + touch controller (XPT2046).
- Audio: **MAX98357** over I2S (simple sound effects / beeps).
- Data: **Nightscout** provides BG readings.

## Project Architecture (hard rules)
Follow these boundaries unless the user explicitly requests otherwise:

- `main.cpp`
  - Owns boot/setup and the main loop tick flow.
  - Calls into alert logic + rendering logic.
  - Should stay readable: orchestrate, don’t implement details here.

- `alerts/` (ex: `alerts.cpp`)
  - Contains alert state machine + thresholds + snooze/mute behaviors.
  - Should expose clear functions like:
    - `Alerts::update(...)`
    - `Alerts::isHighActive()` / `Alerts::shouldBeepNow()` etc.
  - **Do not** play audio directly here unless your project already does; prefer returning “what to do” and let main/audio handle output.

- `audio/` (ex: `audio.cpp`)
  - Owns I2S init, generating tones, and playing sound effects.
  - Keep audio generation **non-blocking** where possible.
  - Avoid long `delay()` loops for beeps. Prefer scheduled tick/elapsed timers.

- `display/`
  - Owns all drawing to TFT.
  - Use “draw only when changed” patterns where possible (avoid full-screen redraws).
  - Any UI additions (icons/time/history) go here.

- `wifi/`
  - Owns connectivity logic.
  - If adding features like multiple saved networks, store configuration in `system/` and keep connection attempts non-blocking.

- `system/`
  - All settings, pins, compile-time constants, versioning.
  - Pins live in `system/settings_pins.h` (or similar). **Never** hardcode pins elsewhere.

## Coding Style
- Keep changes minimal and localized.
- Prefer `constexpr`, `enum class`, and small helpers over macros.
- Avoid heap allocation in hot paths.
- Prefer `millis()`-based timing; avoid blocking `delay()` in the main loop.
- Include guards / `#pragma once` in headers (match repo style).
- Keep includes minimal; forward declare when appropriate.

## Safety & Reliability
- Never log or print secrets (Wi-Fi passwords, tokens, Nightscout URL with API key, etc.).
- Don’t introduce busy loops that starve Wi-Fi / UI / touch.
- Handle “no data” cases gracefully (Nightscout missing value).
- When modifying alert behavior, preserve existing thresholds/meaning unless asked to change them.

## How to Make Changes (workflow Copilot should follow)
When asked to implement something:
1. Identify the correct module(s) to touch.
2. Propose a short plan (1–5 bullets).
3. Provide code changes that compile:
   - show full functions or clear diffs
   - include any required new declarations in headers
4. Mention which files changed and why (short).

## Common Feature Patterns (preferred)
### Beep loop until snooze
- Alerts module should decide **when** to beep (every N seconds) based on:
  - high/low active state
  - snooze end timestamp
  - mute enabled
- Audio module should provide `AudioOut::play...()` / `AudioOut::beep(...)`.
- Main loop should call:
  - `Alerts::update(nowMs, bg, ...)`
  - If `Alerts::shouldBeep(nowMs)` then `AudioOut::beep(...)`

### UI additions (Wi-Fi icon, time/date, history)
- Add to `display/` only.
- Cache last values (BG, icons state, text) to avoid redraw spam.
- If adding a “last 5 readings” history tracker:
  - store data in a small ring buffer (in Nightscout module or a `history/` module),
  - render in display with minimal repaint.

## “Do Not” List
- Do not rewrite large parts of the project unless explicitly asked.
- Do not move files or rename public APIs without warning.
- Do not change pin mappings unless asked.
- Do not add heavy dependencies.

## If Anything Is Unclear
Ask for the smallest clarification needed **only if** you truly can’t proceed.
Otherwise, make reasonable assumptions and implement in a safe, minimal way.
