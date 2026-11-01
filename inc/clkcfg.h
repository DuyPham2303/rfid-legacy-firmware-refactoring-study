#ifndef CLKCFG_H
#define CLKCFG_H

#include <stdint.h>

/* base address and register offsets for RCC */
#define RCC_BASE       0x40021000U
#define RCC_CR         (RCC_BASE + 0x00U)   /* Clock Control Register */
#define RCC_CFGR       (RCC_BASE + 0x04U)   /* Clock Configuration Register */
#define RCC_APB2ENR    (RCC_BASE + 0x18U)   /* APB2 Peripheral Clock Enable Register */

/* public API */
void MCO_Pin_Config(void);
uint32_t HSE_Setup(void);
uint32_t HSI_Setup(void);
uint32_t Sysclk_Pclk(uint8_t Input, uint32_t pres);

#endif /* CLKCFG_H */