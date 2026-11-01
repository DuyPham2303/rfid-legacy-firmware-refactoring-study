// Khai báo các hàm điều khiển đầu ra.
#include "control_output.h"
#include "servo.h"
#include "AppTypes.h"
#include "Systick.h"

// Mốc tick đo thời gian chờ giữa các lần chuyển động barrier.
uint32_t wait_time = 0;
// Mốc thời gian cập nhật góc servo.
uint32_t servo_updateTime;
// Trạng thái hiện tại của barrier và LED.
DeviceStatusCfg_s DeviceStatusCfg = {.barrierSignal = CLOSING,
									 .LedSignal = OFF};

// Khởi tạo servo và LED.
void SignalOutput_ModuleInit()
{
	Servo_Init();
	Led_Init();
}

// Bật hoặc tắt ngắt nhấp nháy LED theo trạng thái chuyển động.
void Led_Control()
{
	// Bật nhấp nháy khi barrier hoạt động và chưa bước vào thời gian chờ.
	if (wait_time == 0 && ModeStatusCfg.apptype == RUN_MODE)
	{
		if (DeviceStatusCfg.LedSignal == OFF)
		{
			// Bắt đầu ngắt timer khi servo chuyển động.
			LedTimerInterruptEnable();
			DeviceStatusCfg.LedSignal = ON;
			ModeStatusCfg.IsLcdModeUpdate = true;
		}
	}
	// Tắt nhấp nháy khi đang chờ hoặc barrier đã đóng.
	else if (wait_time > 0 || DeviceStatusCfg.barrierSignal == CLOSING)
	{
		if (DeviceStatusCfg.LedSignal == ON)
		{
			// Dừng ngắt timer khi servo ngừng chuyển động.
			LedTimerInterruptDisable();
			DeviceStatusCfg.LedSignal = OFF;
			ModeStatusCfg.IsLcdModeUpdate = true;
		}
	}
}

// Điều khiển servo mở barrier, chờ xe qua và đóng barrier.
void Barrier_Control()
{
	// Lưu trạng thái hành trình servo giữa các lần gọi.
	static ServoStatus_e st;
	// Thực hiện hành trình mở khi barrier đang đóng.
	if (DeviceStatusCfg.barrierSignal == CLOSING)
	{
		// Bắt đầu tính thời gian chờ khi servo hoàn tất hành trình mở.
		if (st == FULL_RANGE)
		{
			DeviceStatusCfg.barrierSignal = OPENING;
			wait_time = getTick();
		}
		else
		{
			// Tiếp tục tăng góc servo để mở barrier.
			st = Increase_ServoPos();
		}
	}
	// Bắt đầu đóng barrier sau khi hết thời gian chờ.
	if (DeviceStatusCfg.barrierSignal == OPENING && getTick() - wait_time > 3000)
	{
		if (wait_time > 0)
		{
			// Xóa mốc chờ trước khi bắt đầu hành trình đóng.
			wait_time = 0;
		}
		// Chuyển về chế độ chờ khi servo hoàn tất hành trình đóng.
		if (st == FULL_RANGE)
		{
			DeviceStatusCfg.barrierSignal = CLOSING;
			ModeStatusCfg.apptype = IDLE_MODE;
		}
		else
		{
			// Tiếp tục giảm góc servo để đóng barrier.
			st = Decrease_ServoPos();
		}
	}
}