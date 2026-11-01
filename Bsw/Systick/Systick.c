#include "Systick.h"
#include "PinCfg.h"
volatile uint32_t uwtick;
volatile uint16_t Timer1, Timer2;

static void SD_Timer_Handler(void);
static void SPL_IncTick(void);

void SysTick_Init(void)
{
	// tao ngat 1ms
	SysTick_Config(SystemCoreClock / 1000);

	// khai bao cau hinh luu tru timer delay
	TIM_TimeBaseInitTypeDef delay_timerInit;

	// cau hinh tham so timer3
	delay_timerInit.TIM_ClockDivision = TIM_CKD_DIV1;
	delay_timerInit.TIM_Prescaler = 7200 - 1;
	delay_timerInit.TIM_CounterMode = TIM_CounterMode_Up;
	delay_timerInit.TIM_Period = 0xffff;
	TIM_TimeBaseInit(DELAY_TIM_SOURCE, &delay_timerInit);

	// kich hoat timer 3
	TIM_Cmd(DELAY_TIM_SOURCE, ENABLE);
}

void delay_ms(uint32_t time)
{
	TIM_SetCounter(DELAY_TIM_SOURCE, 0);
	while (TIM_GetCounter(DELAY_TIM_SOURCE) < time * 10)
		;
}

void SysTick_Handler(void)
{
	/*
	static uint8_t FatFsCnt = 0;
	FatFsCnt++;
	if(FatFsCnt >= 10){
			FatFsCnt = 0;
			SD_Timer_Handler();
	}
	*/
	SD_Timer_Handler();
	SPL_IncTick();
}

uint32_t getTick()
{
	return uwtick;
}

static void SD_Timer_Handler(void)
{
	if (Timer1 > 0)
		Timer1--;
	if (Timer2 > 0)
		Timer2--;
}
static void SPL_IncTick(void)
{
	uwtick++;
}
