---
title: Color LED Deck Firmware
page_id: home
---

LED controller firmware for the Color LED expansion deck designed for the [Crazyflie](https://www.bitcraze.io/documentation/system/platform/) drone platform.

## Highlights

- **Safety-first design** - Hardware thermal protection prevents damage through automatic brightness throttling
- **Simple I2C protocol** - Fixed-frame I2C interface for color, fade, and status queries

## Overview

This firmware handles RGBW LED control with built-in thermal protection, brightness
correction, and on-device color fading, driven over a simple I2C interface from the
Crazyflie.

{% sub_page_menu %}
