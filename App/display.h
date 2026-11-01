#ifndef DISPLAY_H
#define DISPLAY_H
// Dùng chung các kiểu trạng thái của ứng dụng.
#include "AppTypes.h"

// Khởi tạo LCD.
void LCD_ModuleInit();
// Lấy chế độ hiện tại của ứng dụng.
AppMode_e AppModeUpdate();

// Hiển thị thông báo khởi tạo.
void display_Init();
// Hiển thị trạng thái chờ quét thẻ.
void display_Idle();
// Hiển thị trạng thái vận hành barrier.
void display_Run();
// Hiển thị lỗi đọc hoặc xác thực thẻ.
void display_Error();
// Hiển thị lỗi khởi tạo hệ thống.
void display_Fail();

#endif