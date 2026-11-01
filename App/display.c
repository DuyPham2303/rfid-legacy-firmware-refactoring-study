// Khai báo các hàm và trạng thái hiển thị của ứng dụng.
#include "display.h"
// Giao tiếp LCD qua I2C.
#include "lcd.h"
// Gửi thông tin gỡ lỗi đến terminal.
#include "usbttl.h"
// Tạo khoảng chờ khi cập nhật giao diện.
#include "Systick.h"

// Lưu trạng thái chế độ, lỗi và yêu cầu làm mới LCD.
ModeStatusCfg_s ModeStatusCfg = {.apptype = INIT_FAIL,
								 .errortype = NORMAL,
								 .IsLcdModeUpdate = false};
// Đối tượng LCD dùng chung trong module hiển thị.
static LCD lcd;

// Trả về chế độ hiện tại để main xử lý.
AppMode_e AppModeUpdate()
{
	return ModeStatusCfg.apptype;
}

// Khởi tạo LCD I2C với địa chỉ và kích thước màn hình.
void LCD_ModuleInit()
{
	LCD_I2C_Init(&lcd, 0x61, 16, 2);
}

// Hiển thị thông báo khởi tạo và chuyển ứng dụng sang trạng thái chờ.
void display_Init()
{
	// Hiển thị thông báo khởi tạo thành công trên LCD.
	LCD_I2C_SetCursor(&lcd, 4, 0);
	LCD_I2C_WriteString(&lcd, "KHOI TAO");
	LCD_I2C_SetCursor(&lcd, 3, 1);
	LCD_I2C_WriteString(&lcd, "THANH CONG");

	// Chuyển sang chế độ chờ và đánh dấu LCD cần cập nhật.
	ModeStatusCfg.apptype = IDLE_MODE;
	ModeStatusCfg.IsLcdModeUpdate = true;
	delay_ms(1000);

	// Gửi trạng thái ban đầu cùng thông tin người dùng ra terminal.
	Terminal_InitDebug(count,
					   list_RfidUserData[RfidUserData_Index].name,
					   list_RfidUserData[RfidUserData_Index].license_plate);
}

// Hiển thị lời nhắc quét thẻ khi hệ thống đang chờ.
void display_Idle()
{
	// Xóa màn hình và cập nhật terminal khi trạng thái vừa thay đổi.
	if (ModeStatusCfg.IsLcdModeUpdate)
	{
		LCD_I2C_Clear(&lcd);
		ModeStatusCfg.IsLcdModeUpdate = false;
		delay_ms(2);

		Terminal_IdleDebug();
	}

	// Giữ lời nhắc quét thẻ trên LCD trong lúc chờ.
	else
	{
		LCD_I2C_SetCursor(&lcd, 4, 0);
		LCD_I2C_WriteString(&lcd, "VUI LONG");
		LCD_I2C_SetCursor(&lcd, 4, 1);
		LCD_I2C_WriteString(&lcd, "QUET THE");
	}
}

// Hiển thị tiến trình đóng/mở barrier và trạng thái LED.
void display_Run()
{
	// Xóa nội dung cũ và gửi thông tin thẻ khi bắt đầu lượt xử lý.
	if (ModeStatusCfg.IsLcdModeUpdate)
	{
		LCD_I2C_Clear(&lcd);
		ModeStatusCfg.IsLcdModeUpdate = false;
		delay_ms(2);
		LCD_I2C_SetCursor(&lcd, 0, 0);

		Terminal_RunDebug(list_RfidUserData[RfidUserData_Index].name,
						  list_RfidUserData[RfidUserData_Index].license_plate);
	}

	// Cập nhật giao diện khi trạng thái barrierSignal đang là CLOSING.
	if (DeviceStatusCfg.barrierSignal == CLOSING)
	{
		// Làm mới LCD trước khi hiển thị trạng thái mới.
		if (ModeStatusCfg.IsLcdModeUpdate)
		{
			LCD_I2C_Clear(&lcd);
			delay_ms(2);
			ModeStatusCfg.IsLcdModeUpdate = false;
		}

		// LED bật biểu thị barrier đang chuyển động mở.
		else
		{
			if (DeviceStatusCfg.LedSignal == ON)
			{
				LCD_I2C_WriteString(&lcd, "BARRIER IS");
				LCD_I2C_SetCursor(&lcd, 0, 1);
				LCD_I2C_WriteString(&lcd, "OPENING");
			}
			// LED tắt biểu thị barrier đã đóng xong.
			else
			{
				LCD_I2C_WriteString(&lcd, "BARRIER CLOSED");
				LCD_I2C_SetCursor(&lcd, 0, 1);
				LCD_I2C_WriteString(&lcd, "SUCCESSFULLY");
				delay_ms(1000);
			}
		}
	}
	// Cập nhật giao diện khi trạng thái barrierSignal đang là OPENING.
	else
	{
		// Xóa nội dung cũ khi có yêu cầu cập nhật LCD.
		if (ModeStatusCfg.IsLcdModeUpdate)
		{
			LCD_I2C_Clear(&lcd);
			delay_ms(1);
			ModeStatusCfg.IsLcdModeUpdate = false;
		}

		// LED bật biểu thị barrier đang chuyển động đóng.
		if (DeviceStatusCfg.LedSignal == ON)
		{
			LCD_I2C_WriteString(&lcd, "BARRIER IS");
			LCD_I2C_SetCursor(&lcd, 0, 1);
			LCD_I2C_WriteString(&lcd, "CLOSING");
		}
		// LED tắt biểu thị barrier đã mở xong.
		else
		{
			LCD_I2C_WriteString(&lcd, "BARRIER OPENED");
			LCD_I2C_SetCursor(&lcd, 0, 1);
			LCD_I2C_WriteString(&lcd, "SUCCESSFULLY");
		}
	}
}

// Hiển thị nguyên nhân thẻ không được chấp nhận.
void display_Error()
{
	// Xóa màn hình và gửi lỗi ra terminal khi trạng thái vừa đổi.
	if (ModeStatusCfg.IsLcdModeUpdate)
	{
		LCD_I2C_Clear(&lcd);
		delay_ms(1);
		ModeStatusCfg.IsLcdModeUpdate = false;

		Terminal_ErrorDebug();
	}
	else
	{
		// Chọn nội dung hiển thị theo loại lỗi RFID.
		switch ((uint8_t)ModeStatusCfg.errortype)
		{
		case UID_NOTFOUND:
			LCD_I2C_SetCursor(&lcd, 0, 0);
			LCD_I2C_WriteString(&lcd, "KHONG CO THE");
			LCD_I2C_SetCursor(&lcd, 5, 1);
			LCD_I2C_WriteString(&lcd, "RFID");
			break;
		case RFID_SCAN_EMPTY:
			LCD_I2C_SetCursor(&lcd, 0, 0);
			LCD_I2C_WriteString(&lcd, "THE RFID");
			LCD_I2C_SetCursor(&lcd, 0, 1);
			LCD_I2C_WriteString(&lcd, "KHONG CO DATA");
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

// Hiển thị thông báo khi khởi tạo hệ thống thất bại.
void display_Fail()
{
	LCD_I2C_SetCursor(&lcd, 4, 0);
	LCD_I2C_WriteString(&lcd, "HE THONG ");
	LCD_I2C_SetCursor(&lcd, 3, 1);
	LCD_I2C_WriteString(&lcd, "KHONG CO DATA");
}