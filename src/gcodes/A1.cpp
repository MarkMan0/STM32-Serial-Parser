#include "gcode_parser.h"
#include "main.h"

void GcodeParser::A1() {
  pins::led.write(1);
}
