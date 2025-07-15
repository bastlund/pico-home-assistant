# Release Information

All releases, version history, and detailed change logs for the Pico Home Assistant project.

> 💾 **Quick Downloads:** [GitHub Releases](https://github.com/bastlund/pico-home-assistant/releases)  
> 🏠 **Back to main:** [README.md](README.md)

---

## Current Release: v0.1.3-alpha

**DS18B20 Digital Temperature Sensor Integration**

This release introduces high-precision external temperature sensing capabilities and dual sensor monitoring.

### 🌡️ **Key Features**
- ✅ **Dual Temperature Monitoring** - Onboard ADC + external DS18B20 sensors
- ✅ **Home Assistant Integration** - Automatic MQTT discovery with separate sensor entities  
- ✅ **Pure C DS18B20 Driver** - Custom 1-Wire implementation optimized for Pico W
- ✅ **Development Tools** - Standalone monitoring application for diagnostics
- ✅ **Robust Error Handling** - Graceful operation when external sensor is disconnected

### 📦 **Installation**
```bash
git clone https://github.com/bastlund/pico-home-assistant.git
cd pico-home-assistant
git checkout v0.1.3-alpha
```

> **Note:** This is an alpha release suitable for testing and development. APIs and configuration may change in future releases.

---

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [v0.1.3-alpha] - 2024-12-XX

### Added
- **DS18B20 Digital Temperature Sensor Support**
  - Pure C DS18B20 driver with custom 1-Wire protocol implementation optimized for Pico W
  - Dual temperature monitoring (onboard ADC + external DS18B20)
  - Separate Home Assistant sensor entities for onboard and external temperature readings
  - Robust error handling for disconnected or failed external sensors
  - Precision timing with interrupt-safe communication using busy_wait
  - New `pico_w_ds18b20_monitor` application for sensor validation and diagnostics

### Changed
- **Enhanced MQTT Discovery** - Improved Home Assistant auto-discovery for dual sensor setup
- **Temperature Accuracy** - DS18B20 provides ±0.5°C accuracy vs ±2°C for onboard sensor
- **Documentation** - Complete hardware wiring instructions and configuration examples

### Fixed
- **Panic Resolution** - Fixed "sleep in exception handler" errors in sensor communication

### Technical Details
- **Hardware Support** - DS18B20 wiring guide with 4.7kΩ pull-up resistor requirements
- **Applications** - `pico_w_sensor` now supports dual temperature sensors with separate MQTT topics

**Note:** As an alpha release, this version is suitable for testing and development. APIs and configuration may change in future releases.

## [v0.1.2-alpha] - 2024-11-XX

### Added
- **Shared Version Display Module** - Extracted common stdio initialization and version display functionality into reusable `version_display.c/h` components
- **Intelligent USB Initialization** - Replaced fixed delays with `stdio_usb_connected()` smart detection
- **Timeout Protection** - Added 10-second timeout for headless operation when no USB connection is available

### Changed
- **Code Architecture** - Eliminated code duplication across all applications (DRY principle)
- **Startup Performance** - Applications start immediately when USB is connected
- **Memory Efficiency** - Reduced code duplication saves flash memory
- **Developer Experience** - Cleaner main() functions focus on core application logic

### Technical Metrics
- **Code Reduction** - ~400+ characters of duplicated code eliminated
- **Consistency** - 100% identical stdio initialization across all applications
- **Reliability** - Smart USB detection with fallback timeout for production deployments

**Note:** As an alpha release, this version is suitable for testing and development. APIs and configuration may change in future releases.

## [v0.1.1-alpha] - 2024-10-XX

### Added
- **Complete MQTT sensor** with Home Assistant auto-discovery
- **Temperature monitoring** via onboard ADC sensor  
- **WiFi connectivity** with automatic reconnection
- **Network diagnostic tools** (ping and WiFi scan utilities)
- **Configurable debug levels** (0-4) for development and production
- **Professional licensing** (BSD-3-Clause) with proper attribution
- **Comprehensive documentation** and build instructions

**Note:** As an alpha release, this version is suitable for testing and development. APIs and configuration may change in future releases.

---

For release downloads and GitHub releases, visit [GitHub Releases](https://github.com/bastlund/pico-home-assistant/releases).
