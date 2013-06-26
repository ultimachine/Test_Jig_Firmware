#include <Arduino.h>
#include <SPI.h>


#define DIGIPOTSS_PIN 38
#define DIGIPOT_CHANNELS {4,5,3,0,1}

inline void digipotWrite(int address, int value) // From Arduino DigitalPotControl example
{
#if DIGIPOTSS_PIN > -1
  digitalWrite(DIGIPOTSS_PIN,LOW); // take the SS pin low to select the chip
;
  SPI.transfer(value);
  digitalWrite(DIGIPOTSS_PIN,HIGH); // take the SS pin high to de-select the chip:
#endif
}

inline void digipotSetVoltage(uint8_t driver, int current)
{
#if DIGIPOTSS_PIN > -1
  const uint8_t digipot_ch[] = DIGIPOT_CHANNELS;
  digipotWrite(digipot_ch[driver], current);
#endif
}

void digipotInit() //Initialize Digipot Motor Current
{
#if DIGIPOTSS_PIN > -1
  const uint8_t digipot_motor_current[] = {135,135,135,135,135};

  SPI.begin();
  pinMode(DIGIPOTSS_PIN, OUTPUT);
  for(int i=0; i<=4; i++)
    //digitalPotWrite(digipot_ch[i], digipot_motor_current[i]);
    digipotSetVoltage(i,digipot_motor_current[i]);
#endif
}

