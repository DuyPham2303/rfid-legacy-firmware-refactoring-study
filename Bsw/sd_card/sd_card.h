#ifndef SD_CARD_H
#define SD_CARD_H

#include "diskio.h"

// ham khoi tao sd card
DSTATUS SD_Card_Initialize(void);
// kiem tra sd card co hoat dong binh thuong
DSTATUS SD_Card_Ready(void);
// ham doc du lieu tu sd card
DRESULT SD_Card_Read(BYTE *buff, DWORD sector, UINT count);
// ham ghi du lieu xuong sd card
DRESULT SD_Card_Write(const BYTE *buff, DWORD sector, UINT count);

#endif
