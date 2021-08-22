#include "gcode_parser.h"
#include "main.h"

void GcodeParser::A0() {
  pins::led.write(0);
}
