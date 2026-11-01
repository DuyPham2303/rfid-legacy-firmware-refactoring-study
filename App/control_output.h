#ifndef CONTROL_OUTPUT_H
#define CONTROL_OUTPUT_H

// Khởi tạo các đầu ra servo và LED.
void SignalOutput_ModuleInit();
// Điều khiển chu trình mở và đóng barrier.
void Barrier_Control();
// Điều khiển trạng thái nhấp nháy LED.
void Led_Control();
#endif