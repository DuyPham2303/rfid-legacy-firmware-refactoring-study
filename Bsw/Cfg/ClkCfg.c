#include "ClkCfg.h"
#include "stm32f10x.h"
/* Clock for I/O Ports*/
#define PORT_A_RCC RCC_APB2Periph_GPIOA
#define PORT_B_RCC RCC_APB2Periph_GPIOB
#define PORT_C_RCC RCC_APB2Periph_GPIOC

/* Clock For Peripherals/ */
#define SD_RC522_SPI_RCC RCC_APB2Periph_SPI1
#define LCD_I2C_RCC RCC_APB1Periph_I2C1
#define DEBUG_USART_RCC RCC_APB2Periph_USART1
#define LED_TIM_RCC RCC_APB2Periph_TIM1
#define SERVO_TIM_RCC RCC_APB1Periph_TIM2
#define DELAY_TIM_RCC RCC_APB1Periph_TIM3

void RCC_Cfg()
{
    RCC_APB1PeriphClockCmd(LCD_I2C_RCC | SERVO_TIM_RCC | DELAY_TIM_RCC, ENABLE);
    RCC_APB2PeriphClockCmd(PORT_A_RCC | PORT_B_RCC | PORT_C_RCC |
                               SD_RC522_SPI_RCC | DEBUG_USART_RCC | LED_TIM_RCC,
                           ENABLE);
}
