---
title: Usage
page_id: usage
---

The firmware provides a minimal I2C protocol for controlling RGBW LEDs with built-in thermal protection.

## Quick Start

To set the LED color, send a 9-byte I2C command:

```c
// Command format: [CMD_SET_COLOR_FADE, W, R, G, B, fadeSeconds (float32, little-endian)]
uint8_t command[9] = {0x07, 0, 255, 0, 0};  // Full red
```

Colors are raw (pre-correction) - the deck applies gamma/perceptual correction and
fading itself. See [I2C Protocol](../functional-areas/i2c_protocol.md) for the full
command set.

## Integration Examples

For Python examples demonstrating how to control the Color LED deck from a host computer, see:
- [color_led_deck/color_led_cycle](https://github.com/bitcraze/crazyflie-demos/tree/main/demos/scripts/cflib/color_led_deck/color_led_cycle)
- [color_led_deck/color_led_set](https://github.com/bitcraze/crazyflie-demos/tree/main/demos/scripts/cflib/color_led_deck/color_led_set)
- [generic_led_deck/led_cycle](https://github.com/bitcraze/crazyflie-demos/tree/main/demos/scripts/cflib/generic_led_deck/led_cycle)

Example applications in the Crazyflie firmware:
- [app_color_led_cycle](https://github.com/bitcraze/crazyflie-demos/tree/main/demos/firmware/color_led_cycle) - Color cycling demo
- [app_generic_led_cycle](https://github.com/bitcraze/crazyflie-demos/tree/main/demos/firmware/generic_led_cycle) - Generic LED deck controller
