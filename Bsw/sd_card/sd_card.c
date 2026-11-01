#include "sd_card.h"
#include "PinCfg.h"
#include "stm32f1_spl_spi.h"

// dinh nghia trang thai chan CS
#define SD_CS_LOW() GPIO_ResetBits(SPI_PORT, SD_CS_PIN)
#define SD_CS_HIGH() GPIO_SetBits(SPI_PORT, SD_CS_PIN)
static void SPI1_init()
{
    SPI_InitTypeDef sdcard_SpiInit;
    GPIO_InitTypeDef sdcard_PinInit;

    sdcard_PinInit.GPIO_Pin = MISO_PIN | MOSI_PIN | SCK_PIN;
    sdcard_PinInit.GPIO_Mode = GPIO_Mode_AF_PP;
    sdcard_PinInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_PORT, &sdcard_PinInit);

    sdcard_PinInit.GPIO_Pin = SD_CS_PIN;
    sdcard_PinInit.GPIO_Mode = GPIO_Mode_Out_PP;
    sdcard_PinInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_PORT, &sdcard_PinInit);

    // C?u h�nh SPI1
    sdcard_SpiInit.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    sdcard_SpiInit.SPI_Mode = SPI_Mode_Master;
    sdcard_SpiInit.SPI_DataSize = SPI_DataSize_8b;
    sdcard_SpiInit.SPI_CPOL = SPI_CPOL_Low;
    sdcard_SpiInit.SPI_CPHA = SPI_CPHA_1Edge;
    sdcard_SpiInit.SPI_NSS = SPI_NSS_Soft;
    sdcard_SpiInit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    sdcard_SpiInit.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(HSPI1, &sdcard_SpiInit);
    SPI_Cmd(HSPI1, ENABLE);
}
static uint8_t SPI_Send_Receive(uint8_t data)
{
    while (SPI_I2S_GetFlagStatus(HSPI1, SPI_I2S_FLAG_TXE) == RESET)
        ;
    SPI_I2S_SendData(HSPI1, data);
    while (SPI_I2S_GetFlagStatus(HSPI1, SPI_I2S_FLAG_RXNE) == RESET)
        ;
    return (uint8_t)SPI_I2S_ReceiveData(HSPI1);
}

DSTATUS SD_Card_Initialize(void)
{
    SD_CS_HIGH(); // reset sd card
    for (int i = 0; i < 10; i++)
        SPI_Send_Receive(0xFF); // G?i 80 xung clock d? reset SD

    SD_CS_LOW();            // bat dau giao tiep sd card
    SPI_Send_Receive(0x40); // Gui CMD0 (GO_IDLE_STATE)
    SPI_Send_Receive(0x00);
    SPI_Send_Receive(0x00);
    SPI_Send_Receive(0x00);
    SPI_Send_Receive(0x00);
    SPI_Send_Receive(0x95); // Checksum cho CMD0

    for (int i = 0; i < 10; i++)
    {
        if (SPI_Send_Receive(0xFF) == 0x01)
            break; // cho sd card phan hoi 0x01
    }

    SD_CS_HIGH(); // dung giao tiep sd card
    return RES_OK;
}
DSTATUS SD_Card_Ready(void)
{

    SD_CS_HIGH();
    uint8_t response = SPI_Send_Receive(0xFF);
    SD_CS_LOW();
    return (response == 0xFF) ? RES_OK : RES_ERROR;
}

DRESULT SD_Card_Read(BYTE *buff, DWORD sector, UINT count)
{
    if (count == 0)
        return RES_PARERR;
    SD_CS_LOW();

    SPI_Send_Receive(0x58); // G?i CMD24 (WRITE_SINGLE_BLOCK)
    SPI_Send_Receive((sector >> 24) & 0xFF);
    SPI_Send_Receive((sector >> 16) & 0xFF);
    SPI_Send_Receive((sector >> 8) & 0xFF);
    SPI_Send_Receive(sector & 0xFF);
    SPI_Send_Receive(0xFF); // Dummy CRC

    if (SPI_Send_Receive(0xFF) != 0x00)
    {
        SD_CS_HIGH();
        return RES_ERROR;
    }

    SPI_Send_Receive(0xFE); // Start token

    for (UINT i = 0; i < 512; i++)
    {
        SPI_Send_Receive(buff[i]); // G?i d? li?u
    }

    SPI_Send_Receive(0xFF); // Dummy CRC
    SPI_Send_Receive(0xFF);

    if ((SPI_Send_Receive(0xFF) & 0x1F) != 0x05)
    {
        SD_CS_HIGH();
        return RES_ERROR;
    }

    while (SPI_Send_Receive(0xFF) == 0)
        ; // Ch? ghi xong
    SD_CS_HIGH();
    return RES_OK;
}
DRESULT SD_Card_Write(const BYTE *buff, DWORD sector, UINT count)
{
    if (count == 0)
        return RES_PARERR;
    SD_CS_LOW();

    SPI_Send_Receive(0x58); // G?i CMD24 (WRITE_SINGLE_BLOCK)
    SPI_Send_Receive((sector >> 24) & 0xFF);
    SPI_Send_Receive((sector >> 16) & 0xFF);
    SPI_Send_Receive((sector >> 8) & 0xFF);
    SPI_Send_Receive(sector & 0xFF);
    SPI_Send_Receive(0xFF); // Dummy CRC

    if (SPI_Send_Receive(0xFF) != 0x00)
    {
        SD_CS_HIGH();
        return RES_ERROR;
    }

    SPI_Send_Receive(0xFE); // Start token

    for (UINT i = 0; i < 512; i++)
    {
        SPI_Send_Receive(buff[i]); // G?i d? li?u
    }

    SPI_Send_Receive(0xFF); // Dummy CRC
    SPI_Send_Receive(0xFF);

    if ((SPI_Send_Receive(0xFF) & 0x1F) != 0x05)
    {
        SD_CS_HIGH();
        return RES_ERROR;
    }

    while (SPI_Send_Receive(0xFF) == 0)
        ; // Ch? ghi xong
    SD_CS_HIGH();
    return RES_OK;
}
