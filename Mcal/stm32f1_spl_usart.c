#include "stm32f1_spl_usart.h"
#include <stdint.h>
void SPL_USART_Send(USART_HandleTypedef* pUSARTHandle,uint8_t* pTxData,uint16_t Size){
	while(Size > 0){
		// Wait until the transmit data register is empty (TXE flag is set)
    while (USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_TXE) == RESET);

    // Transmit the byte
    USART_SendData(pUSARTHandle->pUSARTx,*pTxData);

    // Wait until transmission is complete (TC flag is set)
    while (USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_TC) == RESET);
		pTxData++;
		Size--;
	}
}
void SPL_USART_Receive(USART_HandleTypedef* pUSARTHandle,uint8_t* pRxData,uint16_t Size){
	while(Size > 0){
		while (USART_GetFlagStatus(pUSARTHandle->pUSARTx,USART_FLAG_RXNE) == RESET);
    // Read the received data
		*pRxData = USART_ReceiveData(pUSARTHandle->pUSARTx);
		pRxData++;
		Size--;
	}
}
