/** @file main.cpp
 * File containing int main()
 */

#include "main.h"
#include "uart.h"
#include "gcode_parser.h"
#include "pin_api.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "semphr.h"


osThreadId_t periodic_send_task_handle;
const osThreadAttr_t periodic_send_attr = { .name = "periodic_send_task",
                                            .attr_bits = 0,
                                            .cb_mem = nullptr,
                                            .cb_size = 0,
                                            .stack_mem = nullptr,
                                            .stack_size = 128 * 4,
                                            .priority = (osPriority_t)osPriorityBelowNormal1,
                                            .tz_module = 0,
                                            .reserved = 0 };  //!< send task attributes

void periodic_send_task(void* arg) {
  while(1) {
    uart2.send_queue("Hello");
    osDelay(pdMS_TO_TICKS(5000));
  }
}

/**
 * @brief Configures the system clock, called from main()
 *
 */
void SystemClock_Config();

int main() {
  HAL_Init();

  SystemClock_Config();

  pins::led.init();

  uart2.init_peripherals();

  osKernelInitialize();

  uart2.begin();
  gcode.begin();

  periodic_send_task_handle = osThreadNew(periodic_send_task, NULL, &periodic_send_attr);

  osKernelStart();
}


void Error_Handler() {
  while (1) {
  }
}

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
  RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief Initializes the clocks, called by HAL in the background
 *
 */
void HAL_MspInit() {
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}
