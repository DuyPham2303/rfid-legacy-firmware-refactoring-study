// Khai báo các hàm điều khiển đầu ra.
#include "control_output.h"
#include "servo.h"
#include "AppTypes.h"
#include "Systick.h"

#define BARRIER_OPEN_WAIT_MS 3000U

// kiểu mô tả hành vi cụ thể của barrier
typedef enum
{
	BARRIER_MOVING_TO_OPEN,
	BARRIER_MOVING_TO_CLOSED
} BarrierMovement_e;

static BarrierMovement_e barrierMovement; // lưu hướng di chuyển cụ thể của barrier
static uint32_t barrierOpenedAtTick;	  // đo thời gian chờ của barrier sau khi mở
DeviceStatusCfg_s DeviceStatusCfg;		  // Lưu trạng thái của barrier và led

// Khởi tạo servo, LED và trạng thái ban đầu.
void SignalOutput_ModuleInit()
{
	Servo_Init();
	Led_Init();
	DeviceStatusCfg.barrierSignal = CLOSED;
	DeviceStatusCfg.LedSignal = OFF;
}

// Nhấp nháy LED khi barrier đang di chuyển.
void Led_Control()
{

	if (DeviceStatusCfg.barrierSignal == RUNNING &&
		ModeStatusCfg.apptype == RUN_MODE)
	{
		if (DeviceStatusCfg.LedSignal == OFF)
		{
			LedTimerInterruptEnable(); // kích hoạt led nhấp nháy
			DeviceStatusCfg.LedSignal = BLINKING;
			ModeStatusCfg.IsLcdModeUpdate = true;
		}
	}
	else if (DeviceStatusCfg.LedSignal == BLINKING)
	{
		LedTimerInterruptDisable(); // dừng nhấp nháy led
		DeviceStatusCfg.LedSignal = OFF;
		ModeStatusCfg.IsLcdModeUpdate = true;
	}
}

// Cập nhật servo theo hướng di chuyển hiện tại.
static void updateBarrierMovement(ServoStatus_e *servoStatus)
{
	// tác vụ mở barrier
	if (barrierMovement == BARRIER_MOVING_TO_OPEN)
	{
		if (*servoStatus == MAX_RANGE)
		{
			DeviceStatusCfg.barrierSignal = OPENED;
			barrierOpenedAtTick = getTick(); // bắt đầu tính thời gian chờ trước khi đóng barrier
		}
		else
		{
			*servoStatus = Increase_ServoPos();
		}
	}
	// tác vụ đóng barrier
	else
	{
		if (*servoStatus == MIN_RANGE)
		{
			DeviceStatusCfg.barrierSignal = CLOSED;
			ModeStatusCfg.apptype = IDLE_MODE;
		}
		else
		{
			*servoStatus = Decrease_ServoPos();
		}
	}
}

// Mở barrier, chờ xe qua rồi đóng barrier.
void Barrier_Control()
{
	static ServoStatus_e servoStatus;

	// Update trạng thái và hành vi tương ứng khi barrier đang đóng
	if (DeviceStatusCfg.barrierSignal == CLOSED)
	{
		barrierMovement = BARRIER_MOVING_TO_OPEN;
		DeviceStatusCfg.barrierSignal = RUNNING;
	}
	// Update trạng thái và hành vi tương ứng khi barrier đang mở
	else if (DeviceStatusCfg.barrierSignal == OPENED &&
			 getTick() - barrierOpenedAtTick > BARRIER_OPEN_WAIT_MS)
	{
		barrierMovement = BARRIER_MOVING_TO_CLOSED;
		DeviceStatusCfg.barrierSignal = RUNNING;
	}

	// xử lý nghiệp vụ đóng / mở  barrier dựa trên trạng thái hiện tại
	if (DeviceStatusCfg.barrierSignal == RUNNING)
	{
		updateBarrierMovement(&servoStatus);
		delay_ms(250);
	}
}
