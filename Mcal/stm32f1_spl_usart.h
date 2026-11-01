#ifndef	STM32F1_SPL_USART_H
#define STM32F1_SPL_USART_H

#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART

typedef struct{
		USART_TypeDef* pUSARTx;
		USART_InitTypeDef USART_Config;
}USART_HandleTypedef;
void SPL_USART_Send(USART_HandleTypedef* pUSARTHandle,uint8_t* pTxData,uint16_t Size);
void SPL_USART_Receive(USART_HandleTypedef* pUSARTHandle,uint8_t* pRxData,uint16_t Size);
#endif