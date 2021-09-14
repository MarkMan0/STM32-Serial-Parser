#ifndef OS_TASKS_H_
#define OS_TASKS_H_


#include "FreeRTOS.h"
#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief FreeRTOS task helper functions
 *
 */
namespace tasks {

  /**
   * @brief Checks if RTOS object was created
   *
   * @param t pointer to RTOS object
   */
  void check_rtos_create(void* t, const char* fmt);

  /**
   * @defgroup monitor_group Monitor task group
   * @{
   */
  extern osThreadId_t monitor_task_handle;
  void monitor_task(void* arg);
  void start_monitor_task();
  /** @} */

  /**
   * @defgroup display_group Display task group
   * @{
   */
  extern osThreadId_t display_task_handle;
  void display_task(void* arg);
  void start_display_task();
  /** @} */

};  // namespace tasks


#ifdef __cplusplus
}
#endif

#endif
