#include "Servo.h"
#include "PinCfg.h"
#include "misc.h"

/* Servo pulse widths for -90, 0, and +90 degree positions. */
#define PULSE_NEG90_ANGLE 500
#define PULSE_0_ANGLE 1500
#define PULSE_POS90_ANGLE 2500
/* Servo timer prescaler and period settings. */
#define prescaler_servo 71
#define SERVO_PERIOD 20000

uint8_t angle;

void TIM1_UP_IRQHandler()
{
	if (TIM_GetITStatus(LED_TIM_SOURCE, TIM_IT_Update) != RESET)
	{
		GPIOC->ODR ^= GPIO_Pin_13;
	}
	TIM_ClearITPendingBit(LED_TIM_SOURCE, TIM_IT_Update);
}
uint16_t servo_duty = PULSE_NEG90_ANGLE;
static int convertPulseToAngle(int pulse, int pulse_min, int pulse_max)
{
	return (pulse - pulse_min) * 180 / (pulse_max - pulse_min);
}
void Servo_Init()
{
	// cau hinh gpio pin
	GPIO_InitTypeDef GPIO_ServoInit;
	GPIO_ServoInit.GPIO_Pin = SERVO_PIN;
	GPIO_ServoInit.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_ServoInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SERVO_PORT, &GPIO_ServoInit);

	// cau hinh tham so pwm
	TIM_TimeBaseInitTypeDef servo_timerInit;

	// Cau hinh timer2 de tao xung pwm -> su dung cho file servo.c
	servo_timerInit.TIM_ClockDivision = TIM_CKD_DIV1;
	servo_timerInit.TIM_Prescaler = prescaler_servo;
	servo_timerInit.TIM_Period = SERVO_PERIOD - 1;
	servo_timerInit.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(SERVO_TIM_SOURCE, &servo_timerInit);

	// cai dat cau hinh channel 2 pwm
	TIM_OCInitTypeDef servo_pwmInit;
	servo_pwmInit.TIM_OCMode = TIM_OCMode_PWM1;
	servo_pwmInit.TIM_OutputState = TIM_OutputState_Enable;
	servo_pwmInit.TIM_Pulse = PULSE_NEG90_ANGLE;
	servo_pwmInit.TIM_OCPolarity = TIM_OCPolarity_High;

	// cai dat cau hinh pwm cho channel 2
	TIM_OC2Init(SERVO_TIM_SOURCE, &servo_pwmInit);
	// tu dong dat lai counter period de kiem tra duty cycle
	TIM_OC2PolarityConfig(SERVO_TIM_SOURCE, TIM_OCPreload_Enable);

	// cho phep timer2 hoat dong
	TIM_Cmd(SERVO_TIM_SOURCE, ENABLE);
}
void Led_Init()
{
	// cau hinh bo quan ly ngat long
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitTypeDef NVIC_ledTimerInit;
	NVIC_ledTimerInit.NVIC_IRQChannel = TIM_IRQn;
	NVIC_ledTimerInit.NVIC_IRQChannelSubPriority = 0;
	NVIC_ledTimerInit.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_ledTimerInit.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_ledTimerInit);

	// Cai dat GPIO Pin cho led
	GPIO_InitTypeDef Led_PinInit;
	Led_PinInit.GPIO_Pin = LED_PIN;
	Led_PinInit.GPIO_Mode = GPIO_Mode_Out_PP;
	Led_PinInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &Led_PinInit);

	// Cau hinh timer1 de nhay led
	TIM_TimeBaseInitTypeDef led_timerInit;

	led_timerInit.TIM_ClockDivision = TIM_CKD_DIV1;
	led_timerInit.TIM_Prescaler = 7200 - 1;
	led_timerInit.TIM_Period = 499; // 50ms
	led_timerInit.TIM_CounterMode = TIM_CounterMode_Up;
	led_timerInit.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(LED_TIM_SOURCE, &led_timerInit);

	// cho phep timer3 hoat dong
	TIM_Cmd(LED_TIM_SOURCE, ENABLE);
	TIM_ITConfig(LED_TIM_SOURCE, TIM_IT_Update, ENABLE);
}

void LedTimerInterruptEnable()
{
	TIM_ITConfig(LED_TIM_SOURCE, TIM_IT_Update, ENABLE);
}
void LedTimerInterruptDisable()
{
	TIM_ITConfig(LED_TIM_SOURCE, TIM_IT_Update, DISABLE);
}

const ServoStatus_e Increase_ServoPos()
{
	if (servo_duty < PULSE_POS90_ANGLE)
	{
		TIM_SetCompare2(SERVO_TIM_SOURCE, servo_duty);
		servo_duty += 100;
	}
	else
	{
		return FULL_RANGE;
	}
	angle = convertPulseToAngle(servo_duty, PULSE_NEG90_ANGLE, PULSE_POS90_ANGLE);
	return IN_RANGE;
}
const ServoStatus_e Decrease_ServoPos()
{
	if (servo_duty > PULSE_NEG90_ANGLE)
	{
		TIM_SetCompare2(SERVO_TIM_SOURCE, servo_duty);
		servo_duty -= 100;
	}
	else
	{
		return FULL_RANGE;
	}
	angle = convertPulseToAngle(servo_duty, PULSE_NEG90_ANGLE, PULSE_POS90_ANGLE);
	return IN_RANGE;
}
