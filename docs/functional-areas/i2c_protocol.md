---
title: I2C Protocol
page_id: i2c_protocol
---

## Protocol Specification (Version 1)

The LED controller firmware implements a simple I2C slave interface for LED control:

| Command | Code | Request Bytes | Response Bytes | Description |
|---------|------|---------------|----------------|-------------|
| GET_VERSION | 0x00 | 1 | 1 | Returns protocol version |
| SET_COLOR | 0x01 | 5 | 0 | Sets RGBW color [R, G, B, W] |
| GET_THERMAL_STATUS | 0x02 | 1 | 2 | Returns [temp_celsius, throttle_pct] |

## Command Details

### GET_VERSION (0x00)

Returns the protocol version number. Current version is 1.

```c
// Command: CMD_GET_VERSION (0x00)
// Returns: protocol version number
```

### SET_COLOR (0x01)

Sets the RGBW LED color. Takes 5 bytes: command byte followed by R, G, B, W values (0-255 each).

```c
// Command: CMD_SET_COLOR (0x01)
// Format: [CMD, R, G, B, W]
uint8_t command[] = {0x01, 255, 0, 0, 0};  // Full red
```

### GET_THERMAL_STATUS (0x02)

Returns the current thermal status. Response contains two bytes:
- Byte 0: Temperature in degrees Celsius (integer)
- Byte 1: Throttle percentage (0-100%)

```c
// Command: CMD_GET_THERMAL_STATUS (0x02)
// Returns: [temperature_int, throttle_percent]
uint8_t status[2];
// temperature = status[0] (in degrees C)
// throttle = status[1] (0-100%)
```
