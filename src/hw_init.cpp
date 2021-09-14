/**
 * @file hw_init.cpp
 * @brief Hardware init definitions
 *
 */

#include "hw_init.h"

#include "main.h"
#include "utils.h"
#include "uart.h"

TIM_HandleTypeDef htim7;

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
  RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
  RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;

  utils::hal_wrap(HAL_RCC_OscConfig(&RCC_OscInitStruct));

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  utils::hal_wrap(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2));

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12 | RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV8;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;

  utils::hal_wrap(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit));
}





HAL_StatusTypeDef init_TIM7(uint32_t TickPriority) {
  RCC_ClkInitTypeDef clkconfig;
  uint32_t uwTimclock = 0;
  uint32_t uwPrescalerValue = 0;
  uint32_t pFLatency;

  HAL_NVIC_SetPriority(TIM7_DAC2_IRQn, TickPriority, 0);
  HAL_NVIC_EnableIRQ(TIM7_DAC2_IRQn);

  __HAL_RCC_TIM7_CLK_ENABLE();

  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

  uwTimclock = HAL_RCC_GetPCLK1Freq();

  uwPrescalerValue = (uint32_t)((uwTimclock / 1000000) - 1);

  htim7.Instance = TIM7;

  htim7.Init.Period = (1000000 / 1000) - 1;
  htim7.Init.Prescaler = uwPrescalerValue;
  htim7.Init.ClockDivision = 0;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  if (HAL_TIM_Base_Init(&htim7) == HAL_OK) {
    return HAL_TIM_Base_Start_IT(&htim7);
  }

  /* Return function status */
  return HAL_ERROR;
}

/**
 * @section HAL-helper HAL helper API
 *
 */

/**
 * @brief Initializes the clocks, called by HAL in the background
 *
 */
void HAL_MspInit() {
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}


/**
 * @brief Initializes TIM7 as HAL tick source
 *
 * @param TickPriority
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority) {
  return init_TIM7(TickPriority);
}

/**
 * @brief Pause HAL tick
 *
 */
void HAL_SuspendTick(void) {
  __HAL_TIM_DISABLE_IT(&htim7, TIM_IT_UPDATE);
}

/**
 * @brief Resume HAL tick
 *
 */
void HAL_ResumeTick(void) {
  __HAL_TIM_ENABLE_IT(&htim7, TIM_IT_UPDATE);
}


/**
 * @brief Called on any timer period elapsed ISR
 *
 * @param htim
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
  if (htim->Instance == TIM7) {
    HAL_IncTick();
    uart2.on_systick_ISR();
  }
}
