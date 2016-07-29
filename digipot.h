#ifndef DIGIPOT_H
#define DIGIPOT_H

#include <Arduino.h>

namespace digipot{
  void write(byte address, byte value);
  void init();
  void setVoltage(byte driver, byte current);
  void setMotorCurrent(uint8_t current); //Set Digipot Motor Current
}
#endif
