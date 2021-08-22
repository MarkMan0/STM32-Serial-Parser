#ifndef MAIN_H_
#define MAIN_H_

/** @file main.h
 * main.h file
 *
 */

#ifdef __cplusplus
extern "C" {
#endif
#include <stm32f3xx_hal.h>
#include "pin_api.h"

/**
 * @brief Called when initialization fails for peripherials
 *
 */
void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif  // MAIN_H_
