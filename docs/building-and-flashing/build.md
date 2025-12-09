---
title: Development Guide
page_id: build
---

## Building

### Prerequisites

- CMake
- ARM GCC toolchain

### Build Commands

```bash
cmake --preset Release
cmake --build build/Release
```

This will generate the firmware binary at `build/Release/color-led.elf`.

## Flashing

Once built, flash the firmware to the board using the Crazyradio and `cfloader`:

```bash
cfloader flash build/Release/color-led.bin deck-bcColorLedBot:col-fw -w radio://0/80/2M/E7E7E7E7E7
# or
cfloader flash build/Release/color-led.bin deck-bcColorLedTop:col-fw -w radio://0/80/2M/E7E7E7E7E7
```

Or with a debugger using probe-rs:

```bash
probe-rs download --chip stm32c011f6ux build/Release/color-led.elf
```

## Debugging

For a more advanced development workflow with debugging capabilities:

### Prerequisites

- **VSCode** with the STM32Cube extension (see [.vscode/extensions.json](../../.vscode/extensions.json))

The STM32Cube VSCode extension will automatically install and configure dependencies.

### Setup

1. Clone the repository
2. Open the project in VSCode
3. Install the recommended extensions when prompted
4. The STM32Cube extension will set up the build environment automatically

### Debug Session

To build, flash, and debug in one step:
- Press `F5` to start a debug session
- Or click the debug button in the VSCode debug panel

### Developer Note: STM32 Option Bytes

When working with dev decks, you may need to configure the STM32 option bytes before flashing:

```bash
STM32_Programmer_CLI -c port=SWD mode=UR -ob RDP=0xAA -ob nBOOT_SEL=0
```

This is required for current dev decks but will be set at production in the future. If flashing fails (regardless of method), check that these option bytes are configured correctly.
