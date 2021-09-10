#ifndef MY_FONTS_H_
#define MY_FONTS_H_

#include <cstdint>

/**
 * @brief Define new fonts here, see README on how-to
 *
 */
namespace my_fonts {

  /**
   * @brief Press Start K font bitmap
   * @details Author: Codeman38, Font can be found at https://www.dafont.com/press-start.font
   */
  constexpr uint8_t font1_data[] = {
    0x00, 0x18, 0x6c, 0x00, 0x18, 0x62, 0x38, 0x18, 0x0c, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x38, 0x18, 0x7c,
    0x7e, 0x1c, 0xfc, 0x3c, 0xfe, 0x78, 0x7c, 0x00, 0x00, 0x0c, 0x00, 0x30, 0x7c, 0x7c, 0x38, 0xfc, 0x3c, 0xf8, 0xfe,
    0xfe, 0x3e, 0xc6, 0x7e, 0x06, 0xc6, 0xc0, 0xc6, 0xc6, 0x7c, 0xfc, 0x7c, 0xfc, 0x78, 0x7e, 0xc6, 0xc6, 0xc6, 0xc6,
    0x66, 0xfe, 0x3c, 0x40, 0x3c, 0x10, 0x00, 0x60, 0x00, 0xc0, 0x00, 0x06, 0x00, 0x1c, 0x00, 0xc0, 0x18, 0x18, 0xc0,
    0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x18, 0x38, 0x00,
    0x00, 0x3c, 0x6c, 0x6c, 0x3e, 0x66, 0x6c, 0x18, 0x18, 0x18, 0x6c, 0x18, 0x00, 0x00, 0x00, 0x06, 0x4c, 0x38, 0xc6,
    0x0c, 0x3c, 0xc0, 0x60, 0xc6, 0x84, 0xc6, 0x00, 0x00, 0x18, 0x00, 0x18, 0xc6, 0xce, 0x6c, 0xc6, 0x66, 0xcc, 0xc0,
    0xc0, 0x60, 0xc6, 0x18, 0x06, 0xcc, 0xc0, 0xee, 0xe6, 0xc6, 0xc6, 0xc6, 0xc6, 0xcc, 0x18, 0xc6, 0xc6, 0xc6, 0xee,
    0x66, 0x0e, 0x30, 0x60, 0x0c, 0x28, 0x00, 0x30, 0x00, 0xc0, 0x00, 0x06, 0x00, 0x36, 0x00, 0xc0, 0x00, 0x00, 0xc0,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x18, 0x0c, 0x00,
    0x00, 0x3c, 0x6c, 0xfe, 0x60, 0x0c, 0x38, 0x18, 0x30, 0x0c, 0x38, 0x18, 0x00, 0x00, 0x00, 0x0c, 0xc6, 0x18, 0x0e,
    0x18, 0x6c, 0xfc, 0xc0, 0x0c, 0xc4, 0xc6, 0x18, 0x18, 0x30, 0x3c, 0x0c, 0x06, 0xd2, 0xc6, 0xc6, 0xc0, 0xc6, 0xc0,
    0xc0, 0xc0, 0xc6, 0x18, 0x06, 0xd8, 0xc0, 0xfe, 0xf6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc0, 0x18, 0xc6, 0xc6, 0xd6, 0x7c,
    0x66, 0x1c, 0x30, 0x30, 0x0c, 0x44, 0x00, 0x00, 0x7e, 0xfc, 0x7c, 0x7e, 0x7c, 0x30, 0x7e, 0xfc, 0x38, 0x38, 0xcc,
    0x18, 0xec, 0xfc, 0x7c, 0xfc, 0x7e, 0xfc, 0x7c, 0xfc, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xfe, 0x30, 0x18, 0x0c, 0x60,
    0x00, 0x3c, 0x00, 0x6c, 0x3c, 0x18, 0x76, 0x00, 0x30, 0x0c, 0xfe, 0x7e, 0x00, 0x7e, 0x00, 0x18, 0xc6, 0x18, 0x3c,
    0x3c, 0xcc, 0x06, 0xfc, 0x18, 0x78, 0x7e, 0x18, 0x18, 0x60, 0x00, 0x06, 0x1c, 0xd2, 0xc6, 0xfc, 0xc0, 0xc6, 0xfc,
    0xfc, 0xce, 0xfe, 0x18, 0x06, 0xf0, 0xc0, 0xfe, 0xfe, 0xc6, 0xc6, 0xc6, 0xce, 0x7c, 0x18, 0xc6, 0xc6, 0xfe, 0x38,
    0x3c, 0x38, 0x30, 0x18, 0x0c, 0x82, 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0xc6, 0xc6, 0x18, 0x18, 0xd8,
    0x18, 0xfe, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xe0, 0x30, 0xc6, 0xc6, 0xc6, 0x6c, 0xc6, 0x1c, 0x60, 0x18, 0x06, 0xf2,
    0x00, 0x18, 0x00, 0xfe, 0x06, 0x30, 0xdc, 0x00, 0x30, 0x0c, 0x38, 0x18, 0x00, 0x00, 0x00, 0x30, 0xc6, 0x18, 0x78,
    0x06, 0xfe, 0x06, 0xc6, 0x30, 0x9e, 0x06, 0x00, 0x00, 0x30, 0x3c, 0x0c, 0x38, 0xce, 0xfe, 0xc6, 0xc0, 0xc6, 0xc0,
    0xc0, 0xc6, 0xc6, 0x18, 0x06, 0xf8, 0xc0, 0xd6, 0xde, 0xc6, 0xfc, 0xde, 0xf8, 0x06, 0x18, 0xc6, 0x6c, 0xfe, 0x7c,
    0x18, 0x70, 0x30, 0x0c, 0x0c, 0x00, 0x00, 0x00, 0xc6, 0xc6, 0xc0, 0xc6, 0xfe, 0x30, 0xc6, 0xc6, 0x18, 0x18, 0xf0,
    0x18, 0xd6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc0, 0x7c, 0x30, 0xc6, 0x6c, 0xd6, 0x38, 0xc6, 0x38, 0x30, 0x18, 0x0c, 0x9e,
    0x00, 0x00, 0x00, 0x6c, 0x7c, 0x66, 0xcc, 0x00, 0x18, 0x18, 0x6c, 0x18, 0x18, 0x00, 0x18, 0x60, 0x64, 0x18, 0xe0,
    0xc6, 0x0c, 0xc6, 0xc6, 0x30, 0x86, 0x0c, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00, 0xc0, 0xc6, 0xc6, 0x66, 0xcc, 0xc0,
    0xc0, 0x66, 0xc6, 0x18, 0xc6, 0xdc, 0xc0, 0xc6, 0xce, 0xc6, 0xc0, 0xcc, 0xdc, 0xc6, 0x18, 0xc6, 0x38, 0x6c, 0xee,
    0x18, 0xe0, 0x30, 0x06, 0x0c, 0x00, 0x00, 0x00, 0xc6, 0xc6, 0xc0, 0xc6, 0xc0, 0x30, 0x7e, 0xc6, 0x18, 0x18, 0xd8,
    0x18, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc0, 0x0e, 0x36, 0xc6, 0x38, 0xfe, 0x6c, 0x7e, 0x70, 0x30, 0x18, 0x0c, 0x0c,
    0x00, 0x18, 0x00, 0x00, 0x18, 0x46, 0x76, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x18, 0x00, 0x18, 0x40, 0x38, 0x7e, 0xfe,
    0x7c, 0x0c, 0x7c, 0x7c, 0x30, 0x7c, 0x78, 0x18, 0x18, 0x0c, 0x00, 0x30, 0x38, 0x7e, 0xc6, 0xfc, 0x3c, 0xf8, 0xfe,
    0xc0, 0x3e, 0xc6, 0x7e, 0x7c, 0xce, 0xfe, 0xc6, 0xc6, 0x7c, 0xc0, 0x7a, 0xce, 0x7c, 0x18, 0x7c, 0x10, 0x44, 0xc6,
    0x18, 0xfe, 0x3c, 0x02, 0x3c, 0x00, 0x00, 0x00, 0x7e, 0xfc, 0x7e, 0x7e, 0x7c, 0x30, 0x06, 0xc6, 0x3c, 0x18, 0xcc,
    0x3c, 0xc6, 0xc6, 0x7c, 0xfc, 0x7e, 0xc0, 0xfc, 0x1c, 0x7e, 0x10, 0x6c, 0xc6, 0x06, 0xfe, 0x1c, 0x18, 0x38, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc0, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x18, 0x00, 0x00
  };

  /**
   * @brief Font data with pointer to the struct
   *
   */
  struct Font_t {
    const uint8_t* const font_;
    const uint8_t width;
    const uint16_t num_glyphs_;
    const uint8_t offset_;
  };

  /**
   * @brief Font 1
   *
   */
  constexpr Font_t font1{ .font_ = font1_data, .width = 8, .num_glyphs_ = sizeof(font1_data) / 8, .offset_ = 32 };

}  // namespace my_fonts


#endif
