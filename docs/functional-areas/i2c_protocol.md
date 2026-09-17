---
title: I2C Protocol
page_id: i2c_protocol
---

## Protocol Specification (Version 4)

The Crazyflie writes a command, then reads a response for query commands. Requests
have command-specific lengths; every response is nine bytes. Unused request bytes
are zero. Responses start with the command byte and have zero-filled unused bytes.

| Command | Code | Request bytes | Description |
|---------|------|---------------|-------------|
| GET_VERSION | 0x00 | 5 | Returns `[CMD, protocolVersion, 0, ...]` |
| SET_COLOR | 0x01 | 5 | Legacy `[CMD, W, R, G, B]`, instantaneous, already corrected by the sender |
| GET_THERMAL_STATUS | 0x02 | 5 | Returns `[CMD, temp_celsius, throttle_pct, 0, ...]` |
| GET_LED_POSITION | 0x03 | 5 | Returns `[CMD, cached_position, 0, ...]` |
| GET_LED_CURRENT | 0x04 | 5 | Returns `[CMD, R, G, B, W]` with four big-endian uint16 currents in mA |
| GET_I2C_ADDR_PIN | 0x05 | 5 | Returns `[CMD, pin_state, 0, ...]` |
| SET_BRIGHTNESS_CORR | 0x06 | 5 | `[CMD, enable, 0, 0, 0]`, toggles correction for raw colors |
| SET_COLOR_FADE | 0x07 | 9 | `[CMD, W, R, G, B, fadeSeconds(f32)]`, raw color target and fade duration |

`fadeSeconds` is a little-endian float32 in seconds. Zero means instantaneous.
The fade command starts a new fade when the target changes, or when switching from
legacy colors to raw colors. Brightness correction is enabled by default for raw
colors. The legacy color command cancels a fade and bypasses deck-side correction,
so existing senders do not have their colors corrected twice. Thermal limiting
continues to apply to both commands.

### Stable discovery

The version exchange must remain unchanged when adding commands or increasing
buffer capacity:

```text
Write: [0x00, 0, 0, 0, 0]       (5 bytes)
Read:  [0x00, version, 0, ...]  (9 bytes)
```

The Crazyflie currently waits 1 ms between these transactions. The returned value
is the protocol version, not a firmware release identifier. The Crazyflie can
read a different version and refuse normal operation while retaining its separate
deck update path. This does not imply that old Crazyflie firmware accepts version 4.

New features should use new command identifiers without changing the framing or
meaning of existing commands. In particular, `SET_COLOR_FADE` extends the interface
without changing the original `SET_COLOR` command.

### Reception and malformed requests

The receiver first arms a one-byte HAL receive for the command, then arms a second
receive for the remaining bytes. Both stages are within the same I2C write; no
extra START or address is required between them. Commands are dispatched at STOP
or the following address match, after the complete payload has arrived.

Unknown commands and bytes beyond a complete payload are NACKed. Short requests
are discarded. A pending extra byte drained by the HAL STOP handler also causes
the request to be discarded. Reads without a valid query return a response whose
first byte is `0xff`, so an old response cannot masquerade as a successful version
query after a malformed request. The next valid transaction can resume normally;
peripheral errors request reinitialization by the main loop.

### Host regression tests

Run `sh tests/run.sh` from the repository root. This compiles the production I2C
callbacks against a small HAL boundary fake and runs with AddressSanitizer and
UndefinedBehaviorSanitizer. It requires a host C compiler and both sanitizer
runtimes. It covers the five-byte version request, legacy and extended lengths,
truncated and unknown requests, excess data, repeated START, and recovery after
HAL errors. It does not simulate the STM32 peripheral or electrical bus timing.
Upgrade/downgrade order and bus recovery still require hardware validation.
