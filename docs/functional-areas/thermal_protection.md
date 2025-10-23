---
title: Thermal Protection
page_id: thermal_protection
---

The firmware continuously monitors MCU temperature and automatically reduces LED brightness when needed to prevent damage.

## Temperature Thresholds

- **Below 40°C**: No throttling, full brightness available
- **40°C - 55°C**: Gradual brightness reduction (linear slope)
- **Above 55°C**: Maximum throttling, zero brightness

Throttling is applied smoothly to prevent oscillation and is **not user-overridable** - thermal protection is enforced by the firmware.

## Thermal Algorithm

The thermal protection system uses a two-stage exponential smoothing approach:

1. **Temperature filtering**: Exponential moving average (alpha=0.8) on raw ADC readings, updated every 10ms
2. **Throttling filtering**: Exponential moving average (alpha=0.99) on throttling factor

This dual-filter approach provides:
- Stable temperature readings despite ADC noise
- Smooth brightness transitions without visible flickering
