#pragma once

#include <stdbool.h>
#include "color.h"

#define MAX_TEMP_CELSIUS 40.0f
#define SLOPE_START_CELSIUS 30.0f

void initThermalADC(void);
rgbw_t thermalLimitColor(rgbw_t input);
void thermalDebugPrint(void);