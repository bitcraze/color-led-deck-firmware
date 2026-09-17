#pragma once
#include <stdint.h>
typedef struct { uint32_t CR2; } I2C_TypeDef;
typedef struct {
  I2C_TypeDef *Instance;
  uint8_t *pBuffPtr;
  uint16_t XferCount;
  uint32_t ErrorCode;
} I2C_HandleTypeDef;
typedef enum { HAL_OK, HAL_ERROR } HAL_StatusTypeDef;
#define I2C_CR2_NACK 1u
#define I2C_DIRECTION_TRANSMIT 0u
#define I2C_FIRST_FRAME 1u
#define I2C_LAST_FRAME 2u
#define I2C_FIRST_AND_LAST_FRAME 3u
#define HAL_I2C_ERROR_AF 1u
#define HAL_I2C_ERROR_BERR 2u
#define SET_BIT(reg, bits) ((reg) |= (bits))
HAL_StatusTypeDef HAL_I2C_Slave_Seq_Receive_IT(I2C_HandleTypeDef *, uint8_t *, uint16_t, uint32_t);
HAL_StatusTypeDef HAL_I2C_Slave_Seq_Transmit_IT(I2C_HandleTypeDef *, uint8_t *, uint16_t, uint32_t);
HAL_StatusTypeDef HAL_I2C_EnableListen_IT(I2C_HandleTypeDef *);
uint32_t HAL_I2C_GetError(I2C_HandleTypeDef *);
void HAL_I2C_AddrCallback(I2C_HandleTypeDef *, uint8_t, uint16_t);
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *);
void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *);
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *);
