/**
 * @file parser.h
 * @brief Parser class
 *
 */
#ifndef PARSER_H_
#define PARSER_H_

#include <array>

#include "main.h"

/**
 * @brief Class to parse commands into arguments
 *
 */
class Parser {
public:
  /**
   * @brief Set the message to be parsed
   *
   * @param arr null-terminated string
   */
  void set_string(const char* arr);

  /**
   * @brief reset parser
   *
   */
  void reset();
  /**
   * @brief Get the command prefix
   *
   * @return char the prefix
   */
  char get_prefix() const;
  /**
   * @brief Get the command number
   *
   * @return uint16_t
   */
  uint16_t get_number() const;
  /**
   * @brief Get the value of a parameter or default
   * @details Retruns true if the parameter is found, even without value
   * @param param Parameter to search
   * @param dest where to copy the value
   * @param def default value
   * @return true if parameter was found
   */
  bool get_parameter(char param, int16_t& dest, int16_t def = 0) const;

  /**
   * @brief Retruns true if the parser has a string it can work with
   *
   * @return true if parser string is set
   */
  bool has_string() const;

private:
  /**
   * @brief Index of first non-whitespace char
   *
   * @return size_t
   */
  size_t get_first_not_whitespace() const;
  /**
   * @brief Work string
   *
   */
  const char* str_{ nullptr };
  /**
   * @brief size of work string
   *
   */
  uint16_t str_sz_{ 0 };
};

#endif
