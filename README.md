# Pico Home Assistant

Home Assistant integration using Raspberry Pi Pico W for IoT sensor applications.

## Overview

This project contains three main applications for the Raspberry Pi Pico W, designed to work with WiFi networks and MQTT brokers.

## Release Information

### Current Release: v0.1.1-alpha

This is the first alpha release of the Pico Home Assistant project. It includes:

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
git checkout v0.1.1-alpha  # Replace with latest version from releases page
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
- **Automatic MQTT Discovery** - Registers itself in Home Assistant automatically
- **Temperature Monitoring** - Reads onboard temperature sensor and reports in Celsius/Fahrenheit
- **Real-time Updates** - Publishes temperature changes with configurable intervals
- **Availability Tracking** - Reports online/offline status with Last Will and Testament
- **LED Control** - Remotely controllable onboard LED via MQTT commands
- **Robust Connectivity** - Automatic WiFi and MQTT reconnection handling
- **Unique Device ID** - Uses Pico's unique board ID for device identification

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
    "DEBUG_LEVEL": "2"
  },
  "isTrusted": true
}
```

## Requirements

- Raspberry Pi Pico W
- Pico SDK (latest version recommended)
- Home Assistant with MQTT broker (for sensor application)
- CMake 3.13 or later
- ARM GCC toolchain (arm-none-eabi-gcc)
- VS Code with CMake Tools extension (recommended)

## Building

### Using VS Code with CMake Tools

1. Install the CMake Tools extension in VS Code
2. Configure your `cmake-tools-kits.json` with the required variables
3. Select the ARM GCC kit in VS Code
4. Choose your desired build target (pico_w_scan, pico_w_ping, or pico_w_sensor)
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
  -DDEBUG_LEVEL=2
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


