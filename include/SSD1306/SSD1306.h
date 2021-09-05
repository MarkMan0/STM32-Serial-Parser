/**
 * @file SSD1306.h
 * @brief Low level access to SSD1306 OLED display
 *
 */
#ifndef SSD_1306_H_
#define SSD_1306_H_

#include <cstdint>
#include <array>

#include "GFX.h"

/**
 * @brief SSD1306 functions
 *
 */
class SSD1306 {
public:
  /**
   * @brief Inits the registers and turns on the display
   *
   * @return success
   */
  bool begin();

  /**
   * @brief Used by the GFX class to redraw the whole display
   *
   * @return success
   */
  static bool draw_canvas(GFX::canvas_t&);

  /**
   * @brief Sets the whole ram to the given value, 0 or 1
   *
   * @param val
   */
  void set_ram_val(uint8_t val);

private:
  /**
   * @brief Reset the RAM internal pointer to 0, 0
   *
   * @return success
   */
  static bool reset_ram_address();


  static constexpr uint8_t addr_{ 0x3C << 1 }; /*!< I2C address already shifted */
};


#endif
