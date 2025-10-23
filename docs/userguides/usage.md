---
title: Usage
page_id: usage
---

The firmware provides a minimal I2C protocol for controlling RGBW LEDs with built-in thermal protection.

## Quick Start

To set the LED color, send a 5-byte I2C command:

```c
// Command format: [CMD_SET_COLOR, R, G, B, W]
uint8_t command[] = {0x01, 255, 0, 0, 0};  // Full red
```

For complete protocol details including all commands and response formats, see [I2C Protocol](../functional-areas/i2c_protocol.md).

## Integration Examples

For Python examples demonstrating how to control the Color LED deck from a host computer, see:
- [crazyflie-lib-python/examples/color_led_deck/](https://github.com/bitcraze/crazyflie-lib-python/tree/master/examples/color_led_deck)
- [crazyflie-lib-python/examples/generic_led_deck/](https://github.com/bitcraze/crazyflie-lib-python/tree/master/examples/generic_led_deck)

Example applications in the Crazyflie firmware:
- [app_color_led_cycle](https://github.com/bitcraze/crazyflie-firmware/tree/master/examples/app_color_led_cycle) - Color cycling demo
- [app_generic_led_cycle](https://github.com/bitcraze/crazyflie-firmware/tree/master/examples/app_generic_led_cycle) - Generic LED deck controller
