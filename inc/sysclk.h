#ifndef SYSCLK_H
#define SYSCLK_H

#include "stm32f10x.h"   /* device registers and type definitions */
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_flash.h"
/* PCLK source selection for Get_Timer_Clock_Freq() */
#define APB1_CLK   0U
#define APB2_CLK   1U
/* 
    * Configure the system clock 
*/
void SystemClockConfigure();
/* 
    * Initialize MCO1 and MCO2 pins (PA8 and PC9) to output the system clock (SYSCLK).
    * This can be used for debugging or as a clock source for external devices.
    * Note: The actual frequency on the pin will depend on the system clock configuration.
*/
void MCO_OutputPin_Init();
/*
 * Return the current APB1 peripheral clock frequency (PCLK1) in Hz.
 * The value is computed from SystemCoreClock and the APB1 prescaler.
 */
uint32_t Get_PCLK1(void);

/*
 * Return the current APB2 peripheral clock frequency (PCLK2) in Hz.
 */
uint32_t Get_PCLK2(void);

/*
 * Determine the timer input clock frequency for a given TIMx instance.
 * Timers on APB1/2 run at PCLK*2 when the corresponding prescaler is
 * greater than or equal to 2 (i.e. prescaler code >=4).
 *
 * Parameters:
 *   TIMx         - pointer to the timer peripheral (unused in this simple
 *                  implementation, but kept for API compatibility).
 *   PCLK_SOURCE  - one of APB1_CLK or APB2_CLK to select which bus.
 */
uint32_t Get_Timer_Clock_Freq(TIM_TypeDef *TIMx);

#endif /* SYSCLK_H */
