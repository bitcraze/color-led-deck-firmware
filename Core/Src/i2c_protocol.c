/* SPDX-License-Identifier: GPL-3.0-only */
/* Command-sized I2C reception. Discovery always retains its legacy framing. */
#include "i2c_protocol.h"
#include "protocol.h"
#include "stm32c0xx_hal.h"
#include <string.h>

typedef enum {
  RX_IDLE,
  RX_COMMAND,
  RX_PAYLOAD,
  RX_COMPLETE,
} RxState;

// The HAL STOP handler can drain one pending RXDR byte after reception has
// completed. Reserve a guard byte and reject that overlong request below.
static uint8_t request[COLOR_LED_RX_CAPACITY + 1];
static uint8_t response[TXBUFFERSIZE];
static uint8_t expectedLength;
static RxState rxState;
static volatile bool needsRecovery;
static I2cProtocolCommandHandler processCommand;

static void invalidateResponse(void)
{
  memset(response, 0, sizeof(response));
  response[0] = 0xff; // Cannot be mistaken for a successful GET_VERSION.
}

void i2cProtocolReset(void)
{
  rxState = RX_IDLE;
  expectedLength = 0;
  needsRecovery = false;
  invalidateResponse();
}

void i2cProtocolInit(I2cProtocolCommandHandler handler)
{
  processCommand = handler;
  i2cProtocolReset();
}

bool i2cProtocolNeedsRecovery(void)
{
  return needsRecovery;
}

static uint8_t commandLength(uint8_t command)
{
  switch (command) {
    case CMD_GET_VERSION:
    case CMD_SET_COLOR:
    case CMD_GET_THERMAL_STATUS:
    case CMD_GET_LED_POSITION:
    case CMD_GET_LED_CURRENT:
    case CMD_GET_I2C_ADDR_PIN:
    case CMD_SET_BRIGHTNESS_CORR:
      return COLOR_LED_REQUEST_SIZE;
    case CMD_SET_COLOR_FADE:
      return COLOR_LED_FADE_REQUEST_SIZE;
    default:
      return 0;
  }
}

static void rejectRequest(I2C_HandleTypeDef *hi2c)
{
  rxState = RX_IDLE;
  invalidateResponse();
  // Refuse further bytes in this transaction, but keep STOP/address listening.
  SET_BIT(hi2c->Instance->CR2, I2C_CR2_NACK);
}

static void receive(I2C_HandleTypeDef *hi2c, uint8_t *buffer,
                    uint8_t length, uint32_t options)
{
  if (HAL_I2C_Slave_Seq_Receive_IT(hi2c, buffer, length, options) != HAL_OK) {
    rejectRequest(hi2c);
    needsRecovery = true;
  }
}

// Commit only at a transaction boundary, not when the payload first fills.
static void finishRequest(I2C_HandleTypeDef *hi2c)
{
  if (rxState == RX_COMPLETE &&
      hi2c->pBuffPtr == request + expectedLength) {
    invalidateResponse();
    processCommand(request, response);
  } else if (rxState != RX_IDLE) {
    invalidateResponse();
  }
  rxState = RX_IDLE;
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t direction,
                         uint16_t address)
{
  (void)address;
  // Also handle a repeated START without a preceding STOP.
  finishRequest(hi2c);
  if (direction == I2C_DIRECTION_TRANSMIT) {
    invalidateResponse();
    rxState = RX_COMMAND;
    receive(hi2c, request, 1, I2C_FIRST_FRAME);
  } else {
    if (HAL_I2C_Slave_Seq_Transmit_IT(hi2c, response, sizeof(response),
                                    I2C_FIRST_AND_LAST_FRAME) != HAL_OK) {
      needsRecovery = true;
    }
  }
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  if (rxState == RX_COMMAND) {
    expectedLength = commandLength(request[0]);
    if (expectedLength == 0) {
      rejectRequest(hi2c);
      return;
    }
    rxState = RX_PAYLOAD;
    receive(hi2c, request + 1, expectedLength - 1, I2C_LAST_FRAME);
  } else if (rxState == RX_PAYLOAD) {
    rxState = RX_COMPLETE;
    // A further data byte must be NACKed, not left clock-stretching forever.
    SET_BIT(hi2c->Instance->CR2, I2C_CR2_NACK);
  }
}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
  finishRequest(hi2c);
  if (HAL_I2C_EnableListen_IT(hi2c) != HAL_OK) {
    needsRecovery = true;
  }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
  rejectRequest(hi2c);
  // A short request (or early end to a read) is a transaction error, not a
  // reason to reinitialize the peripheral. HAL handles STOP and listen state.
  if (HAL_I2C_GetError(hi2c) != HAL_I2C_ERROR_AF) {
    needsRecovery = true;
  }
}
