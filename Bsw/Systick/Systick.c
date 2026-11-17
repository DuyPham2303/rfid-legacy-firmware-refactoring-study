// Khai báo các hàm timer và cấu hình chân liên quan.
#include "Systick.h"
#include "PinCfg.h"

// Bộ đếm tick hệ thống và hai timer đếm ngược dùng chung.
volatile uint32_t uwtick;
volatile uint16_t Timer1, Timer2;

// Hàm giảm timer phụ và tăng tick hệ thống.
static void SD_Timer_Handler(void);
static void SPL_IncTick(void);

// Cấu hình SysTick 1 ms và timer phần cứng tạo delay.
void SysTick_Init(void)
{
	// Tạo ngắt mỗi 1 ms từ xung nhịp lõi.
	SysTick_Config(SystemCoreClock / 1000);

	// Khai báo cấu hình bộ đếm thời gian delay.
	TIM_TimeBaseInitTypeDef delay_timerInit;

	// Cấu hình timer đếm lên với tần số đếm 10 kHz.
	delay_timerInit.TIM_ClockDivision = TIM_CKD_DIV1;
	delay_timerInit.TIM_Prescaler = 7200 - 1;
	delay_timerInit.TIM_CounterMode = TIM_CounterMode_Up;
	delay_timerInit.TIM_Period = 0xffff;
	// Áp dụng cấu hình cho timer tạo delay.
	TIM_TimeBaseInit(DELAY_TIM_SOURCE, &delay_timerInit);

	// Bật timer để hàm delay_ms có thể đọc bộ đếm.
	TIM_Cmd(DELAY_TIM_SOURCE, ENABLE);
}

// Chờ đồng bộ bằng cách polling bộ đếm timer, đơn vị mili giây.
void delay_ms(uint32_t time)
{
	// Bắt đầu đo thời gian chờ từ 0.
	TIM_SetCounter(DELAY_TIM_SOURCE, 0);
	// Mỗi mili giây tương ứng 10 xung đếm ở cấu hình hiện tại.
	while (TIM_GetCounter(DELAY_TIM_SOURCE) < time * 10)
		;
}

// Hàm phục vụ ngắt SysTick, chạy mỗi 1 ms.
void SysTick_Handler(void)
{
	// Mẫu gọi handler FatFs theo chu kỳ 10 ms, hiện đang tắt.
	/*
	static uint8_t FatFsCnt = 0;
	FatFsCnt++;
	if(FatFsCnt >= 10){
			FatFsCnt = 0;
			SD_Timer_Handler();
	}
	*/
	// Cập nhật các bộ đếm phụ và tick hệ thống.
	SD_Timer_Handler();
	SPL_IncTick();
}

// Trả về số mili giây đã trôi qua kể từ khi khởi tạo.
uint32_t getTick()
{
	return uwtick;
}

// Giảm các timer phụ nếu chúng đang đếm.
static void SD_Timer_Handler(void)
{
	if (Timer1 > 0)
		Timer1--;
	if (Timer2 > 0)
		Timer2--;
}

// Tăng tick hệ thống sau mỗi ngắt SysTick.
static void SPL_IncTick(void)
{
	uwtick++;
}
