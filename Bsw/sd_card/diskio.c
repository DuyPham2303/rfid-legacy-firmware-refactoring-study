/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "sd_card.h"
#include "diskio.h"		
#include "ff_gen_drv.h"

DSTATUS disk_initialize(BYTE drv) {
    if (drv == RES_OK) return SD_Card_Initialize();
    return STA_NOINIT;
}

DSTATUS disk_status(BYTE drv) {
    if (drv == RES_OK) return SD_Card_Ready();
    return STA_NOINIT;
}
DSTATUS disk_read(BYTE pdrv, BYTE* buff, DWORD sector, UINT count) {
    if (pdrv != 0) return RES_PARERR;  // Ki?m tra ? dia h?p l?
    return SD_Card_Read(buff, sector, count);
}

DSTATUS disk_write(BYTE pdrv, const BYTE* buff, DWORD sector, UINT count){
    if (pdrv == 0) return SD_Card_Write(buff, sector, count);
    return RES_PARERR;
}