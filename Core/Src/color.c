#include "color.h"
#include "math.h"

// Gamma correction LUT (gamma = 2.0 with minimum threshold)
// Input 0 -> 0 (off), Input 1-255 -> 3-255 (gamma corrected)
// Minimum output of 3 ensures LEDs start at lowest visible level
static const uint8_t gamma8[256] = {
      0,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,
      4,   4,   4,   4,   5,   5,   5,   5,   5,   5,   6,   6,   6,   6,   6,   7,
      7,   7,   7,   8,   8,   8,   9,   9,   9,  10,  10,  10,  11,  11,  11,  12,
     12,  12,  13,  13,  13,  14,  14,  15,  15,  16,  16,  16,  17,  17,  18,  18,
     19,  19,  20,  20,  21,  21,  22,  23,  23,  24,  24,  25,  25,  26,  27,  27,
     28,  28,  29,  30,  30,  31,  32,  32,  33,  34,  34,  35,  36,  37,  37,  38,
     39,  39,  40,  41,  42,  43,  43,  44,  45,  46,  47,  47,  48,  49,  50,  51,
     52,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,
     66,  67,  68,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,
     83,  84,  86,  87,  88,  89,  90,  91,  93,  94,  95,  96,  97,  99, 100, 101,
    102, 103, 105, 106, 107, 109, 110, 111, 112, 114, 115, 116, 118, 119, 120, 122,
    123, 124, 126, 127, 129, 130, 131, 133, 134, 136, 137, 139, 140, 141, 143, 144,
    146, 147, 149, 150, 152, 153, 155, 156, 158, 160, 161, 163, 164, 166, 167, 169,
    171, 172, 174, 176, 177, 179, 180, 182, 184, 185, 187, 189, 191, 192, 194, 196,
    197, 199, 201, 203, 204, 206, 208, 210, 212, 213, 215, 217, 219, 221, 223, 224,
    226, 228, 230, 232, 234, 236, 238, 239, 241, 243, 245, 247, 249, 251, 253, 255
};

static inline uint8_t applyGamma(uint8_t value) {
    return gamma8[value];
}

rgbw_t intensity_scaling(const rgbw_t *input_rgb) {
    // Scale based on luminance values from datasheet
    // Dynamically find the weakest channel to use as reference
    uint8_t target_lumens = fminf(fminf(fminf(LED_LUMINANCE.r_lumens, LED_LUMINANCE.g_lumens),
                                            LED_LUMINANCE.b_lumens),
                                        LED_LUMINANCE.w_lumens);

    rgbw_t result = {
        .r = input_rgb->r * target_lumens / LED_LUMINANCE.r_lumens,
        .g = input_rgb->g * target_lumens / LED_LUMINANCE.g_lumens,
        .b = input_rgb->b * target_lumens / LED_LUMINANCE.b_lumens,
        .w = input_rgb->w * target_lumens / LED_LUMINANCE.w_lumens
    };

    return result;
}

rgbw_t rgbw_to_rgbw_corrected(rgbw_t *input_rgbw){
    // Apply intensity scaling based on LED datasheet
    rgbw_t led_rgbw = intensity_scaling(input_rgbw);

    // Apply gamma correction for perceptual linearity
    // This makes brightness changes feel uniform across the entire range
    led_rgbw.r = applyGamma(led_rgbw.r);
    led_rgbw.g = applyGamma(led_rgbw.g);
    led_rgbw.b = applyGamma(led_rgbw.b);
    led_rgbw.w = applyGamma(led_rgbw.w);

    return led_rgbw;
}
