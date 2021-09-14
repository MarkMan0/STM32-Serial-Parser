#include "gcode_parser.h"

#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "semphr.h"
#include "os_tasks.h"
#include "uart.h"

/**
 * @file gcode_parser.cpp
 * @brief Function definitions for GcodeParser class
 *
 */



void GcodeParser::gcode_task(void* arg) {
  while (1) {
    if (xSemaphoreTake(uart2.rx_semaphore_, portMAX_DELAY)) {
      while (uart2.has_message()) {
        const bool need_ok = uart2.is_rx_full();
        const auto& msg = uart2.get_message();
        gcode.parse_and_call(msg.data());
        uart2.send_queue(msg.data());
        uart2.pop_rx();
        if (need_ok) {
          uart2.printf("ok");
        }
      }
    }
  }
}

void GcodeParser::begin() {
  gcode_task_handle_ = osThreadNew(GcodeParser::gcode_task, NULL, &kGcodeTaskAttr_);
  tasks::check_rtos_create(gcode_task_handle_, "GCODE TASK");
}

void GcodeParser::parse_and_call(const char* arr) {
  parser_.set_string(arr);
  switch (parser_.get_prefix()) {
    case 'A':
      parse_A();
      break;

    default:
      break;
  }
}

void GcodeParser::parse_A() {
  switch (parser_.get_number()) {
    case 0:
      A0();
      break;
    case 1:
      A1();
      break;
    case 2:
      A2();
      break;
    case 3:
      A3();
      break;

    default:
      break;
  }
}


GcodeParser gcode;
