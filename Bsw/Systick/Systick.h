#ifndef SYSTICK_H
#define SYSTICK_H
#include <stdint.h>
void SysTick_Init(void);
uint32_t getTick(void);
void delay_ms(uint32_t time);
#endif