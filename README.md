# Pico Home Assistant

Home Assistant integration using Raspberry Pi Pico W for IoT sensor applications.

## Overview

This project contains four main applications for the Raspberry Pi Pico W, designed to work with WiFi networks and MQTT brokers.

## Release Information

### Current Release: v0.1.2-alpha

**Code Architecture & Reliability Improvements**

This release focuses on code quality, maintainability, and improved startup reliability:

#### 🏗️ **Code Refactoring & Architecture**
- ✅ **Shared Version Display Module** - Extracted common stdio initialization and version display functionality into reusable `version_display.c/h` components
- ✅ **DRY Principle Implementation** - Eliminated code duplication across all three applications, reducing maintenance overhead
- ✅ **Intelligent USB Initialization** - Replaced fixed delays with `stdio_usb_connected()` smart detection for faster, more reliable startup
- ✅ **Timeout Protection** - Added 10-second timeout for headless operation when no USB connection is available
- ✅ **Consistent User Experience** - All applications now have identical, professional version information display
- ✅ **Improved Build System** - Updated CMakeLists.txt to properly link shared components across all executables

#### 🔧 **Technical Improvements**
- **Startup Performance**: Applications start immediately when USB is connected, instead of waiting fixed delays
- **Code Maintainability**: Single source of truth for stdio/version handling reduces bugs and simplifies updates  
- **Memory Efficiency**: Reduced code duplication saves flash memory across all three applications
- **Developer Experience**: Cleaner main() functions focus on core application logic

#### 📊 **Metrics**
- **Code Reduction**: ~400+ characters of duplicated code eliminated
- **Consistency**: 100% identical stdio initialization across all applications
- **Reliability**: Smart USB detection with fallback timeout for production deployments

**Note:** As an alpha release, this version is suitable for testing and development. APIs and configuration may change in future releases.

### Previous Release: v0.1.1-alpha

The first alpha release of the Pico Home Assistant project included:

- ✅ **Complete MQTT sensor** with Home Assistant auto-discovery
- ✅ **Temperature monitoring** via onboard ADC sensor  
- ✅ **WiFi connectivity** with automatic reconnection
- ✅ **Network diagnostic tools** (ping and WiFi scan utilities)
- ✅ **Configurable debug levels** (0-4) for development and production
- ✅ **Professional licensing** (BSD-3-Clause) with proper attribution
- ✅ **Comprehensive documentation** and build instructions

**Note:** As an alpha release, this version is suitable for testing and development. APIs and configuration may change in future releases.

For release notes and download links, visit [GitHub Releases](https://github.com/bastlund/pico-home-assistant/releases).

## Quick Start

### Get the Latest Stable Release

To get the latest stable version, clone the repository and checkout the release tag:

```bash
git clone https://github.com/bastlund/pico-home-assistant.git
cd pico-home-assistant
git checkout v0.1.2-alpha  # Replace with latest version from releases page
```

Alternatively, download the release directly from [GitHub Releases](https://github.com/bastlund/pico-home-assistant/releases/latest).

**Current stable version:** See [releases page](https://github.com/bastlund/pico-home-assistant/releases) for the latest version.

### Development Version

For the latest development code (may be unstable):

```bash
git clone https://github.com/bastlund/pico-home-assistant.git
cd pico-home-assistant
# Uses main branch by default
```

## Build Targets

### 1. `pico_w_scan` 
WiFi network scanner that discovers and displays available wireless networks with their signal strength and security settings. Useful for network diagnostics and site surveys.

### 2. `pico_w_ping`
Network connectivity tester that performs ICMP ping operations to verify network reachability and measure latency to specified hosts. Essential for troubleshooting network connectivity issues.

### 3. `pico_w_sensor` 
Complete Home Assistant MQTT sensor implementation featuring:
- **Automatic MQTT Discovery** - Registers dual temperature sensors in Home Assistant automatically
- **Dual Temperature Monitoring** - Reads both onboard ADC sensor and external DS18B20 digital sensor
- **Separate Sensor Entities** - Creates two distinct temperature sensors in Home Assistant:
  - **Pico Onboard Temperature** - Internal ADC-based temperature sensor
  - **Pico External Temperature** - High-precision DS18B20 digital sensor (GPIO 2)
- **Real-time Updates** - Publishes temperature changes with configurable intervals and thresholds
- **Availability Tracking** - Reports online/offline status with Last Will and Testament
- **LED Control** - Remotely controllable onboard LED via MQTT commands
- **Robust Connectivity** - Automatic WiFi and MQTT reconnection handling
- **Unique Device ID** - Uses Pico's unique board ID for device identification
- **Error Handling** - Graceful operation when external sensor is not connected

### 4. `pico_w_ds18b20_test`
Standalone DS18B20 temperature sensor test application for development and debugging:
- **Real-time Temperature Display** - Continuous temperature readings from DS18B20 sensor
- **Error Diagnostics** - Detailed error reporting for sensor communication issues
- **LED Status Indicator** - Visual feedback of sensor operation status
- **Development Tool** - Useful for testing DS18B20 wiring and sensor functionality before integration

## Configuration

All applications require configuration through CMake variables. These can be set in your `cmake-tools-kits.json` file or passed directly to CMake.

### Required CMake Variables

#### WiFi Configuration
- `WIFI_SSID` - Your WiFi network name (SSID)
- `WIFI_PASSWORD` - Your WiFi network password

#### MQTT Configuration (for pico_w_sensor)
- `MQTT_SERVER` - MQTT broker IP address or hostname
- `MQTT_PORT` - MQTT broker port (default: 1883)
- `MQTT_USERNAME` - MQTT broker username (optional)
- `MQTT_PASSWORD` - MQTT broker password (optional)

#### Network Testing Configuration (for pico_w_ping)
- `PING_TARGET_IP_STR` - IP address to ping (e.g., "192.168.1.1")

### Optional CMake Variables

#### Home Assistant Discovery
- `HA_DISCOVERY_PREFIX` - Home Assistant MQTT discovery prefix (default: "homeassistant")
- `HA_DEVICE_NAME` - Device name shown in Home Assistant (default: "Pico W Sensor")
- `HA_DEVICE_MODEL` - Device model (default: "Raspberry Pi Pico W")
- `HA_DEVICE_MANUFACTURER` - Device manufacturer (default: "Raspberry Pi Foundation")

#### Temperature Settings
- `TEMPERATURE_UNITS` - Temperature unit, 'C' or 'F' (default: 'C')

#### DS18B20 External Sensor Configuration (for pico_w_sensor and pico_w_ds18b20_test)
- `DS18B20_GPIO_PIN` - GPIO pin number for DS18B20 1-Wire bus (default: 2)
- `TEMPERATURE_SENSOR` - Set to "ds18b20" to enable external sensor support (optional)

**Note**: DS18B20 sensor requires a 4.7kΩ pull-up resistor between the data line and 3.3V. The sensor operates on the 1-Wire protocol and provides high-precision temperature measurements (±0.5°C accuracy).

#### MQTT Device Settings
- `MQTT_DEVICE_NAME` - Base name for MQTT client ID (default: "pico")
- `MQTT_UNIQUE_TOPIC` - Set to 1 to add client name to topics (default: 0)

#### TLS/SSL Configuration (optional)
- `MQTT_TLS_PORT` - MQTT TLS port (default: 8883)
- `MQTT_CERT_INC` - Path to certificate header file for TLS

#### Debug Configuration
- `DEBUG_LEVEL` - Controls debug output verbosity (default: 2)
  - `0` - Errors only
  - `1` - Errors + Warnings  
  - `2` - Errors + Warnings + Info (default)
  - `3` - Errors + Warnings + Info + Debug
  - `4` - Errors + Warnings + Info + Debug + Verbose

**Note**: `DEBUG_LEVEL` can be configured via cmake-tools-kits.json for VS Code users, or passed as a CMake argument: `-DDEBUG_LEVEL=3`

### Example cmake-tools-kits.json Configuration

```json
{
  "name": "GCC 13.2.1 arm-none-eabi",
  "compilers": {
    "C": "/usr/bin/arm-none-eabi-gcc",
    "CXX": "/usr/bin/arm-none-eabi-g++"
  },
  "cmakeSettings": {
    "PICO_BOARD": "pico_w",
    "WIFI_SSID": "YourWiFiNetwork",
    "WIFI_PASSWORD": "YourWiFiPassword",
    "PING_TARGET_IP_STR": "192.168.1.1",
    "MQTT_SERVER": "192.168.1.100",
    "MQTT_PORT": "1883",
    "MQTT_USERNAME": "mqtt_user",
    "MQTT_PASSWORD": "mqtt_password",
    "DEBUG_LEVEL": "2",
    "TEMPERATURE_SENSOR": "ds18b20",
    "DS18B20_GPIO_PIN": "2"
  },
  "isTrusted": true
}
```

## Requirements

### Hardware
- Raspberry Pi Pico W
- DS18B20 temperature sensor (optional, for external temperature monitoring)
- 4.7kΩ pull-up resistor (required for DS18B20 operation)
- Breadboard and jumper wires for sensor connections

### Software
- Pico SDK (latest version recommended)
- Home Assistant with MQTT broker (for sensor application)
- CMake 3.13 or later
- ARM GCC toolchain (arm-none-eabi-gcc)
- VS Code with CMake Tools extension (recommended)

### DS18B20 Wiring

For external temperature sensing, connect the DS18B20 as follows:
- **VDD (Pin 3)** → Pico 3.3V (Pin 36)
- **GND (Pin 1)** → Pico GND (Pin 38)  
- **DQ (Pin 2)** → Pico GPIO 2 (Pin 4) + 4.7kΩ pull-up to 3.3V

**Note**: The pull-up resistor is essential for reliable 1-Wire communication.

## Building

### Using VS Code with CMake Tools

1. Install the CMake Tools extension in VS Code
2. Configure your `cmake-tools-kits.json` with the required variables
3. Select the ARM GCC kit in VS Code
4. Choose your desired build target (pico_w_scan, pico_w_ping, pico_w_sensor, or pico_w_ds18b20_test)
5. Build using Ctrl+Shift+P → "CMake: Build"

### Manual CMake Build

```bash
mkdir build
cd build
cmake .. \
  -DPICO_BOARD=pico_w \
  -DWIFI_SSID="YourWiFiNetwork" \
  -DWIFI_PASSWORD="YourWiFiPassword" \
  -DMQTT_SERVER="192.168.1.100" \
  -DMQTT_PORT="1883" \
  -DMQTT_USERNAME="mqtt_user" \
  -DMQTT_PASSWORD="mqtt_password" \
  -DPING_TARGET_IP_STR="192.168.1.1" \
  -DDEBUG_LEVEL=2 \
  -DTEMPERATURE_SENSOR="ds18b20" \
  -DDS18B20_GPIO_PIN="2"
make -j4
```

#### Debug Level Examples

**For production (minimal output):**
```bash
-DDEBUG_LEVEL=0
```

**For development (full debugging):**
```bash
-DDEBUG_LEVEL=4
```

### Flashing the Firmware

1. Hold the BOOTSEL button while connecting the Pico W to USB
2. Copy the generated `.uf2` file to the Pico's mass storage device
3. The device will automatically reboot and start running your application

## Troubleshooting

### Common Build Issues

- **Missing PICO_SDK_PATH**: Ensure the Pico SDK is properly installed and the environment variable is set
- **WiFi credentials not defined**: All applications require WIFI_SSID and WIFI_PASSWORD to be set
- **MQTT_SERVER not defined**: The sensor application specifically requires MQTT broker configuration

### Runtime Issues

- **WiFi connection failures**: Check credentials and signal strength
- **MQTT connection issues**: Verify broker IP, port, and credentials
- **Home Assistant discovery problems**: Ensure MQTT integration is enabled in Home Assistant


