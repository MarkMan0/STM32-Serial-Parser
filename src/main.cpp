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

osThreadId_t uart_send_task_handle;
const osThreadAttr_t uart_send_task_attributes = { .name = "uart_send_task",
                                                   .attr_bits = 0,
                                                   .cb_mem = nullptr,
                                                   .cb_size = 0,
                                                   .stack_mem = nullptr,
                                                   .stack_size = 128 * 4,
                                                   .priority = (osPriority_t)osPriorityNormal,
                                                   .tz_module = 0,
                                                   .reserved = 0 };
void start_uart_send_task(void*);

SemaphoreHandle_t uart_rx_sem, uart_tx_sem;

osThreadId_t gcode_task_handle;
const osThreadAttr_t gcode_task_attributes = { .name = "gcode_task",
                                               .attr_bits = 0,
                                               .cb_mem = nullptr,
                                               .cb_size = 0,
                                               .stack_mem = nullptr,
                                               .stack_size = 128 * 4,
                                               .priority = (osPriority_t)osPriorityNormal,
                                               .tz_module = 0,
                                               .reserved = 0 };
void start_gcode_task(void*);

/**
 * @brief Configures the system clock, called from main()
 *
 */
void SystemClock_Config();

int main() {
  HAL_Init();

  SystemClock_Config();

  pins::led.init();

  uart2.init();
  uart2.start_listen();

  osKernelInitialize();

  uart_rx_sem = xSemaphoreCreateBinary();
  uart_tx_sem = xSemaphoreCreateBinary();

  uart_send_task_handle = osThreadNew(start_uart_send_task, NULL, &uart_send_task_attributes);
  gcode_task_handle = osThreadNew(start_gcode_task, NULL, &gcode_task_attributes);

  osKernelStart();
}

auto osDelayMs(uint32_t ms) -> auto {
  return osDelay(pdMS_TO_TICKS(ms));
}

void start_uart_send_task(void* arg) {
  while (true) {
    if (xSemaphoreTake(uart_tx_sem, pdMS_TO_TICKS(1000))) {
      uart2.tick();
    } else {
      uart2.transmit("uart_tx timeout");
    }
  }
}

void start_gcode_task(void* arg) {
  while (true) {
    if (xSemaphoreTake(uart_rx_sem, pdMS_TO_TICKS(1000))) {
      if (uart2.has_message()) {
        const bool need_ok = uart2.is_rx_full();
        const auto& msg = uart2.get_message();
        GcodeParser::getInstance().parse_and_call(msg.data());
        uart2.send_queue(msg.data());
        uart2.pop_rx();
        if (need_ok) {
          uart2.send_queue("ok");
        }
      }
    } else {
      uart2.transmit("uart_rx timeout");
    }
  }
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
