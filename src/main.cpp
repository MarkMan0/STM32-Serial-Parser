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
#include "SSD1306/SSD1306.h"
#include "hw_init.h"

#include "DS3231/DS3231.h"
#include "GFX.h"


#include "os_tasks.h"


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

  i2c.init_os();

  uart2.begin();
  gcode.begin();

  tasks::start_display_task();
  tasks::start_monitor_task();

  osKernelStart();
}


void Error_Handler() {
  while (1) {
  }
}
