#ifndef __STM32F3xx_IT_H
#define __STM32F3xx_IT_H

/** @file stm32f3xx_it.h
 * Interrupt prototypes
 * To get these prototypes, it's easiest to STM32CubeIDE
 */

#include "main.h"
#ifdef __cplusplus
extern "C" {
#endif

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void DMA1_Channel6_IRQHandler(void);
void DMA1_Channel7_IRQHandler(void);
void USART2_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif  // __STM32F3xx_IT_H
