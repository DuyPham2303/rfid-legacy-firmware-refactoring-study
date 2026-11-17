#include "display.h"
#include "lcd.h"
#include "usbttl.h"
#include "Systick.h"

// Lưu trạng thái chế độ, lỗi và yêu cầu làm mới LCD.
ModeStatusCfg_s ModeStatusCfg;
// Đối tượng LCD dùng chung trong module hiển thị.
static LCD lcd;

// API nội bộ điều khiển LCD.
// Khởi tạo LCD và thao tác hiển thị dùng chung.
static void display_initLcd(uint8_t address, uint8_t columns, uint8_t rows)
{
	LCD_I2C_Init(&lcd, address, columns, rows);
}

static void display_clearLcd(void)
{
	LCD_I2C_Clear(&lcd);
}

static void display_setCursor(uint8_t column, uint8_t row)
{
	LCD_I2C_SetCursor(&lcd, column, row);
}

static void display_writeAt(uint8_t column, uint8_t row, char *text)
{
	display_setCursor(column, row);
	LCD_I2C_WriteString(&lcd, text);
}

// API USB-TTL: khởi tạo UART và gửi trạng thái ra Terminal.
void display_UsbTtlModuleInit(void)
{
	Usbttl_Init();
}

// Gửi thông tin khởi tạo và danh sách người dùng ra Terminal.
void display_UsbTtlInit(void)
{
	Terminal_InitDebug(count,
					   list_RfidUserData[RfidUserData_Index].name,
					   list_RfidUserData[RfidUserData_Index].license_plate);
}

// Gửi trạng thái sẵn sàng ra Terminal.
void display_UsbTtlIdle(void)
{
	Terminal_IdleDebug();
}

// Gửi thông tin thẻ đang được xử lý ra Terminal.
void display_UsbTtlRun(void)
{
	Terminal_RunDebug(list_RfidUserData[RfidUserData_Index].name,
					  list_RfidUserData[RfidUserData_Index].license_plate);
}

// Gửi trạng thái thẻ không hợp lệ ra Terminal.
void display_UsbTtlError(void)
{
	Terminal_ErrorDebug();
}

// Hàm nội bộ cập nhật trạng thái hiển thị trên LCD.
// Xóa nội dung cũ và gửi thông tin thẻ khi bắt đầu lượt xử lý.
static void display_RunUpdateCardInfo()
{
	if (ModeStatusCfg.IsLcdModeUpdate)
	{
		display_clearLcd();
		ModeStatusCfg.IsLcdModeUpdate = false;
		delay_ms(2);
		display_setCursor(0, 0);

		display_UsbTtlRun();
	}
}

// Hiển thị trạng thái barrier đã đóng.
static void display_RunClosingBarrier()
{
	display_writeAt(0, 0, "BARRIER CLOSED");
	display_writeAt(0, 1, "SUCCESSFULLY");
	delay_ms(1000);
}

// Hiển thị trạng thái barrier đã mở.
static void display_RunOpeningBarrier()
{
	display_writeAt(0, 0, "BARRIER OPENED");
	display_writeAt(0, 1, "SUCCESSFULLY");
}

// Hiển thị barrier đang di chuyển.
static void display_RunMovingBarrier()
{
	display_writeAt(0, 0, "BARRIER IS");
	display_writeAt(0, 1, "MOVING");
}

// API chung của ứng dụng: đọc chế độ hiện tại để main điều phối.
// Trả về chế độ hiện tại để main xử lý.
AppMode_e AppModeUpdate()
{
	return ModeStatusCfg.apptype;
}

// API LCD: khởi tạo màn hình và trạng thái hiển thị ban đầu.
// Khởi tạo LCD I2C với địa chỉ và kích thước màn hình.
void display_LCDModuleInit(void)
{
	display_initLcd(0x61, 16, 2);
	ModeStatusCfg.apptype = INIT_FAIL;
	ModeStatusCfg.errortype = NORMAL;
	ModeStatusCfg.IsLcdModeUpdate = false;
};

// API LCD: hiển thị trạng thái khởi tạo.
// Hiển thị thông báo khởi tạo và chuyển ứng dụng sang trạng thái chờ.
void display_LCDInit(void)
{
	// Hiển thị thông báo khởi tạo thành công trên LCD.
	display_writeAt(4, 0, "KHOI TAO");
	display_writeAt(3, 1, "THANH CONG");

	// Chuyển sang chế độ chờ và đánh dấu LCD cần cập nhật.
	ModeStatusCfg.apptype = IDLE_MODE;
	ModeStatusCfg.IsLcdModeUpdate = true;
	delay_ms(1000);

	// Gửi trạng thái ban đầu cùng thông tin người dùng ra terminal.
	display_UsbTtlInit();
}

// API LCD: hiển thị lời nhắc quét thẻ khi hệ thống chờ.
// Hiển thị lời nhắc quét thẻ khi hệ thống đang chờ.
void display_LCDIdle(void)
{
	// Xóa màn hình và cập nhật terminal khi trạng thái vừa thay đổi.
	if (ModeStatusCfg.IsLcdModeUpdate)
	{
		display_clearLcd();
		ModeStatusCfg.IsLcdModeUpdate = false;
		delay_ms(2);

		display_UsbTtlIdle();
	}

	// Giữ lời nhắc quét thẻ trên LCD trong lúc chờ.
	else
	{
		display_writeAt(4, 0, "VUI LONG");
		display_writeAt(4, 1, "QUET THE");
	}
}

// API LCD: hiển thị trạng thái hoạt động của barrier.
// Hiển thị tiến trình đóng/mở barrier và trạng thái LED.
void display_LCDRun(void)
{
	display_RunUpdateCardInfo();

	if (DeviceStatusCfg.barrierSignal == CLOSED)
	{
		display_RunClosingBarrier();
	}
	else if (DeviceStatusCfg.barrierSignal == OPENED)
	{
		display_RunOpeningBarrier();
	}
	else
	{
		display_RunMovingBarrier();
	}
}

// API LCD: hiển thị lỗi đọc hoặc xác thực thẻ.
// Hiển thị nguyên nhân thẻ không được chấp nhận.
void display_LCDError(void)
{
	// Xóa màn hình và gửi lỗi ra terminal khi trạng thái vừa đổi.
	if (ModeStatusCfg.IsLcdModeUpdate)
	{
		display_clearLcd();
		delay_ms(1);
		ModeStatusCfg.IsLcdModeUpdate = false;

		display_UsbTtlError();
	}
	else
	{
		// Chọn nội dung hiển thị theo loại lỗi RFID.
		switch ((uint8_t)ModeStatusCfg.errortype)
		{
		case UID_NOTFOUND:
			display_writeAt(0, 0, "KHONG CO THE");
			display_writeAt(5, 1, "RFID");
			break;
		case RFID_SCAN_EMPTY:
			display_writeAt(0, 0, "THE RFID");
			display_writeAt(0, 1, "KHONG CO DATA");
			break;
		// Bỏ qua các mã lỗi chưa có nội dung hiển thị.
		default:
			break;
		}

		// Sau thời gian hiển thị lỗi, quay lại chế độ chờ.
		ModeStatusCfg.apptype = IDLE_MODE;
		ModeStatusCfg.IsLcdModeUpdate = true;
		delay_ms(2000);
	}
}

// API LCD: hiển thị lỗi khởi tạo hệ thống.
// Hiển thị thông báo khi khởi tạo hệ thống thất bại.
void display_LCDFail(void)
{
	display_writeAt(4, 0, "HE THONG ");
	display_writeAt(3, 1, "KHONG CO DATA");
}