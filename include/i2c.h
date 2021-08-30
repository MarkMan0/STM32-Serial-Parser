#ifndef I2C_H_
#define I2C_H_

/**
 * @file i2c.h
 * @brief I2C wrapper
 *
 */

#include "main.h"

/**
 * @brief I2C peripheral wrapper
 *
 */
class I2C {
public:
  I2C_HandleTypeDef hi2c1_;
  /**
   * @brief Init the I2C peripheral and pins
   *
   */
  void init_peripheral();
};

extern I2C i2c;

#endif
