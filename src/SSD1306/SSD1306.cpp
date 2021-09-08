/**
 * @file SSD1306.cpp
 * @brief SSD1306 class implementation
 *
 */

#include "SSD1306/SSD1306.h"
#include "SSD1306/commands.h"

#include "i2c.h"
#include "uart.h"


bool SSD1306::begin() {
  // set mux
  namespace reg = SSD_1306_reg;
  constexpr uint8_t control_byte{ 0 };
  uint8_t buff[3];
  bool res{ true };

  auto write = [&res, &buff](size_t len) { res = res && i2c.write(addr_, buff, len); };

  buff[0] = control_byte;

  // Turn the display off for config
  buff[1] = reg::SET_DISPLAY_OFF;
  write(2);

  // Display height - 1
  buff[1] = reg::SET_MUX_RATIO;
  buff[2] = 63;
  write(3);

  // display vertical shift
  buff[1] = reg::SET_DISPLAY_OFFSET;
  buff[2] = 0;
  write(3);

  // display start line is 0
  buff[1] = reg::SET_DISPLAY_START_LINE;
  write(2);

  // segment remap 1 - flip in X
  buff[1] = reg::SET_SEGMENT_REMAP | 0x1;
  write(2);

  // normal COM scan
  buff[1] = reg::SET_COM_OUTPUT_SCAN_DIR;
  write(2);

  // COM pins hardware layout, change bits
  // bit 4: com pin config
  // bit 5: left/right remap
  buff[1] = reg::SET_COM_HW_CONFIG;
  buff[2] = 0x2 | (0x1 << 4) | (0x0 << 5);
  write(3);

  // set display contrast/brightness
  buff[1] = reg::SET_CONTRAST_CONTROL;
  buff[2] = 100;
  write(3);

  // use ram to display
  buff[1] = reg::ENTIRE_DISPLAY_FROM_RAM;
  write(2);

  // 1 in RAM means OLED on
  buff[1] = reg::SET_NORMAL_DISPLAY;
  write(2);

  // set oscillator from datasheet
  buff[1] = reg::SET_CLOCK_DIVIDE_RATIO;
  buff[2] = (0b1000 << 4);
  write(3);

  // enable charge pump
  buff[1] = reg::CHARGE_PUMP_SETTINGS;
  buff[2] = 0x10 | (0x1 << 2);
  write(3);

  // set addressing mode
  buff[1] = SSD_1306_reg::SET_MEMORY_ADDRESSING_MODE;
  buff[2] = 0x01;
  write(3);

  // turn on display only if everything went OK
  if (res) {
    buff[1] = reg::SET_DISPLAY_ON;
    write(2);
  }
  return res;
}

bool SSD1306::draw_canvas(GFX::canvas_t& canvas) {
  if (!reset_ram_address()) return false;
  // data is stored in a 2D std::array, which is contiguous, so we can transfer in one go
  return i2c.write_register(addr_, 0x40, reinterpret_cast<uint8_t*>(canvas.data()), 128 * 8);
}

void SSD1306::set_ram_val(uint8_t val) {
  if (!reset_ram_address()) return;
  uint8_t buff[2] = { 0x40, (val >= 0) ? 0xFF : 0 };
  for (int curr_col = 0; curr_col < 128; ++curr_col) {
    for (int curr_page = 0; curr_page < 8; ++curr_page) {
      if (!i2c.write(addr_, buff, 2)) {
        return;
      }
    }
  }
}


bool SSD1306::reset_ram_address() {
  static constexpr uint8_t buff[]{ SSD_1306_reg::SET_PAGE_ADDRESS, 0, 0x7, SSD_1306_reg::SET_COLUMN_ADDRESS, 0, 127 };
  return i2c.write_register(addr_, 0x00, const_cast<uint8_t*>(buff), sizeof(buff));
}
