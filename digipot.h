#ifndef DIGIPOT_H
#define DIGIPOT_H

void digipotWrite(int address, int value);

void digipotInit();

void digipotSetVoltage(int driver, int current);

#endif
