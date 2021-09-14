#ifndef I2C_H_
#define I2C_H_

/**
 * @file i2c.h
 * @brief I2C wrapper
 *
 */

#include "main.h"
#include "cmsis_os.h"
#include "semphr.h"
#include "utils.h"
/**
 * @brief I2C peripheral wrapper
 *
 */
class I2C {
public:
  I2C_HandleTypeDef hi2c1_;

  utils::SimpleLock get_lock() {
    return utils::SimpleLock{ mutex_ };
  }

  /**
   * @brief Init the I2C peripheral and pins
   *
   */
  void init_peripheral();

  /**
   * @brief Inits mutex and if needed other parts that need the OS
   *
   */
  void init_os();

  [[nodiscard]] bool write(uint8_t address, uint8_t* data, size_t len);
  [[nodiscard]] bool read(uint8_t address, uint8_t* data, size_t len);
  [[nodiscard]] bool write_register(uint8_t address, uint8_t reg_addr, uint8_t* data, size_t len);
  [[nodiscard]] bool read_register(uint8_t address, uint8_t reg_addr, uint8_t* data, size_t len);

private:
  SemaphoreHandle_t mutex_;
};

extern I2C i2c;

#endif
