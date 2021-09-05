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
#include "utils.h"
#include "adc.h"
#include "i2c.h"

#include "DS3231/DS3231.h"


osThreadId_t i2c_task_handle;
const osThreadAttr_t i2c_task_attr = utils::create_thread_attr("i2c", 128 * 4, osPriorityBelowNormal1);

DS3231 ds;
void i2c_task(void* arg) {
  while (1) {
    osDelay(pdMS_TO_TICKS(2000));
  }
}



osThreadId_t monitor_task_handle; /*!< handle for monitor task */
const osThreadAttr_t monitor_task_attr =
    utils::create_thread_attr("monitor", 128 * 4, osPriorityBelowNormal4); /*!< monitor task attributes */
/**
 * @brief Memory consumption monitoring
 *
 * Monitors stack of all running tasks and heap memory
 * IMPORTANT: start as last task, as it needs to know the number of tasks running
 * @param arg nothing
 */
void monitor_task(void* arg) {
  const auto num_of_tasks = uxTaskGetNumberOfTasks();

  auto statuses = static_cast<TaskStatus_t*>(pvPortMalloc(num_of_tasks * sizeof(TaskStatus_t)));
  constexpr size_t buff_sz{ 30 };
  static char buff[buff_sz];

  constexpr size_t memory_low_th{ 10 };
  while (1) {
    if (auto n = uxTaskGetSystemState(statuses, num_of_tasks, NULL)) {
      for (unsigned int i = 0; i < n; ++i) {
        if (statuses[i].usStackHighWaterMark < memory_low_th) {
          snprintf(buff, buff_sz, "MEM:%s:%d", statuses[i].pcTaskName, statuses[i].usStackHighWaterMark);
          uart2.send_queue(buff);
        }
      }
    } else {
      uart2.send_queue("Couldn't get system state");
    }

    if (xPortGetFreeHeapSize() < memory_low_th) {
      snprintf(buff, buff_sz - 1, "HEAP:%d", static_cast<int>(xPortGetFreeHeapSize()));
      uart2.send_queue(buff);
    }

    osDelay(pdMS_TO_TICKS(10000));
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
  pins::A1.init();
  pins::A0.init();

  uart2.init_peripherals();
  adc1.init_adc();
  i2c.init_peripheral();
  osKernelInitialize();

  uart2.begin();
  gcode.begin();

  i2c_task_handle = osThreadNew(i2c_task, NULL, &i2c_task_attr);


  monitor_task_handle = osThreadNew(monitor_task, NULL, &monitor_task_attr);
  osKernelStart();
}


void Error_Handler() {
  while (1) {
  }
}

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

/**
 * @brief Initializes the clocks, called by HAL in the background
 *
 */
void HAL_MspInit() {
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}
