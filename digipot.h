#ifndef DIGIPOT_H
#define DIGIPOT_H

#include <Arduino.h>

#ifdef BOARD_RAMBO

namespace digipot{
  void write(byte address, byte value);
  void init();
  void setVoltage(byte driver, byte current);
}
#endif

#endif
