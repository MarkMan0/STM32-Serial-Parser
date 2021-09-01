#include "gcode_parser.h"
#include "main.h"
#include "uart.h"

#include "DS3231/DS3231.h"

void GcodeParser::A3() {
  DS3231::time t;
  if (rtc.get_time(t)) {
    DS3231::report_time(t);
  } else {
    uart2.send_queue("Failed to get time");
  }
}
