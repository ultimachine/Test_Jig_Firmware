#include <Arduino.h>
#ifndef DIGIPOT_H
#define DIGIPOT_H

void digipotWrite(int address, int value);

void digipotInit();

void digipotSetVoltage(uint8_t driver, int current);

#endif
