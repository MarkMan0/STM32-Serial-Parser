#include "gcode_parser.h"

/**
 * @file gcode_parser.cpp
 * @brief Function definitions for GcodeParser class
 *
 */

void GcodeParser::parse_and_call(const char* arr) {
  parser_.set_string(arr);
  switch (parser_.get_prefix()) {
    case 'A':
      parse_A();
      break;

    default:
      break;
  }
}

void GcodeParser::parse_A() {
  switch (parser_.get_number()) {
    case 0:
      A0();
      break;
    case 1:
      A1();
      break;

    default:
      break;
  }
}
