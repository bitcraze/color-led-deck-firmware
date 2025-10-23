/**
 * ,---------,       ____  _ __
 * |  ,-^-,  |      / __ )(_) /_______________ _____  ___
 * | (  O  ) |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * | / ,--´  |    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *    +------`   /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie control firmware
 *
 * Copyright (C) 2025 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * thermal_control.c - Thermal monitoring and LED brightness throttling
 */

#include "thermal_control.h"
#include "stm32c0xx_hal.h"
#include "stm32c0xx_ll_adc.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>

// Temperature sensor slope from datasheet (µV/°C)
#define TEMPSENSOR_AVGSLOPE_UV_PER_C    (2530)

// Temperature sensor calibration point 1 reference temperature (°C)
#define TEMPSENSOR_CAL1_TEMP_C          (30.0f)

// ADC 12-bit resolution: 2^12 - 1 = 4095 (max digital value)
#define ADC_12BIT_MAX_VALUE             (4095.0f)

float throttlingFactor = 0.0f;
float lastTemperature = 20.0f;

// ADC handle
extern ADC_HandleTypeDef hadc1;

void initThermalADC(void) {
    // Enable internal measurement paths for VREFINT and temperature sensor
    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(hadc1.Instance),
                                   LL_ADC_PATH_INTERNAL_VREFINT | LL_ADC_PATH_INTERNAL_TEMPSENSOR);

    // Wait for temperature sensor stabilization (120µs minimum)
    HAL_Delay(1);

    // Calibrate ADC
    HAL_ADCEx_Calibration_Start(&hadc1);
}

static uint16_t readADCChannel(uint32_t channel) {
    ADC_ChannelConfTypeDef sConfig = {0};

    sConfig.Channel = channel;
    sConfig.Rank = ADC_REGULAR_RANK_1;

    // Internal channels require longer sampling times
    if (channel == ADC_CHANNEL_VREFINT || channel == ADC_CHANNEL_TEMPSENSOR) {
        sConfig.SamplingTime = ADC_SAMPLETIME_160CYCLES_5;
    } else {
        sConfig.SamplingTime = ADC_SAMPLETIME_12CYCLES_5;
    }

    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    HAL_ADC_Start(&hadc1);

    if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) {
        uint16_t result = HAL_ADC_GetValue(&hadc1);
        HAL_ADC_Stop(&hadc1);
        return result;
    }

    HAL_ADC_Stop(&hadc1);
    return 0;
}

static float readTemperature(void) {
    // Read internal reference voltage and temperature sensor
    uint16_t vrefRaw = readADCChannel(ADC_CHANNEL_VREFINT);
    uint16_t tempRaw = readADCChannel(ADC_CHANNEL_TEMPSENSOR);

    // Calculate actual Vref from VREFINT measurement
    uint16_t vrefMillivolts = __LL_ADC_CALC_VREFANALOG_VOLTAGE(vrefRaw, LL_ADC_RESOLUTION_12B);

    float vSenseMv = (float)tempRaw * (float)vrefMillivolts / ADC_12BIT_MAX_VALUE;
    float v30Mv = (float)(*TEMPSENSOR_CAL1_ADDR) * (float)vrefMillivolts / ADC_12BIT_MAX_VALUE;
    float avgSlopeUvPerC = (float)TEMPSENSOR_AVGSLOPE_UV_PER_C;

    float temperatureCelsius = ((vSenseMv - v30Mv) * 1000.0f / avgSlopeUvPerC) + TEMPSENSOR_CAL1_TEMP_C;

    return temperatureCelsius;
}

rgbw_t thermalLimitBrightness(rgbw_t input) {
    const float ALPHA = 0.99f;
    const float TEMP_FILTER_ALPHA = 0.8f;  // Low-pass filter coefficient for temperature
    static uint32_t lastReadTime = 0;
    static float filteredTemperature = 20.0f;  // Filtered temperature value

    uint32_t currentTime = HAL_GetTick();

    // Update temperature reading every 10ms with low-pass filtering
    if (currentTime - lastReadTime >= 10) {
        float rawTemp = readTemperature();
        filteredTemperature = TEMP_FILTER_ALPHA * filteredTemperature + (1.0f - TEMP_FILTER_ALPHA) * rawTemp;
        lastTemperature = filteredTemperature;
        lastReadTime = currentTime;
    }

    // Calculate target throttling
    float targetThrottling;
    if (lastTemperature > SLOPE_START_CELSIUS) {
        if (lastTemperature >= MAX_TEMP_CELSIUS) {
            targetThrottling = 1.0f;
        } else {
            targetThrottling = (lastTemperature - SLOPE_START_CELSIUS) / (MAX_TEMP_CELSIUS - SLOPE_START_CELSIUS);
            targetThrottling = fmaxf(0.0f, fminf(1.0f, targetThrottling));  // Clamp 0-1
        }
    } else {
        targetThrottling = 0.0f;
    }

    // Apply filtering to smooth transitions
    throttlingFactor = ALPHA * throttlingFactor + (1.0f - ALPHA) * targetThrottling;

    rgbw_t output;

    output.r = (uint8_t)(input.r * (1 - throttlingFactor));
    output.g = (uint8_t)(input.g * (1 - throttlingFactor));
    output.b = (uint8_t)(input.b * (1 - throttlingFactor));
    output.w = (uint8_t)(input.w * (1 - throttlingFactor));

    return output;
}
