---
title: Color LED Deck Firmware
page_id: home
---

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

{% sub_page_menu %}
