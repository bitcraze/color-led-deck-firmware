#include "thermal_control.h"
#include "stm32c0xx_hal.h"
#include <stdint.h>

extern ADC_HandleTypeDef hadc1;

static float readTemperature(void) {
    uint32_t adcValue;

    ADC_ChannelConfTypeDef sConfig = {0};

    // Configure ADC channel for temperature sensor
    sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_160CYCLES_5;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        return -999.0f;
    }

    // ADC calibration and reading (keep this part)
    if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK) {
        return -999.0f;
    }

    if (HAL_ADC_Start(&hadc1) != HAL_OK) {
        return -999.0f;
    }

    if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) != HAL_OK) {
        return -999.0f;
    }

    adcValue = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    // Replace ALL temperature calculation with one macro call:
    int32_t temperature = __HAL_ADC_CALC_TEMPERATURE_TYP_PARAMS(
        2530,                    // Avg_Slope: 2530 µV/°C from STM32C0 datasheet
        760,                     // V30: voltage at 30°C in mV
        30,                      // Temperature at V30 (30°C)
        3300,                    // VREFANALOG_VOLTAGE: analog reference voltage in mV
        adcValue,
        ADC_RESOLUTION_12B
    );

    return (float)temperature;
}

bool thermalShouldCutoff(void) {
      static uint32_t lastReadTime = 0;
      static float lastTemperature = 25.0f;

      uint32_t currentTime = HAL_GetTick();

      // Read temperature every 100ms
      if (currentTime - lastReadTime >= 100) {
          lastTemperature = readTemperature();
          lastReadTime = currentTime;
      }

      // Cutoff if temperature > 30°C
      return (lastTemperature > 25.0f);
}
