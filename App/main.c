// Cấu hình xung nhịp và các thành phần của ứng dụng.
#include "ClkCfg.h"
#include "Systick.h"
#include "display.h"
#include "control_output.h"
#include "RFID_Scan.h"
#include "SdCard_Read.h"

// Điểm bắt đầu chương trình và vòng lặp điều phối các chế độ.
int main()
{
	// Bật clock cho GPIO và các thiết bị ngoại vi.
	RCC_Cfg();
	// Khởi tạo SysTick để tạo ngắt 1 ms và hàm delay.
	SysTick_Init();
	// Khởi tạo các ngoại vi dùng trong hệ thống.
	SignalOutput_ModuleInit();
	LCD_ModuleInit();
	RFID_ModuleInit();
	SDcard_ModuleInit();

	while (1)
	{
		// Chọn tác vụ tương ứng với trạng thái hiện tại của hệ thống.
		switch (AppModeUpdate())
		{
		case INIT_MODE:
			// Hiển thị kết quả khởi tạo.
			display_Init();
			break;
		case IDLE_MODE:
			// Hiển thị trạng thái chờ quét thẻ.
			display_Idle();
			// Đọc thẻ và chuyển chế độ theo kết quả kiểm tra.
			RFID_Scan();
			break;
		case RUN_MODE:
			// Hiển thị thông tin thẻ và trạng thái barrier.
			display_Run();
			// Điều khiển servo thực hiện chu trình barrier.
			Barrier_Control();
			// Bật/tắt chức năng nhấp nháy LED theo trạng thái barrier.
			Led_Control();
			break;
		case ERROR_MODE:
			// Hiển thị lỗi thẻ rồi quay lại trạng thái chờ.
			display_Error();
			break;
		default:
			// Báo lỗi khởi tạo nếu trạng thái không thuộc các chế độ trên.
			display_Fail();
			break;
		}
		// Kết thúc hàm main sau lượt xử lý đầu tiên.
		return 0;
	}
}
