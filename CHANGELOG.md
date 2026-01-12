# Changelog

All notable changes to this project will be documented in this file.

This project follows a simple versioning scheme during early development:
- **0.x.y** releases are experimental and subject to change
- APIs, structure, and behavior may change between minor versions

---

## [0.1.0] – January 11th, 2026

### Added
- Modular firmware architecture (audio, input, display, wifi, system, test)
- Audio output subsystem using I2S (MAX98357)
- Button input subsystem with debounce handling
- Display subsystem with placeholder boot screen
- WiFi subsystem scaffold
- Hardware test mode
- Touch calibration scaffolding
- Centralized hardware configuration headers
- Example PlatformIO configuration file for safe credential setup
- Version display on boot screen

### Changed
- Refactored monolithic firmware into modular subsystems
- Cleaned up project structure and file layout

### Removed
- Legacy LED module
- Unused placeholder README files
- Deprecated test and example files

### Notes
- This release establishes the hardware and firmware baseline.
- Nightscout polling and BG display are **not yet implemented**.
- This version is intended for testing, learning, and iteration only.
- **Not a medical device.**

---

## [Unreleased]

### Planned
- Nightscout API polling and parsing
- On-screen blood glucose display
- Trend and delta indicators
- High and low BG alerts (audio and visual)
- Offline and error states
- UI layout iterations

[0.1.0]: https://github.com/McGee-Lab/sentinel-bg-monitor/releases/tag/v0.1.0
