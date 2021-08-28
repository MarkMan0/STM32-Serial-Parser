#ifndef UTILS_H_
#define UTILS_H_

#include "main.h"
#include <stm32f3xx_hal.h>


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

}  // namespace utils

#endif
