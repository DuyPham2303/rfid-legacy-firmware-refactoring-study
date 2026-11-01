#ifndef STDTYPES_H
#define STDTYPES_H
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* State machine options in main loop */
typedef enum AppMode_e
{
    INIT_FAIL,
    INIT_MODE,
    IDLE_MODE,
    RUN_MODE,
    ERROR_MODE
} AppMode_e;

/**
 * @brief Trạng thái của hệ thống.
 *
 * Enum này dùng để quản lý kết quả xử lý của các hàm trong hệ thồng
 * Phạm vi sử dụng: toàn bộ module
 */
typedef enum
{

    /**
     * @brief hàm xử lý thành công
     */
    E_OK,

    /**
     * @brief hàm xử lý thất bại
     */
    NOT_OK
} Status;

#endif