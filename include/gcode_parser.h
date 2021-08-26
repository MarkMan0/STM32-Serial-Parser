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
#include "FreeRTOS.h"
#include "cmsis_os.h"

/**
 * @brief Class to call gcodes parsed from null terminated string
 *
 */
class GcodeParser {
public:
  /**
   * @brief Constuctor, but only 1 instance is needed
   *
   */
  GcodeParser() = default; /*!< Private constructor */

  /**
   * @brief Start the RTOS task
   *
   */
  void begin();

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
  Parser parser_;                  /*!< Parser */
  osThreadId_t gcode_task_handle_; /*!< gcode task handle*/
  static constexpr osThreadAttr_t kGcodeTaskAttr_ = { .name = "gcode_task",
                                                      .attr_bits = 0,
                                                      .cb_mem = nullptr,
                                                      .cb_size = 0,
                                                      .stack_mem = nullptr,
                                                      .stack_size = 128 * 4,
                                                      .priority = (osPriority_t)osPriorityNormal,
                                                      .tz_module = 0,
                                                      .reserved = 0 }; /*!< gcode task attributes*/

  /**
   * @brief The gcode task, which calls gcodes
   *
   */
  static void gcode_task(void*);

  /**
   * @brief Parse gcodes beginning with 'A'
   *
   */
  void parse_A();

public:
  GcodeParser(const GcodeParser&) = delete;
  void operator=(const GcodeParser&) = delete;
};


extern GcodeParser gcode;
#endif
