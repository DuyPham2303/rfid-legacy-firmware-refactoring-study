#ifndef	STM32F1_SPL_I2C_H
#define STM32F1_SPL_I2C_H
#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C
#define BIT_WRITE 0
#define BIT_READ  1

typedef struct{
	I2C_TypeDef*	pI2Cx;
	I2C_InitTypeDef I2C_Config;
}I2C_HandleTypedef;

void SPL_I2C_MasterSend(I2C_HandleTypedef* pI2CHandle,uint8_t* pTxData,uint8_t len,uint8_t SlaveAdd);
void SPL_I2C_MasterReceive(I2C_HandleTypedef* pI2CHandle,uint8_t* pRxData,uint8_t len,uint8_t SlaveAdd);
#endif

