#ifndef GFX_H_
#define GFX_H_

/**
 * @file GFX.h
 * @brief Graphics driver
 *
 */

#include <cstdint>
#include "utils.h"

#include <array>
#include <bitset>
#include <utility>

/**
 * @brief Graphics driver
 *
 * @details Drawing is done on an internal canvas. Canvas is transferred to the
 * display when draw() is called
 */
class GFX {
public:
  using Pixel = utils::Point<uint8_t>;  /*!< Used to get/set pixel by position */
  using canvas_t = std::array<std::array<uint8_t, 8>, 128>; /*!< canvas, where each bit is one pixel */
  using draw_fcn_t = bool (*)(canvas_t&); /*!< callback funtion type to draw the canvas*/

  /**
   * @brief Sets the pixel at the given coordinates to \p val
   *
   * @param pix Pixel
   * @param val defaults to true
   */
  void set_pixel(const Pixel& pix, bool val = true) {
    auto [page, mask] = get_page_and_mask(pix.y_);
    if (val) {
      canvas_[pix.x_][page] |= mask;
    } else {
      canvas_[pix.x_][page] &= ~mask;
    }
  }

  /**
   * @brief Resets/turn off the pixel at \p pix
   *
   * @param pix pixel position
   */
  void reset_pixel(const Pixel& pix) {
    set_pixel(pix, false);
  }

  /**
   * @brief Get the value of the pixeal at \p pix
   *
   * @param pix
   * @return true if pixel is set
   */
  bool get_pixel(const Pixel& pix) {
    auto [page, mask] = get_page_and_mask(pix.y_);
    return (canvas_[pix.x_][page] & mask);
  }

  /**
   * @brief Toggles the pixel at \p pix
   *
   * @param pix
   */
  void toggle_pixel(const Pixel& pix) {
    set_pixel(pix, !get_pixel(pix));
  }

  /**
   * @brief Transfers the whole canvas using the draw_fcn_ callback
   *
   */
  void draw() {
    if (draw_fcn_) {
      draw_fcn_(canvas_);
    }
  }

public:
  draw_fcn_t draw_fcn_{ nullptr }; /*!< Callback to transfer the canvas to the display*/
  canvas_t canvas_{ 0 };  /*!< drawing canvas */

  /**
   * @brief For a given row, returns the page number and bit mask
   *
   * @param row give in pixel coordinate
   * @return std::pair<uint8_t, uint8_t>, first is the page number, second is the mask
   */
  std::pair<uint8_t, uint8_t> get_page_and_mask(uint8_t row) const {
    std::pair<uint8_t, uint8_t> ret{ row / 8, (1 << ( 7 - row % 8) ) };
    return ret;
  }
};


#endif
