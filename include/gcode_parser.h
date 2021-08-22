#ifndef GCODES_H_
#define GCODES_H_

/**
 * @file gcode_parser.h
 * @brief File containing class to convert messages into gcodes and call the appropriate functions
 *
 */

#include "main.h"
#include <array>
#include "parser.h"

/**
 * @brief Class to call gcodes parsed from null terminated string
 *
 */
class GcodeParser {
public:
  /**
   * @brief Get Gcodes singleton instance
   *
   * @return Gcodes&
   */
  static GcodeParser& getInstance() {
    static GcodeParser instance;
    return instance;
  }

  /**
   * @brief Parses the argument and calls the gcode
   *
   * @param arr Null terminated array
   */
  void parse_and_call(const char* arr);

  /** @name Gcode Implementations
   *
   * Gcodes
   */
  ///@{
  void A0(); /*!< Turns off LED*/
  void A1(); /*!< Turns on LED*/
  ///@}

private:
  GcodeParser() = default; /*!< Private constructor */
  Parser parser_;          /*!< Parser */

  /**
   * @brief Parse gcodes beginning with 'A'
   *
   */
  void parse_A();

public:
  GcodeParser(const GcodeParser&) = delete;
  void operator=(const GcodeParser&) = delete;
};



#endif
