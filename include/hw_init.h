/**
 * @file hw_init.h
 * @brief Function that initialize hardware and HAL callbacks
 *
 */
#ifndef HW_INIT_H_
#define HW_INIT_H_

#include "main.h"



extern TIM_HandleTypeDef htim7;
/**
 * @brief Init TIM7 as HAL Tick source
 *
 * @param TickPriority
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef init_TIM7(uint32_t TickPriority);

/**
 * @brief Configures the system clock, called from main()
 *
 */
void SystemClock_Config();

#endif
