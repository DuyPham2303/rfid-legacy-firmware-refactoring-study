#include "stm32f1_spl_i2c.h"


static void I2C_SendPhaseAddress_bitRW(I2C_TypeDef* pI2Cx,uint8_t SlaveAdd,uint8_t bit_RW){
	SlaveAdd = SlaveAdd << 1;
	if(bit_RW == BIT_READ) SlaveAdd |= 0x1; 
	else 	SlaveAdd &= ~(0x1);
	pI2Cx->DR = SlaveAdd;
}
void SPL_I2C_MasterSend(I2C_HandleTypedef* pI2CHandle,uint8_t* pTxData,uint8_t len,uint8_t SlaveAdd){
	//generate start condition
	I2C_GenerateSTART(pI2CHandle->pI2Cx,ENABLE);
	//wait until bus is free
	while(I2C_CheckEvent(pI2CHandle->pI2Cx,I2C_EVENT_MASTER_MODE_SELECT) == RESET);
	//send 7-bit address and Write bit
	I2C_SendPhaseAddress_bitRW(pI2CHandle->pI2Cx,SlaveAdd,BIT_WRITE);
	//wait until slave confirm address and write request from master
	while(I2C_CheckEvent(pI2CHandle->pI2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == RESET);
	while(len > 0){
		I2C_SendData(pI2CHandle->pI2Cx,*pTxData);
		while(I2C_CheckEvent(pI2CHandle->pI2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED) == RESET);
		len--;	
		pTxData++;
	}
	//generate stop condition
	I2C_GenerateSTOP(pI2CHandle->pI2Cx,ENABLE);
}
void SPL_I2C_MasterReceive(I2C_HandleTypedef* pI2CHandle,uint8_t* pRxData,uint8_t len,uint8_t SlaveAdd){
	//generate start condition
	I2C_GenerateSTART(pI2CHandle->pI2Cx,ENABLE);
	//wait until bus is free
	while(I2C_CheckEvent(pI2CHandle->pI2Cx,I2C_EVENT_MASTER_MODE_SELECT) == RESET);
	//send 7-bit address and Write bit
	I2C_SendPhaseAddress_bitRW(pI2CHandle->pI2Cx,SlaveAdd,BIT_READ);
	//wait until slave confirm address and write request from master
	while(I2C_CheckEvent(pI2CHandle->pI2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == RESET);
	while(len > 0){
		if(len == 1){
			I2C_AcknowledgeConfig(pI2CHandle->pI2Cx,DISABLE);	
			I2C_GenerateSTOP(pI2CHandle->pI2Cx,ENABLE);
		}
		while(I2C_CheckEvent(pI2CHandle->pI2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED) == RESET);
		*pRxData = I2C_ReceiveData(pI2CHandle->pI2Cx);
		len--;
		pRxData++;
	}
	if(pI2CHandle->I2C_Config.I2C_Ack == I2C_Ack_Enable){
		I2C_AcknowledgeConfig(pI2CHandle->pI2Cx,ENABLE);
	}
	while(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BUSY) == SET);
}
