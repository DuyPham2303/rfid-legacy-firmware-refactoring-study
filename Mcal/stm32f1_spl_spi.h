#ifndef	STM32F1_SPL_SPI_H_
#define	STM32F1_SPL_SPI_H_
       
#include "stm32f10x_spi.h" 
typedef struct
{
  SPI_TypeDef    *pSPIx;    
  SPI_InitTypeDef  Config;   
}SPI_HandleTypedef;
void SPL_SPI_Transmit(SPI_HandleTypedef *hspi, void* pTxData,uint16_t Size);
void SPL_SPI_Receive(SPI_HandleTypedef *hspi,void* pRxData,uint16_t Size);
void SPL_SPI_TxRx(SPI_HandleTypedef* hspi,uint8_t* pTxData,uint8_t* pRxData,uint16_t Size);
void SPL_SPI_RxTx(SPI_HandleTypedef* hspi,uint8_t* pTxData,uint8_t* pRxData,uint16_t Size);
#endif
