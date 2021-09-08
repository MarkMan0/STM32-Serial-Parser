/**
 * @file GFX.cpp
 * @brief GFX class implementation
 *
 */

#include "GFX.h"

#include <cstring>
#include "utils.h"
#include "uart.h"

std::pair<uint8_t, uint8_t> GFX::get_page_and_mask(uint8_t row) const {
  std::pair<uint8_t, uint8_t> ret{ 7 - (row / 8), (1 << (7 - row % 8)) };
  return ret;
}


void GFX::set_pixel(const Pixel& pix, bool val) {
  auto [page, mask] = get_page_and_mask(pix.y_);
  if (val) {
    canvas_[pix.x_][page] |= mask;
  } else {
    canvas_[pix.x_][page] &= ~mask;
  }
}


void GFX::reset_pixel(const Pixel& pix) {
  set_pixel(pix, false);
}


bool GFX::get_pixel(const Pixel& pix) {
  auto [page, mask] = get_page_and_mask(pix.y_);
  return (canvas_[pix.x_][page] & mask);
}

void GFX::toggle_pixel(const Pixel& pix) {
  set_pixel(pix, !get_pixel(pix));
}

void GFX::draw() {
  if (draw_fcn_) {
    draw_fcn_(canvas_);
  }
}


void GFX::clear_canvas() {
  // we can use memset instead of a double for
  memset(canvas_.data(), 0, 128 * 8);
}



void GFX::draw_circle(const Pixel& pix, uint8_t radius, bool val) {
  // create a slightly larger box around the circle
  static constexpr uint8_t margin = 2;
  const Pixel start{ static_cast<uint8_t>(std::max(0, pix.x_ - radius - margin)),
                     static_cast<uint8_t>(std::max(0, pix.y_ - radius - margin)) },
      end{ static_cast<uint8_t>(std::min(127, pix.x_ + radius + margin)),
           static_cast<uint8_t>(std::min(63, pix.y_ + radius + margin)) };

  // check each pixel in the bo, if it is inside the circle
  // not the most efficient way
  for (Pixel curr{ start }; curr.x_ < end.x_; ++curr.x_) {
    for (curr.y_ = start.y_; curr.y_ < end.y_; ++curr.y_) {
      if (pix.distance(curr) <= radius) {
        set_pixel(curr, val);
      }
    }
  }
}
