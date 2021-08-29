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
#include "utils.h"

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
  const osThreadAttr_t kGcodeTaskAttr_ = utils::create_thread_attr("gcode_task", 128*4, osPriorityNormal);

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
