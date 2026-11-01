#ifndef SDCARD_READ_H
#define SDCARD_READ_H

#ifdef DISKO_H
// Khai báo giao diện driver đĩa và FatFs khi bật hỗ trợ thẻ SD.
#include "diskio.h"
#include "fatfs.h"
#endif

// Khởi tạo thẻ SD và nạp dữ liệu người dùng.
void SDcard_ModuleInit();
#endif