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
  using Pixel = utils::Point<uint8_t>;                      /*!< Used to get/set pixel by position */
  using canvas_t = std::array<std::array<uint8_t, 8>, 128>; /*!< canvas, where each bit is one pixel */
  using draw_fcn_t = bool (*)(canvas_t&);                   /*!< callback funtion type to draw the canvas*/

  /**
   * @brief Sets the pixel at the given coordinates to \p val
   *
   * @param pix Pixel
   * @param val defaults to true
   */
  void set_pixel(const Pixel& pix, bool val = true);

  /**
   * @brief Resets/turn off the pixel at \p pix
   *
   * @param pix pixel position
   */
  void reset_pixel(const Pixel& pix);

  /**
   * @brief Get the value of the pixeal at \p pix
   *
   * @param pix
   * @return true if pixel is set
   */
  bool get_pixel(const Pixel& pix);

  /**
   * @brief Toggles the pixel at \p pix
   *
   * @param pix
   */
  void toggle_pixel(const Pixel& pix);

  /**
   * @brief Set the canvas to all 0
   *
   */
  void clear_canvas();

  /**
   * @brief Draw a circle around \p pix with \p radius . Sets the values in canvas to \p val
   *
   * @param pix
   * @param radius
   * @param val
   */
  void draw_circle(const Pixel& pix, uint8_t radius, bool val = true);


  /**
   * @brief Draw a recatngle filled with \p val
   *
   * @param top_left
   * @param bottom_right
   * @param val
   */
  void draw_rectangle(const Pixel& top_left, const Pixel& bottom_right, bool val = true);

  /**
   * @brief Transfers the whole canvas using the draw_fcn_ callback
   *
   */
  void draw();

public:
  draw_fcn_t draw_fcn_{ nullptr }; /*!< Callback to transfer the canvas to the display*/
  canvas_t canvas_{ 0 };           /*!< drawing canvas */

  /**
   * @brief For a given row, returns the page number and bit mask
   *
   * @param row give in pixel coordinate
   * @return std::pair<uint8_t, uint8_t>, first is the page number, second is the mask
   */
  std::pair<uint8_t, uint8_t> get_page_and_mask(uint8_t row) const;


  /**
   * @brief Iterates through pixels from \p from to \p to
   * @details IMPORTANT: \p from should be LESS than \p to
   *
   * @tparam LAMBDA type of lambda, this allows the lambda to have a capture list
   * @param from
   * @param to
   * @param callback callback, can be a simple function pointer or a lambda
   */
  template <class LAMBDA>
  void pixel_iterate(const Pixel& from, const Pixel& to, LAMBDA& callback) {
    for (Pixel curr{ from }; curr.x_ < to.x_; ++curr.x_) {
      for (curr.y_ = from.y_; curr.y_ < to.y_; ++curr.y_) {
        callback(curr);
      }
    }
  }
};


#endif
