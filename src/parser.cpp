#include "parser.h"
#include "main.h"
#include <cstring>
#include <algorithm>

void Parser::set_string(const char* str) {
  str_ = str;
  str_sz_ = strlen(str);
}

void Parser::reset() {
  str_ = nullptr;
  str_sz_ = 0;
}

char Parser::get_prefix() const {
  if (has_string()) {
    return str_[get_first_not_whitespace()];
  } else {
    return 0;
  }
}

uint16_t Parser::get_number() const {
  if (!has_string()) return 0;

  int start = get_first_not_whitespace() + 1;
  return atoi(str_ + start);
}

bool Parser::get_parameter(char param, int16_t& dest, int16_t def) const {
  dest = def;
  if (!has_string()) return false;
  const auto end = str_ + str_sz_;  // create crude end iterator
  const auto place = std::find(str_, end, param);
  if (place == end) return false;

  if (*(place + 1) == '\0' || *(place + 1) < '0' || *(place + 1) > '9') {
    // parameter has no value
    return true;
  }

  dest = atoi(place + 1);  // conver to int
  return true;
}

bool Parser::has_string() const {
  return str_ != nullptr;
}

size_t Parser::get_first_not_whitespace() const {
  if (!has_string()) return 0;

  int ind = 0;
  while (str_[ind] != '\0' && (str_[ind] == ' ' || str_[ind] == '\n' || str_[ind] == '\r')) {
    ++ind;
  }
  return ind;
}
