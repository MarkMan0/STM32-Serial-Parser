#include "gcode_parser.h"
#include "main.h"
#include "pin_api.h"

void GcodeParser::A0() {
  pins::led.write(0);
}
