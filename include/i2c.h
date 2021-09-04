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
  bool write(uint8_t address, uint8_t* data, size_t len);
  bool read(uint8_t address, uint8_t* data, size_t len);
  bool write_register(uint8_t address, uint8_t reg_addr, uint8_t* data, size_t len);
  bool read_register(uint8_t address, uint8_t reg_addr, uint8_t* data, size_t len);
};

extern I2C i2c;

#endif
