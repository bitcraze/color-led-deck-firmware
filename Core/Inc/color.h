#pragma once

#include <stdint.h>

typedef struct {
    uint8_t r, g, b;
} rgb_t;

typedef struct {
    uint8_t r, g, b, w;
} rgbw_t;

typedef struct {
    uint8_t r_lumens;
    uint8_t g_lumens;
    uint8_t b_lumens;
    uint8_t w_lumens;
} led_luminance_t;

// LED luminance values from datasheet
static const led_luminance_t LED_LUMINANCE = {
    .r_lumens = 90,
    .g_lumens = 210,
    .b_lumens = 50,
    .w_lumens = 250
};

rgbw_t rgb_to_rgbw_corrected(rgb_t *input_rgb);
rgbw_t rgbw_to_rgbw_corrected(rgbw_t *input_rgbw);
