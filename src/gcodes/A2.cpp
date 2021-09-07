#include "gcode_parser.h"
#include "main.h"
#include "uart.h"

#include "DS3231/DS3231.h"

/**
 * @brief Gcode A2 sets the time from the given parameters
 *
 * @details
 * Parameters:
 * **S**: seconds
 * **M**: minutes
 * **H**: hours
 * **W**: day of week
 * **D**: date
 * **O**: month
 * **Y**: year
 */
void GcodeParser::A2() {
  DS3231::time t{ 0 };

  if (!rtc.get_time(t)) {
    uart2.printf("Couldn't get time");
    return;
  }

  t.am_pm = DS3231::AM_PM_UNUSED;
  int16_t dest{ 0 };

  if (parser_.get_parameter('S', dest)) {
    t.seconds = dest;
  }
  if (parser_.get_parameter('M', dest)) {
    t.minutes = dest;
  }
  if (parser_.get_parameter('H', dest)) {
    t.hours = dest;
  }
  if (parser_.get_parameter('W', dest)) {
    t.day = dest;
  }
  if (parser_.get_parameter('D', dest)) {
    t.date = dest;
  }
  if (parser_.get_parameter('O', dest)) {
    t.month = dest;
  }
  if (parser_.get_parameter('Y', dest)) {
    t.year = dest;
  }

  if (rtc.set_time(t)) {
    uart2.printf("Time set!");
  } else {
    uart2.printf("Time set failed!");
  }
}
