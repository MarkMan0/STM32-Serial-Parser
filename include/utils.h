#ifndef UTILS_H_
#define UTILS_H_

#include "main.h"
#include <stm32f3xx_hal.h>

#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"



/**
 * @brief Contains utility functions, not tied to anything else
 *
 */
namespace utils {

  /**
   * @brief if the result of the HAL call is not HAL_OK, will call Error_Handler();
   *
   * @param res result of HAL call
   */
  inline void hal_wrap(HAL_StatusTypeDef res) {
    if (res != HAL_OK) {
      Error_Handler();
    }
  }

  template <class L, class R>
  inline constexpr auto max(const L l, const R r) -> auto {
    return l >= r ? l : r;
  }
  template <class L, class R>
  inline constexpr auto min(const L l, const R r) -> auto {
    return l <= r ? l : r;
  }

  /**
   * @brief varidic template MAX function, can be called with optional number of arguments, uses >=
   *
   * @tparam L type of left
   * @tparam R type of right(s)
   * @param l
   * @param r
   * @return the max value of the arguments
   */
  template <class L, class... R>
  inline constexpr auto max(const L l, const R... r) -> auto {
    return max(l, max(r...));
  }

  /**
   * @brief varidic template MIN function, can be called with optional number of arguments, uses <=
   *
   * @tparam L type of left
   * @tparam R type of right(s)
   * @param l
   * @param r
   * @return the least value of the arguments
   */
  template <class L, class... R>
  inline constexpr auto min(const L l, const R... r) -> auto {
    return min(l, min(r...));
  }

  /**
   * @brief Create a thread attr object with some default parameters
   *
   * @param name the name of the task
   * @param stack stack size og the tesk
   * @param priority task priority
   * @return osThreadAttr_t
   */
  inline osThreadAttr_t create_thread_attr(const char* name, uint32_t stack, osPriority_t priority) {
    const osThreadAttr_t attr = { .name = name,
                                  .attr_bits = 0,
                                  .cb_mem = nullptr,
                                  .cb_size = 0,
                                  .stack_mem = nullptr,
                                  .stack_size = stack,
                                  .priority = priority,
                                  .tz_module = 0,
                                  .reserved = 0 };
    return attr;
  }

}  // namespace utils

#endif
