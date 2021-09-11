/**
 * @file GFX.cpp
 * @brief GFX class implementation
 *
 */

#include "GFX.h"

#include <cstring>
#include "utils.h"
#include "uart.h"

#include "SSD1306/my_fonts.h"

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

  auto callback = [&pix, radius, val, this](const Pixel& curr) {
    if (pix.distance(curr) <= radius) {
      this->set_pixel(curr, val);
    }
  };

  pixel_iterate(start, end, callback);
}


void GFX::draw_rectangle(const Pixel& top_left, const Pixel& bottom_right, bool val) {
  auto callback = [this, val](const Pixel& curr) { this->set_pixel(curr, val); };

  pixel_iterate(top_left, bottom_right, callback);
}

/// \todo rework this mess
void GFX::render_glyph(const Pixel& pos, char c) {
  const auto x_offset = pos.x_;
  const auto page = 7 - utils::constrain(pos.y_, 0, 7);
  const auto& curr_font = my_fonts::font1;

  if (c < curr_font.offset_) {
    // cant render
    return;
  }
  const auto index = c - curr_font.offset_;
  if (index >= curr_font.num_glyphs_) {
    // cant render
    return;
  }

  /*
   * we can only set columns in the canvas, not rows
   * so we need to extract columns from the bitmap
   * bitmap data is in binary, in the format:
   * u8, u8, u8
   * u8, u8, u8
   * with 8 rows
   */
  for (int col = 7; col >= 0; --col) {
    // construct columns in reverse to display correctly
    uint8_t column_val{ 0 };

    for (int glyph_row = 0; glyph_row < 8; ++glyph_row) {
      // get the index for the row of this char
      auto ind = index + glyph_row * curr_font.num_glyphs_;
      auto a = curr_font.font_[ind];
      // extract the bit, and place it into the row
      column_val |= (!!(a & (uint8_t)(1 << col))) << (7 - glyph_row);
    }
    // finally write the column
    canvas_[7 - col + x_offset][page] = column_val;
  }
}


void GFX::draw_text(const Pixel& pos, const char* txt) {
  const auto len = strlen(txt);

  auto curr_page = pos.y_;
  auto curr_x = pos.x_;
  const auto& curr_font = my_fonts::font1;
  const auto increment = curr_font.width;

  bool state{ true };

  auto render_one = [&curr_x, &curr_page, &state, this](const char c) {
    if (!state) {
      return;
    }
    render_glyph({ curr_x, curr_page }, c);
    curr_x += increment;
    if (curr_x >= 127 - increment) {
      // next char won't fit
      curr_x = 0;
      curr_page++;
      if (curr_page > 7) {
        // screen is full
        curr_page = 0;
        state = false;
        return;
      }
    }
    return;
  };

  for (unsigned int i = 0; i < len; ++i) {
    if (!state) {
      return;
    }
    switch (txt[i]) {
      case '\r':
        curr_x = 0;
        break;
      case '\n':
        curr_x = 0;
        curr_page++;
        if (curr_page > 7) {
          return;
        }
        break;
      case '\t':
        // render tab as 2 spaces
        render_one(' ');
        render_one(' ');
        break;
      default:
        render_one(txt[i]);
    }
  }
}
