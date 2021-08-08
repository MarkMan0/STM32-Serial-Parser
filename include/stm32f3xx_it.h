#ifndef __STM32F3xx_IT_H
#define __STM32F3xx_IT_H
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
void DMA1_Channel7_IRQHandler(void);
void USART2_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif  // __STM32F3xx_IT_H