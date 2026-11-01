#include "usbttl.h"
#include "PinCfg.h"
#include "stm32f1_spl_usart.h"
#include "StdTypes.h"
USART_HandleTypedef usart_usbttlInit;
#define SIZE 80
static char txbuff[SIZE];

static void eraseBuff()
{
    memset(txbuff, '\0', sizeof(txbuff));
}

void usbttl_Init()
{
    GPIO_InitTypeDef usbttl_PinInit;
    // TX config
    usbttl_PinInit.GPIO_Pin = TX_PIN;
    usbttl_PinInit.GPIO_Mode = GPIO_Mode_AF_PP;
    usbttl_PinInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART_PORT, &usbttl_PinInit);
    // Rx config
    usbttl_PinInit.GPIO_Pin = RX_PIN;
    usbttl_PinInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    usbttl_PinInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART_PORT, &usbttl_PinInit);

    // cau hinh usart
    usart_usbttlInit.pUSARTx = USART_SOURCE;
    usart_usbttlInit.USART_Config.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    usart_usbttlInit.USART_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_usbttlInit.USART_Config.USART_Parity = USART_Parity_No;
    usart_usbttlInit.USART_Config.USART_StopBits = USART_StopBits_1;
    usart_usbttlInit.USART_Config.USART_WordLength = USART_WordLength_8b;
    usart_usbttlInit.USART_Config.USART_BaudRate = 9600;

    USART_Init(usart_usbttlInit.pUSARTx, &usart_usbttlInit.USART_Config);
    USART_Cmd(usart_usbttlInit.pUSARTx, ENABLE);
}

void Terminal_InitDebug(uint8_t totalID, const char *name, const char *car_id)
{
    // hien thi terminal
    sprintf(txbuff, "KHOI TAO THANH CONG\nCAR PARKING SYSTEM\n");
    SPL_USART_Send(&usart_usbttlInit, (uint8_t *)txbuff, sizeof(txbuff));
    eraseBuff();
    sprintf(txbuff, "\nDU LIEU THE DOC TU DATABASE\n");
    SPL_USART_Send(&usart_usbttlInit, (uint8_t *)txbuff, sizeof(txbuff));
    for (int i = 0; i < totalID; i++)
    {
        eraseBuff();
        sprintf(txbuff, "Name: %s,License series: %s\n", name, car_id);
        SPL_USART_Send(&usart_usbttlInit, (uint8_t *)txbuff, sizeof(txbuff));
    }
}
void Terminal_IdleDebug()
{
    eraseBuff();
    sprintf(txbuff, "SAN SANG DOC THE\n");
    SPL_USART_Send(&usart_usbttlInit, (uint8_t *)txbuff, sizeof(txbuff));
}
void Terminal_RunDebug(const char *name, const char *car_id)
{
    eraseBuff();
    sprintf(txbuff, "\nTHE RFID HOP LE\n");
    SPL_USART_Send(&usart_usbttlInit, (uint8_t *)txbuff, sizeof(txbuff));
    eraseBuff();
    sprintf(txbuff, "Name: %s,License series: %s\n", name, car_id);
    SPL_USART_Send(&usart_usbttlInit, (uint8_t *)txbuff, sizeof(txbuff));
}
void Terminal_ErrorDebug()
{
    eraseBuff();
    sprintf(txbuff, "\nTHE RFID KHONG HOP LE\n");
    SPL_USART_Send(&usart_usbttlInit, (uint8_t *)txbuff, sizeof(txbuff));
}
