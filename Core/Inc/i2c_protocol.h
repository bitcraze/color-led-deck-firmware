/* SPDX-License-Identifier: GPL-3.0-only */
#pragma once

#include <stdbool.h>
#include <stdint.h>

// Called in interrupt context, only for a complete, known request.
typedef void (*I2cProtocolCommandHandler)(const uint8_t *request, uint8_t *response);

void i2cProtocolInit(I2cProtocolCommandHandler handler);
void i2cProtocolReset(void);
bool i2cProtocolNeedsRecovery(void);
