#ifndef UTILS_H_
#define UTILS_H_

/**
 * @file utils.h
 * @brief Inline utility functions
 *
 */

#include "main.h"
#include <stm32f3xx_hal.h>

#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"

#include <cmath>



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
   * @brief Checks if val falls in \p range [ \p low, \p high], including \p low and \p high
   *
   * @tparam T
   * @tparam K
   * @tparam L
   * @param val
   * @param low
   * @param high
   * @return bool
   */
  template <class T, class K, class L>
  inline constexpr bool is_within(T val, K low, L high) {
    return (val >= low) && (val <= high);
  }

  /**
   * @brief Check if \p val is BETWEEN \p low and \p high , not including \p low and \p high
   *
   * @tparam T
   * @tparam K
   * @tparam L
   * @param val
   * @param low
   * @param high
   * @return bool
   */
  template <class T, class K, class L>
  inline constexpr bool is_within_strict(T val, K low, L high) {
    return (val > low) && (val < high);
  }

  /**
   * @brief Constrains val
   *
   * @tparam T
   * @param val
   * @param low
   * @param high
   * @return T
   */
  template <class T, class U>
  constexpr inline T constrain(T val, U low, U high) {
    if (val <= low) {
      return low;
    } else if (val >= high) {
      return high;
    } else {
      return val;
    }
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

  /**
   * @brief Simple point with 2 coordinates
   *
   * @tparam T
   */
  template <class T>
  class Point {
  public:
    using data_t = T;
    data_t x_{ 0 };
    data_t y_{ 0 };

    Point() = default;
    Point(data_t x, data_t y) : x_{ x }, y_{ y } {
    }
    Point(const Point& other) = default;
    Point& operator=(const Point& lhs) = default;

    template <class U>
    float distance(const Point<U>& other) const {
      return hypot(x_ - other.x_, y_ - other.y_);
    }
  };


}  // namespace utils

#endif
