#ifndef PinCfg_H
#define PinCfg_H

/*
 * @file config.h
 * @brief Configuration definitions for STM32 peripheral initialization.
 *
 * This header contains hardware pin mappings, peripheral selection macros,
 * timer settings, and function declarations used by the device configuration
 * layer.
 */
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

/* Port and Pin configurationc similar for SD card and RC522 module */
#define SPI_PORT GPIOA
#define HSPI1 SPI1
#define SCK_PIN GPIO_Pin_5
#define MISO_PIN GPIO_Pin_6
#define MOSI_PIN GPIO_Pin_7

/* for sd card only */
#define SD_CS_PIN GPIO_Pin_4

/*for rc522 only*/
#define RC522_RST_PORT GPIOB
#define RC522_RST_PIN GPIO_Pin_0
#define RC522_CS_PIN GPIO_Pin_4

/* LCD I2C Pin & Port */
#define HI2C1 I2C1
#define I2C_PORT GPIOB
#define LCD_SCL_PIN GPIO_Pin_6
#define LCD_SDA_PIN GPIO_Pin_7

/* LED GPIO Pin & Port */
#define LED_PIN GPIO_Pin_13
#define LED_PORT GPIOC

/* USART Clock RCC for debug purpose */
#define TX_PIN GPIO_Pin_9
#define RX_PIN GPIO_Pin_10
#define USART_SOURCE USART1
#define USART_PORT GPIOA

/*
 *Timer and PWM configuration for servo
 */
#define SERVO_TIM_SOURCE TIM2
#define SERVO_PIN GPIO_Pin_1
#define SERVO_CHANNEL TIM_OCMode_PWM1
#define SERVO_PORT GPIOA

/* Timer configuration for delay function */
#define DELAY_TIM_SOURCE TIM3

/* Timer interrupt configuration for LED blinking */
#define LED_TIM_SOURCE TIM1
#define LED_PIN GPIO_Pin_13
#define LED_PORT GPIOC
#define TIM_IRQn TIM1_UP_IRQn

#endif
