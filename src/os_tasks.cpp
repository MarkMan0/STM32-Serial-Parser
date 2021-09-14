#include "os_tasks.h"

#include "utils.h"
#include "uart.h"
#include "SSD1306/SSD1306.h"
#include "DS3231/DS3231.h"


void tasks::check_rtos_create(void* t, const char* fmt) {
  static int call_cnt{ 0 };
  if (t == NULL) {
    constexpr size_t buff_sz{ 50 };
    static char buff[buff_sz];
    snprintf(buff, buff_sz - 1, fmt, call_cnt);
    uart2.transmit(buff);
    HAL_Delay(1000);
    Error_Handler();
  }
  ++call_cnt;
};

osThreadId_t tasks::monitor_task_handle; /*!< handle for monitor task */
/**
 * @brief Memory consumption monitoring
 *
 * Monitors stack of all running tasks and heap memory
 * @param arg nothing
 */
void tasks::monitor_task(void* arg) {
  osDelay(5000);  // wait for all tasks to start
  const auto num_of_tasks = uxTaskGetNumberOfTasks();

  auto statuses = static_cast<TaskStatus_t*>(pvPortMalloc(num_of_tasks * sizeof(TaskStatus_t)));

  constexpr size_t memory_low_th{ 10 };
  while (1) {
    if (auto n = uxTaskGetSystemState(statuses, num_of_tasks, NULL)) {
      for (unsigned int i = 0; i < n; ++i) {
        if (statuses[i].usStackHighWaterMark < memory_low_th) {
          uart2.printf("MEM:%s:%d", statuses[i].pcTaskName, statuses[i].usStackHighWaterMark);
        }
      }
    } else {
      uart2.printf("Couldn't get system state");
    }

    if (xPortGetFreeHeapSize() < memory_low_th) {
      uart2.printf("HEAP:%d", static_cast<int>(xPortGetFreeHeapSize()));
    }

    osDelay(pdMS_TO_TICKS(10000));
  }
}

/**
 * @brief Start monitor task
 * IMPORTANT: start as last task, as it needs to know the number of tasks running
 *
 */
void tasks::start_monitor_task() {
  const auto attr = utils::create_thread_attr("monitor", 128 * 4, osPriorityBelowNormal4);
  monitor_task_handle = osThreadNew(monitor_task, NULL, &attr);
  check_rtos_create(tasks::monitor_task_handle, "MONITOR TASK");
}


static SSD1306 oled_display; /*!< OLED display driver */
static GFX graphics;         /*!< Graphics driver */

osThreadId_t tasks::display_task_handle; /*!< Display task handle */
/**
 * @brief Transfers the canvas to the display with a set rate
 *
 * @param arg
 */
void tasks::display_task(void* arg) {
  if (!oled_display.begin()) {
    while (1) {
      osDelay(portMAX_DELAY);
    }
  }
  graphics.draw_fcn_ = SSD1306::draw_canvas;
  graphics.draw();
  DS3231::time t;
  while (1) {
    if (rtc.get_time(t)) {
      graphics.clear_canvas();
      graphics.move_cursor({ 12, 0 });
      graphics.printf("%d:%d:%d", (int)t.hours, (int)t.minutes, (int)t.seconds);
      if (t.seconds % 2) {
        graphics.draw_circle({ 5, 11 }, 4);
      }
      graphics.draw();
    }
    osDelay(pdMS_TO_TICKS(1000));
  }
}

/**
 * @brief Start the display task
 *
 */
void tasks::start_display_task() {
  const auto attr = utils::create_thread_attr("display", 128 * 4, osPriorityBelowNormal1);
  display_task_handle = osThreadNew(display_task, NULL, &attr);
  check_rtos_create(display_task_handle, "DISP TASK");
}
