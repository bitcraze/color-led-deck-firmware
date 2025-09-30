#include "thermal_control.h"
#include "stm32c0xx_hal.h"
#include "stm32c0xx_ll_adc.h"
#include <stdint.h>
#include <stdio.h>

// Temperature sensor slope from datasheet (µV/°C)
#define TEMPSENSOR_AVGSLOPE_UV_PER_C    (2530)

// ADC handle
extern ADC_HandleTypeDef hadc1;

// Cached temperature readings
static uint16_t lastVrefRaw = 0;
static uint16_t lastTempRaw = 0;
static uint16_t lastVrefMillivolts = 0;
static int16_t lastTemperatureCelsius = 0;

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

    // Scale factory calibration point to current Vref
    // Factory calibration was done at 3.0V, but actual Vref may differ
    int32_t v30Scaled = ((int32_t)(*TEMPSENSOR_CAL1_ADDR) * (int32_t)vrefMillivolts)
                        / __LL_ADC_DIGITAL_SCALE(LL_ADC_RESOLUTION_12B);

    // Calculate temperature using ST's macro
    int16_t temperatureCelsius = __LL_ADC_CALC_TEMPERATURE_TYP_PARAMS(
                                    TEMPSENSOR_AVGSLOPE_UV_PER_C,
                                    v30Scaled,
                                    TEMPSENSOR_CAL1_TEMP,
                                    vrefMillivolts,
                                    tempRaw,
                                    LL_ADC_RESOLUTION_12B);

    // Cache values for debugging
    lastVrefRaw = vrefRaw;
    lastTempRaw = tempRaw;
    lastVrefMillivolts = vrefMillivolts;
    lastTemperatureCelsius = temperatureCelsius;

    return (float)temperatureCelsius;
}

rgbw_t thermalLimitColor(rgbw_t input) {
    static uint32_t lastReadTime = 0;
    static float lastTemperature = 20.0f;

    uint32_t currentTime = HAL_GetTick();

    static float limitationFactor = 1.0f;

    // Update temperature reading every 100ms
    if (currentTime - lastReadTime >= 100) {
        lastTemperature = readTemperature();
        lastReadTime = currentTime;
    }
    if (lastTemperature > SLOPE_START_CELSIUS) {
        if (lastTemperature >= MAX_TEMP_CELSIUS) {
            limitationFactor = 0.0f;
        } else {
            limitationFactor = (MAX_TEMP_CELSIUS - lastTemperature) / (MAX_TEMP_CELSIUS - SLOPE_START_CELSIUS);
        }
    }
    else {
        limitationFactor = 1.0f;
    }

    rgbw_t output;

    output.r = (uint8_t)(input.r * limitationFactor);
    output.g = (uint8_t)(input.g * limitationFactor);
    output.b = (uint8_t)(input.b * limitationFactor);
    output.w = (uint8_t)(input.w * limitationFactor);

    return output;
}