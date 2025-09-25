#include "color.h"
#include "math.h"

rgbw_t extract_white(const rgb_t *led_rgb) {
    // Extract white component using minimum method
    float W = fminf(fminf(led_rgb->r, led_rgb->g), led_rgb->b);
    W = fmaxf(0.0f, W);

    // Subtract white from RGB channels
    rgbw_t result = {
        .r = fmaxf(0.0f, led_rgb->r - W),
        .g = fmaxf(0.0f, led_rgb->g - W),
        .b = fmaxf(0.0f, led_rgb->b - W),
        .w = W
    };

    return result;
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

rgbw_t rgb_to_rgbw_corrected(rgb_t *input_rgb){
    // Extract white component
    rgbw_t color_rgbw = extract_white(input_rgb);

    rgbw_t led_rgbw = intensity_scaling(&color_rgbw);


    return led_rgbw;
}
