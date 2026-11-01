#ifndef RFID_SCAN_H
#define RFID_SCAN_H

// Quét thẻ và cập nhật chế độ theo thông tin nhận được.
void RFID_Scan(void);
// Khai báo hàm kiểm tra thẻ nếu được triển khai ở module khác.
void RFID_Check(void);
// Khởi tạo module đọc thẻ RFID.
void RFID_ModuleInit(void);

#endif