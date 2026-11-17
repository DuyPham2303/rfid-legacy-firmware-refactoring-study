#include "Servo.h"
#include "PinCfg.h"
#include "misc.h"

#define SERVO_MIN_POSITION_PULSE_US 500U
#define SERVO_CENTER_POSITION_PULSE_US 1500U
#define SERVO_MAX_POSITION_PULSE_US 2500U
#define SERVO_ANGLE_RANGE_DEGREES 180U
#define SERVO_POSITION_STEP_PULSE_US 100U
#define SERVO_INITIAL_POSITION_MARGIN_PULSE_US 200U
#define SERVO_PWM_PERIOD_US 20000U
#define SERVO_TIMER_CLOCK_DIVISOR 72U
#define TIMER_PRESCALER_REGISTER_OFFSET 1U
#define TIMER_AUTO_RELOAD_REGISTER_OFFSET 1U

#define LED_TIMER_CLOCK_DIVISOR 7200U
#define LED_BLINK_INTERVAL_MS 50U
#define TIMER_TICKS_PER_MS 10U
#define LED_IRQ_PREEMPTION_PRIORITY 0U
#define LED_IRQ_SUBPRIORITY 0U
#define TIMER_REPETITION_DISABLED 0U

uint8_t angle;
// Độ rộng xung PWM hiện tại quyết định góc servo.
static uint16_t servo_duty = SERVO_MIN_POSITION_PULSE_US;

// Đảo trạng thái LED mỗi lần timer phát sinh ngắt cập nhật.
void TIM1_UP_IRQHandler()
{
	if (TIM_GetITStatus(LED_TIM_SOURCE, TIM_IT_Update) != RESET)
	{
		GPIOC->ODR ^= GPIO_Pin_13;
	}
	TIM_ClearITPendingBit(LED_TIM_SOURCE, TIM_IT_Update);
}

// Quy đổi độ rộng xung trong dải servo thành góc từ 0 đến 180 độ.
static int convertPulseToAngle(int pulse, int pulse_min, int pulse_max)
{
	return (pulse - pulse_min) * SERVO_ANGLE_RANGE_DEGREES /
		   (pulse_max - pulse_min);
}

void Servo_Init()
{
	// Đặt chân servo ở chế độ ngõ ra PWM.
	GPIO_InitTypeDef GPIO_ServoInit;
	GPIO_ServoInit.GPIO_Pin = SERVO_PIN;
	GPIO_ServoInit.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_ServoInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SERVO_PORT, &GPIO_ServoInit);

	// Cấu hình chu kỳ PWM 20 ms cho servo.
	TIM_TimeBaseInitTypeDef servo_timerInit;

	servo_timerInit.TIM_ClockDivision = TIM_CKD_DIV1;
	servo_timerInit.TIM_Prescaler =
		SERVO_TIMER_CLOCK_DIVISOR - TIMER_PRESCALER_REGISTER_OFFSET;
	servo_timerInit.TIM_Period = SERVO_PWM_PERIOD_US - TIMER_AUTO_RELOAD_REGISTER_OFFSET;
	servo_timerInit.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(SERVO_TIM_SOURCE, &servo_timerInit);

	// Khởi tạo kênh PWM với độ rộng xung tương ứng vị trí nhỏ nhất.
	TIM_OCInitTypeDef servo_pwmInit;
	servo_pwmInit.TIM_OCMode = TIM_OCMode_PWM1;
	servo_pwmInit.TIM_OutputState = TIM_OutputState_Enable;
	servo_pwmInit.TIM_Pulse = servo_duty;
	servo_pwmInit.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC2Init(SERVO_TIM_SOURCE, &servo_pwmInit);
	// Cho phép nạp trước giá trị so sánh PWM.
	TIM_OC2PolarityConfig(SERVO_TIM_SOURCE, TIM_OCPreload_Enable);

	// Bật timer tạo PWM cho servo.
	TIM_Cmd(SERVO_TIM_SOURCE, ENABLE);
}

// Khởi tạo chân LED, ngắt timer và chu kỳ nhấp nháy.
void Led_Init()
{
	// Cấu hình mức ưu tiên và bật ngắt timer cho LED.
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitTypeDef NVIC_ledTimerInit;
	NVIC_ledTimerInit.NVIC_IRQChannel = TIM_IRQn;
	NVIC_ledTimerInit.NVIC_IRQChannelSubPriority = LED_IRQ_SUBPRIORITY;
	NVIC_ledTimerInit.NVIC_IRQChannelPreemptionPriority = LED_IRQ_PREEMPTION_PRIORITY;
	NVIC_ledTimerInit.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_ledTimerInit);

	// Đặt chân LED làm ngõ ra.
	GPIO_InitTypeDef Led_PinInit;
	Led_PinInit.GPIO_Pin = LED_PIN;
	Led_PinInit.GPIO_Mode = GPIO_Mode_Out_PP;
	Led_PinInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &Led_PinInit);

	// Cấu hình timer tạo ngắt định kỳ mỗi 50 ms.
	TIM_TimeBaseInitTypeDef led_timerInit;

	led_timerInit.TIM_ClockDivision = TIM_CKD_DIV1;
	led_timerInit.TIM_Prescaler = LED_TIMER_CLOCK_DIVISOR - TIMER_PRESCALER_REGISTER_OFFSET;
	led_timerInit.TIM_Period = (LED_BLINK_INTERVAL_MS * TIMER_TICKS_PER_MS) - TIMER_AUTO_RELOAD_REGISTER_OFFSET;
	led_timerInit.TIM_CounterMode = TIM_CounterMode_Up;
	led_timerInit.TIM_RepetitionCounter = TIMER_REPETITION_DISABLED;
	TIM_TimeBaseInit(LED_TIM_SOURCE, &led_timerInit);

	// Bật timer
	TIM_Cmd(LED_TIM_SOURCE, ENABLE);
}

// Cho phép ngắt timer để LED bắt đầu nhấp nháy.
void LedTimerInterruptEnable()
{
	TIM_ITConfig(LED_TIM_SOURCE, TIM_IT_Update, ENABLE);
}

// Tắt ngắt timer để LED ngừng nhấp nháy.
void LedTimerInterruptDisable()
{
	TIM_ITConfig(LED_TIM_SOURCE, TIM_IT_Update, DISABLE);
}

// Tăng dần độ rộng xung để xoay servo về phía vị trí lớn nhất.
const ServoStatus_e Increase_ServoPos()
{
	if (servo_duty < SERVO_MAX_POSITION_PULSE_US)
	{
		TIM_SetCompare2(SERVO_TIM_SOURCE, servo_duty);
		servo_duty += SERVO_POSITION_STEP_PULSE_US;
	}
	else
	{
		// Báo đã chạm giới hạn trên.
		return MAX_RANGE;
	}
	angle = convertPulseToAngle(servo_duty,
								SERVO_MIN_POSITION_PULSE_US,
								SERVO_MAX_POSITION_PULSE_US);
	return IN_RANGE;
}

// Giảm dần độ rộng xung để xoay servo về phía vị trí nhỏ nhất.
const ServoStatus_e Decrease_ServoPos()
{
	if (servo_duty > SERVO_MIN_POSITION_PULSE_US)
	{
		TIM_SetCompare2(SERVO_TIM_SOURCE, servo_duty);
		servo_duty -= SERVO_POSITION_STEP_PULSE_US;
	}
	else
	{
		// Báo đã chạm giới hạn dưới.
		return MIN_RANGE;
	}
	angle = convertPulseToAngle(servo_duty,
								SERVO_MIN_POSITION_PULSE_US,
								SERVO_MAX_POSITION_PULSE_US);
	return IN_RANGE;
}
