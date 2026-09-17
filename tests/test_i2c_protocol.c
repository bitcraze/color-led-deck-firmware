/* Exercise the production callbacks with a HAL boundary fake. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "stm32c0xx_hal.h"
#include "i2c_protocol.h"
#include "protocol.h"

static I2C_TypeDef peripheral;
static I2C_HandleTypeDef bus = {.Instance = &peripheral};
static unsigned calls, listens, receivedStages;
static uint8_t command[COLOR_LED_RX_CAPACITY], reply[TXBUFFERSIZE];
static uint16_t lengths[2];
static uint32_t options[2];
static bool failReceive, failTransmit, failListen;

HAL_StatusTypeDef HAL_I2C_Slave_Seq_Receive_IT(I2C_HandleTypeDef *h,
    uint8_t *p, uint16_t n, uint32_t option)
{
  if (failReceive) return HAL_ERROR;
  assert(receivedStages < 2);
  lengths[receivedStages] = n;
  options[receivedStages++] = option;
  h->pBuffPtr = p;
  h->XferCount = n;
  h->ErrorCode = 0;
  h->Instance->CR2 &= ~I2C_CR2_NACK;
  return HAL_OK;
}
HAL_StatusTypeDef HAL_I2C_Slave_Seq_Transmit_IT(I2C_HandleTypeDef *h,
    uint8_t *p, uint16_t n, uint32_t option)
{
  (void)option;
  if (failTransmit) return HAL_ERROR;
  assert(n == sizeof(reply));
  memcpy(reply, p, n);
  h->pBuffPtr = p;
  h->XferCount = 0;
  h->Instance->CR2 &= ~I2C_CR2_NACK;
  return HAL_OK;
}
HAL_StatusTypeDef HAL_I2C_EnableListen_IT(I2C_HandleTypeDef *h)
{
  (void)h;
  listens++;
  return failListen ? HAL_ERROR : HAL_OK;
}
uint32_t HAL_I2C_GetError(I2C_HandleTypeDef *h) { return h->ErrorCode; }

static void dispatch(const uint8_t *request, uint8_t *response)
{
  calls++;
  size_t length = request[0] == CMD_SET_COLOR_FADE ? 9 : 5;
  memcpy(command, request, length);
  if (request[0] != CMD_SET_COLOR && request[0] != CMD_SET_COLOR_FADE &&
      request[0] != CMD_SET_BRIGHTNESS_CORR) {
    response[0] = request[0];
    response[1] = COLOR_LED_PROTOCOL_VERSION;
  }
}
static void begin(void)
{
  receivedStages = 0;
  HAL_I2C_AddrCallback(&bus, I2C_DIRECTION_TRANSMIT, 0);
}
static void byte(uint8_t value)
{
  assert(!(bus.Instance->CR2 & I2C_CR2_NACK));
  assert(bus.XferCount);
  *bus.pBuffPtr++ = value;
  if (--bus.XferCount == 0) HAL_I2C_SlaveRxCpltCallback(&bus);
}
static void stop(void)
{
  if (bus.XferCount) {
    bus.XferCount = 0; // HAL does this before reporting incomplete reception.
    bus.ErrorCode = HAL_I2C_ERROR_AF;
    HAL_I2C_ErrorCallback(&bus);
  }
  HAL_I2C_ListenCpltCallback(&bus);
}
static void readReply(void)
{
  HAL_I2C_AddrCallback(&bus, 1, 0);
  HAL_I2C_ListenCpltCallback(&bus);
}
static void version(void)
{
  unsigned before = calls;
  begin();
  byte(CMD_GET_VERSION);
  assert(lengths[0] == 1 && lengths[1] == 4);
  assert(options[0] == I2C_FIRST_FRAME && options[1] == I2C_LAST_FRAME);
  for (int i = 0; i < 4; i++) byte(0);
  assert(calls == before); // Do not dispatch before the transaction boundary.
  stop();
  assert(calls == before + 1);
  readReply();
  assert(reply[0] == CMD_GET_VERSION && reply[1] == COLOR_LED_PROTOCOL_VERSION);
  for (int i = 2; i < TXBUFFERSIZE; i++) assert(reply[i] == 0);
  assert(!i2cProtocolNeedsRecovery());
}
int main(void)
{
  i2cProtocolInit(dispatch);
  readReply();
  assert(reply[0] == 0xff); // Never a fake version before a valid request.
  version();

  // Every legacy operation keeps the old wire length.
  for (uint8_t cmd = CMD_SET_COLOR; cmd <= CMD_SET_BRIGHTNESS_CORR; cmd++) {
    begin(); byte(cmd);
    assert(lengths[1] == 4);
    for (int i = 1; i <= 4; i++) byte(i);
    stop();
    assert(command[0] == cmd && command[4] == 4);
    if (cmd == CMD_SET_COLOR || cmd == CMD_SET_BRIGHTNESS_CORR) {
      readReply();
      assert(reply[0] == 0xff);
    }
  }

  begin(); byte(CMD_SET_COLOR_FADE);
  assert(lengths[1] == 8);
  for (int i = 1; i <= 8; i++) byte(i);
  stop();
  assert(command[0] == CMD_SET_COLOR_FADE && command[8] == 8);
  readReply();
  assert(reply[0] == 0xff);

  // Each possible truncation of the long command must not reach the handler.
  for (int n = 0; n < 9; n++) {
    unsigned before = calls;
    begin();
    for (int i = 0; i < n; i++) byte(i ? i : CMD_SET_COLOR_FADE);
    stop(); readReply();
    assert(calls == before && reply[0] == 0xff);
    version();
  }

  unsigned before = calls;
  begin(); byte(0xfe);
  assert(bus.Instance->CR2 & I2C_CR2_NACK);
  stop(); readReply();
  assert(calls == before && reply[0] == 0xff);
  version();

  // Full packet: NACK any excess bytes. Model a pending RXDR byte drained by
  // the HAL STOP handler (which writes through pBuffPtr before listen-complete).
  before = calls;
  begin(); byte(CMD_SET_COLOR_FADE);
  for (int i = 1; i < 9; i++) byte(i);
  assert(bus.Instance->CR2 & I2C_CR2_NACK);
  *bus.pBuffPtr++ = 0xaa;
  stop(); readReply();
  assert(calls == before && reply[0] == 0xff);
  version();

  // A repeated START to read commits a complete request, but not a short one.
  before = calls;
  begin(); byte(CMD_GET_VERSION);
  for (int i = 0; i < 4; i++) byte(0);
  readReply();
  assert(calls == before + 1 && reply[0] == CMD_GET_VERSION);
  before = calls;
  begin(); byte(CMD_GET_VERSION); byte(0);
  readReply();
  assert(calls == before && reply[0] == 0xff);
  version();

  // Rearming failures and hardware errors request main-loop recovery.
  failReceive = true;
  begin();
  assert(i2cProtocolNeedsRecovery());
  failReceive = false;
  i2cProtocolReset(); version();
  begin(); failReceive = true; byte(CMD_GET_VERSION);
  assert(i2cProtocolNeedsRecovery());
  failReceive = false;
  i2cProtocolReset(); version();
  failTransmit = true; readReply();
  assert(i2cProtocolNeedsRecovery());
  failTransmit = false;
  i2cProtocolReset(); version();
  failListen = true; HAL_I2C_ListenCpltCallback(&bus);
  assert(i2cProtocolNeedsRecovery());
  failListen = false;
  i2cProtocolReset(); version();
  bus.ErrorCode = HAL_I2C_ERROR_BERR;
  HAL_I2C_ErrorCallback(&bus);
  assert(i2cProtocolNeedsRecovery());
  i2cProtocolReset(); version();
  assert(listens > 0);
  puts("PASS: stable discovery, command lengths, truncation, excess data, unknown commands, repeated START, HAL errors and recovery");
}
