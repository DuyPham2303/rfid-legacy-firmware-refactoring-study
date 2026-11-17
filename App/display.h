#ifndef DISPLAY_H
#define DISPLAY_H
// Dùng chung các kiểu trạng thái của ứng dụng.
#include "AppTypes.h"

// API USB-TTL: khởi tạo UART và gửi từng trạng thái ra Terminal.
void display_UsbTtlModuleInit(void);
void display_UsbTtlInit(void);
void display_UsbTtlIdle(void);
void display_UsbTtlRun(void);
void display_UsbTtlError(void);

// API LCD: khởi tạo màn hình và hiển thị các trạng thái ứng dụng.
void display_LCDModuleInit(void);
void display_LCDInit(void);
void display_LCDIdle(void);
void display_LCDRun(void);
void display_LCDError(void);
void display_LCDFail(void);

// API điều phối: lấy chế độ hiện tại của ứng dụng.
AppMode_e AppModeUpdate(void);

#endif