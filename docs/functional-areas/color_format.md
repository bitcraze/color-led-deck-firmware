---
title: Color Format
page_id: color_format
---

## RGBW Color Specification

Colors are specified as 8-bit RGBW:
- **R, G, B, W**: 0-255 per channel
- Thermal throttling applies multiplicatively: `output = input × (1 - throttlingFactor)`
- All four channels throttled equally to preserve color balance

## Throttling Behavior

When thermal protection is active, the requested color values are scaled down proportionally. This ensures that:
- Color hue is maintained (ratios between channels stay the same)
- Brightness is reduced smoothly
- White channel is throttled at the same rate as RGB channels
