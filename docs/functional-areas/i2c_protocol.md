---
title: I2C Protocol
page_id: i2c_protocol
---

## Protocol Specification (Version 4)

Simple I2C slave interface. Every request and response is a fixed 9-byte frame
(`[CMD, data0, ..., data7]`); unused bytes are zero. Colors sent to the deck are
raw (pre-correction) - brightness correction and fading both run on the deck.

| Command | Code | Description |
|---------|------|-------------|
| GET_VERSION | 0x00 | Returns protocol version |
| SET_COLOR | 0x01 | `[CMD, W, R, G, B, fadeSeconds(f32)]` - sets color target + fade duration |
| GET_THERMAL_STATUS | 0x02 | Returns `[temp_celsius, throttle_pct]` |
| GET_LED_POSITION | 0x03 | Returns the cached LED mounting position |
| GET_LED_CURRENT | 0x04 | Returns per-channel current in mA (big-endian uint16 x4: R, G, B, W) |
| GET_I2C_ADDR_PIN | 0x05 | Returns the I2C address select pin state |
| SET_BRIGHTNESS_CORR | 0x06 | `[CMD, enable]` - enables/disables gamma + perceptual correction |

`fadeSeconds` is a float32, little-endian, packed with `memcpy`. A value
of 0 means "instant".

SET_COLOR only starts a new fade when the target color actually changes; the
duration byte is read fresh from that same packet, so there's no way to send a
color and duration out of sync with each other.
