#include <SPI.h>
#include "pins.h"
#include "rambo.h"
#include "digipot.h"

#define DIGIPOT_CHANNELS {4,5,3,0,1}

#ifdef BOARD_RAMBO

inline void digipot::write(byte address, byte value) // From Arduino DigitalPotControl example
{
  digitalWrite(DIGIPOTSS_PIN,LOW); // take the SS pin low to select the chip
  SPI.transfer(address);
  SPI.transfer(value);
  digitalWrite(DIGIPOTSS_PIN,HIGH); // take the SS pin high to de-select the chip:
}

inline void digipot::setVoltage(byte driver, byte current)
{
  byte digipot_ch[] = DIGIPOT_CHANNELS;
  write(digipot_ch[driver], current);
}

void digipot::init() //Initialize Digipot Motor Current
{
  const byte digipot_motor_current[] = {135,135,135,135,135};

  SPI.begin();
  pinMode(DIGIPOTSS_PIN, OUTPUT);
  for(byte i=0; i<=4; i++)
    setVoltage(i,digipot_motor_current[i]);
}

void digipot::setMotorCurrent(uint8_t current) //Set Digipot Motor Current
{
  SPI.begin();
  pinMode(DIGIPOTSS_PIN, OUTPUT);
  for(byte i=0; i<=4; i++)
    setVoltage(i,current);
}

#endif
