#ifndef ADC_H_
#define ADC_H_

/**
 * @file adc.h
 * @brief Contains a (poorly written) wrapper for the ADC peripheral
 *
 */

#include "main.h"
#include <stm32f3xx_hal_adc.h>
#include "utils.h"

/**
 * @brief Wrapper for ADC peripehral
 *
 */
class ADC {
public:
  ADC_HandleTypeDef hadc1_; /*!< HAL handle to the ADC */

  /**
   * @brief Initializes the ADC1 peripherial
   *
   */
  void init_adc();

  /**
   * @brief Starts the ADC
   *
   */
  void start_adc();

  /**
   * @brief Stops the ADC
   *
   */
  void stop_adc();

  /**
   * @brief return the ADC data register value
   *
   * @return uint32_t ADC->DR
   */
  uint32_t read();

  /**
   * @brief Returns the voltage on the ADC pin
   *
   * @return float
   */
  float read_volt();
};

extern ADC adc1;

#endif
