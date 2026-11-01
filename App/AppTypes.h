#ifndef APPTYPES_H
#define APPTYPES_H
// Dùng chung các kiểu dữ liệu chuẩn và kiểu chế độ ứng dụng.
#include "../Bsw/Cfg/StdTypes.h"

// Các nguyên nhân lỗi liên quan đến thẻ RFID.
typedef enum ErrorType_e
{
    NORMAL,
    UID_NOTFOUND, /* UID thẻ không nằm trong danh sách cho phép. */
    RFID_SCAN_EMPTY,
} ErrorType_e;

// Trạng thái tín hiệu của barrier và LED.
typedef enum
{
    OPENING,
    CLOSING,
    ON,
    OFF
} OutSignalType_e;

// Trạng thái đầu ra của các thiết bị.
typedef struct
{
    OutSignalType_e barrierSignal;
    OutSignalType_e LedSignal;
} DeviceStatusCfg_s;

// Trạng thái chế độ, lỗi và yêu cầu cập nhật LCD.
typedef struct ModeStatusCfg_s
{
    AppMode_e apptype;
    ErrorType_e errortype;
    bool IsLcdModeUpdate;
} ModeStatusCfg_s;

// Thông tin người dùng liên kết với thẻ RFID.
typedef struct RfidUserDtypeCfg_s
{
    char *name;          // Tên chủ xe.
    char *license_plate; // Biển số xe.
} RfidUserDtypeCfg_s;

// Dữ liệu người dùng được nạp từ module thẻ SD.
extern int count;
extern struct RfidUserDtypeCfg_s *list_RfidUserData;

// Chỉ số người dùng đang được đối chiếu trong module RFID.
extern uint8_t RfidUserData_Index;

// Trạng thái đầu ra dùng chung với module điều khiển.
extern DeviceStatusCfg_s DeviceStatusCfg;

// Trạng thái ứng dụng dùng chung với module hiển thị.
extern ModeStatusCfg_s ModeStatusCfg;

#endif