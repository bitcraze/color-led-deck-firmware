# Color LED Deck Firmware

LED controller firmware for the Color LED expansion deck designed for the [Crazyflie](https://www.bitcraze.io/documentation/system/platform/) drone platform.

## Highlights

- **Safety-first design** - Hardware thermal protection prevents damage through automatic brightness throttling
- **Simple I2C protocol** - Three commands: get version, set color, check thermal status
- **Building blocks philosophy** - Provides core capabilities; users implement their own LED patterns and control logic

## Overview

This firmware handles RGBW LED control with built-in thermal protection. It follows a "building blocks" approach:
- **What it does**: Thermal monitoring, safe LED driving, I2C communication
- **What it doesn't do**: Animation patterns, color effects, coordination logic

Applications and choreography belong in the Crazyflie firmware or client applications.

### Integration

Control this firmware from:
- [crazyflie-firmware](https://github.com/bitcraze/crazyflie-firmware) - Color LED deck driver
- [crazyflie-lib-python](https://github.com/bitcraze/crazyflie-lib-python) - Python examples and API

## Usage

The firmware provides a minimal I2C protocol with three commands:

### Set LED Color

```c
// Command: CMD_SET_COLOR (0x01)
// Format: [CMD, R, G, B, W]
uint8_t command[] = {0x01, 255, 0, 0, 0};  // Full red
```

### Get Thermal Status

```c
// Command: CMD_GET_THERMAL_STATUS (0x02)
// Returns: [temperature_int, throttle_percent]
uint8_t status[2];
// temperature = status[0] (in degrees C)
// throttle = status[1] (0-100%)
```

### Get Protocol Version

```c
// Command: CMD_GET_VERSION (0x00)
// Returns: protocol version number
```

### Thermal Protection

The firmware continuously monitors MCU temperature and automatically reduces LED brightness when needed:

- **Below 40°C**: No throttling, full brightness available
- **40°C - 55°C**: Gradual brightness reduction (linear slope)
- **Above 55°C**: Maximum throttling, zero brightness

Throttling is applied smoothly to prevent oscillation and is **not user-overridable** - thermal protection is enforced by the firmware.

## Development

### Prerequisites

- **VSCode** with the STM32Cube extension (see [.vscode/extensions.json](.vscode/extensions.json))

The STM32Cube VSCode extension will automatically install and configure dependencies.

### Setup

1. Clone the repository
2. Open the project in VSCode
3. Install the recommended extensions when prompted
4. The STM32Cube extension will set up the build environment automatically

### Building

- Press `Ctrl+Shift+B` to build
- Or click the build button in the CMake extension toolbar

### Debugging

To flash and debug:
- Press `F5` to build, flash, and start debugging
- Or click the debug button in the VSCode debug panel

## Project Structure

```
color-led/
├── Core/
│   ├── Src/
│   │   ├── main.c              # Main application & I2C protocol
│   │   └── thermal_control.c   # Thermal monitoring & throttling
│   └── Inc/
│       ├── protocol.h          # I2C command definitions
│       ├── thermal_control.h   # Thermal protection API
│       └── color.h             # RGBW color type definition
├── Drivers/                    # STM32 HAL drivers
├── cmake/                      # CMake build configuration
├── CMakeLists.txt              # Main CMake project file
├── color-led.ioc               # STM32CubeMX project file
└── README.md                   # This file
```

## Technical Details

### I2C Protocol Specification (Version 1)

The LED controller firmware implements a simple I2C slave interface for LED control:

| Command | Code | Request Bytes | Response Bytes | Description |
|---------|------|---------------|----------------|-------------|
| GET_VERSION | 0x00 | 1 | 1 | Returns protocol version |
| SET_COLOR | 0x01 | 5 | 0 | Sets RGBW color [R, G, B, W] |
| GET_THERMAL_STATUS | 0x02 | 1 | 2 | Returns [temp_celsius, throttle_pct] |

### Thermal Algorithm

The thermal protection system uses a two-stage exponential smoothing approach:

1. **Temperature filtering**: Exponential moving average (alpha=0.8) on raw ADC readings, updated every 10ms
2. **Throttling filtering**: Exponential moving average (alpha=0.99) on throttling factor

This dual-filter approach provides:
- Stable temperature readings despite ADC noise
- Smooth brightness transitions without visible flickering

### Color Format

Colors are specified as 8-bit RGBW:
- **R, G, B, W**: 0-255 per channel
- Thermal throttling applies multiplicatively: `output = input × (1 - throttlingFactor)`
- All four channels throttled equally to preserve color balance

## Examples

For Python examples demonstrating how to control the Color LED deck from a host computer, see:
- [crazyflie-lib-python/examples/color_led_deck/](https://github.com/bitcraze/crazyflie-lib-python/tree/master/examples/color_led_deck)
- [crazyflie-lib-python/examples/generic_led_deck/](https://github.com/bitcraze/crazyflie-lib-python/tree/master/examples/generic_led_deck)

Example applications in the Crazyflie firmware:
- [app_color_led_cycle](https://github.com/bitcraze/crazyflie-firmware/tree/master/examples/app_color_led_cycle) - Color cycling demo
- [app_generic_led_cycle](https://github.com/bitcraze/crazyflie-firmware/tree/master/examples/app_generic_led_cycle) - Generic LED deck controller

## Contributing

This project is part of the Bitcraze ecosystem. For contribution guidelines, see:
- [Bitcraze Contribution Guidelines](https://www.bitcraze.io/development/contribute/general-guidelines/)

When contributing to this firmware, please test thoroughly, especially thermal behavior.

## Author

This firmware was developed by [Bitcraze](https://www.bitcraze.io) as part of the Crazyflie expansion deck ecosystem.

## License

This project is licensed under the GNU General Public License version 3 (GPLv3) or later. See the [LICENSE](LICENSE) file for details.

This project uses the STM32 HAL library, which is licensed under BSD 3-Clause.