/**
 * @file i2c.cpp
 * @brief I2C wrapper implementation
 *
 */

#include "i2c.h"

#include "main.h"
#include "utils.h"
#include "pin_api.h"
#include "os_tasks.h"

/**
 * @brief Called by HAL in HAL_I2C_Init()
 *
 * @param hi2c
 */
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c) {
  if (hi2c->Instance == I2C1) {
    __HAL_RCC_I2C1_CLK_ENABLE();
    pins::SDA.init();
    pins::SCL.init();
  }
}


void I2C::init_peripheral() {
  constexpr I2C_InitTypeDef init{ .Timing = 0x0000020B,
                                  .OwnAddress1 = 0,
                                  .AddressingMode = I2C_ADDRESSINGMODE_7BIT,
                                  .DualAddressMode = I2C_DUALADDRESS_DISABLE,
                                  .OwnAddress2 = 0,
                                  .OwnAddress2Masks = I2C_OA2_NOMASK,
                                  .GeneralCallMode = I2C_GENERALCALL_DISABLE,
                                  .NoStretchMode = I2C_NOSTRETCH_DISABLE };

  hi2c1_.Instance = I2C1;
  hi2c1_.Init = init;

  utils::hal_wrap(HAL_I2C_Init(&hi2c1_));
  utils::hal_wrap(HAL_I2CEx_ConfigAnalogFilter(&hi2c1_, I2C_ANALOGFILTER_ENABLE));
  utils::hal_wrap(HAL_I2CEx_ConfigDigitalFilter(&hi2c1_, 0x00));
};

void I2C::init_os() {
  mutex_ = xSemaphoreCreateMutex();
  tasks::check_rtos_create(mutex_, "I2CMutex");
}

static constexpr uint32_t timeout = 1000;

bool I2C::write(uint8_t address, uint8_t* data, size_t len) {
  return HAL_I2C_Master_Transmit(&hi2c1_, address, data, len, timeout) == HAL_OK;
}

bool I2C::read(uint8_t address, uint8_t* data, size_t len) {
  return HAL_I2C_Master_Receive(&hi2c1_, address, data, len, timeout) == HAL_OK;
}

bool I2C::write_register(uint8_t address, uint8_t reg_addr, uint8_t* data, size_t len) {
  return HAL_I2C_Mem_Write(&hi2c1_, address, reg_addr, 1, data, len, timeout) == HAL_OK;
}


bool I2C::read_register(uint8_t address, uint8_t reg_addr, uint8_t* data, size_t len) {
  return HAL_I2C_Mem_Read(&hi2c1_, address, reg_addr, 1, data, len, timeout) == HAL_OK;
}


I2C i2c;
