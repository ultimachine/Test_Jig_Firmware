#include <Arduino.h>
#include "pins.h"
#include "einsyrambo.h"
#include <SPI.h>

#ifdef BOARD_EINSYRAMBO

#define MainSerial Serial
#define MYSERIAL Serial

uint8_t cs[4] = { X_TMC2130_CS, Y_TMC2130_CS, Z_TMC2130_CS, E0_TMC2130_CS };

void tmc2130_write(uint8_t chipselect, uint8_t address,uint8_t wval1,uint8_t wval2,uint8_t wval3,uint8_t wval4)
{
  uint32_t val32;
  uint8_t val0;
  uint8_t val1;
  uint8_t val2;
  uint8_t val3;
  uint8_t val4;

  //datagram1 - write
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));
  digitalWrite(chipselect,LOW);
  SPI.transfer(address+0x80);
  SPI.transfer(wval1);
  SPI.transfer(wval2);
  SPI.transfer(wval3);
  SPI.transfer(wval4);
  digitalWrite(chipselect, HIGH);
  SPI.endTransaction();

  //datagram2 - response
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));
  digitalWrite(chipselect,LOW);
  val0 = SPI.transfer(0);
  val1 = SPI.transfer(0);
  val2 = SPI.transfer(0);
  val3 = SPI.transfer(0);
  val4 = SPI.transfer(0);
  digitalWrite(chipselect, HIGH);
  SPI.endTransaction();

/*
  MYSERIAL.print("WriteRead 0x");
  MYSERIAL.print(address,HEX);
  MYSERIAL.print(" Status:");
  MYSERIAL.print(val0 & 0b00000111,BIN);
  MYSERIAL.print("  ");
  MYSERIAL.print(val1,BIN);
  MYSERIAL.print("  ");
  MYSERIAL.print(val2,BIN);
  MYSERIAL.print("  ");
  MYSERIAL.print(val3,BIN);
  MYSERIAL.print("  ");
  MYSERIAL.print(val4,BIN);

  val32 = (uint32_t)val1<<24 | (uint32_t)val2<<16 | (uint32_t)val3<<8 | (uint32_t)val4;

  MYSERIAL.print(" 0x");
  MYSERIAL.println(val32,HEX);
*/
}

void tmc2130_chopconf(uint8_t cs, bool extrapolate256 = 1, uint16_t microstep_resolution = 16)
{
  uint8_t mres=0b0100;
  if(microstep_resolution == 256) mres = 0b0000;
  if(microstep_resolution == 128) mres = 0b0001;
  if(microstep_resolution == 64)  mres = 0b0010;
  if(microstep_resolution == 32)  mres = 0b0011;
  if(microstep_resolution == 16)  mres = 0b0100;
  if(microstep_resolution == 8)   mres = 0b0101;
  if(microstep_resolution == 4)   mres = 0b0110;
  if(microstep_resolution == 2)   mres = 0b0111;
  if(microstep_resolution == 1)   mres = 0b1000;

  mres |= extrapolate256 << 4; //bit28 intpol

  tmc2130_write(cs,0x6C,mres,1,00,0xC5);
}

void tmc2130_init()
{
  uint8_t current[4] = { 31, 31, 31, 31 };

  digitalWrite(X_TMC2130_CS, HIGH);
  digitalWrite(Y_TMC2130_CS, HIGH);
  digitalWrite(Z_TMC2130_CS, HIGH);
  digitalWrite(E0_TMC2130_CS, HIGH);
  pinMode(X_TMC2130_CS,OUTPUT);
  pinMode(Y_TMC2130_CS,OUTPUT);
  pinMode(Z_TMC2130_CS,OUTPUT);
  pinMode(E0_TMC2130_CS,OUTPUT);

  SPI.begin();

  for(int i=0;i<4;i++)
  {
    //tmc2130_write(cs[i],0x6C,0b10100,01,00,0xC5);
    tmc2130_chopconf(cs[i],1,16);
    tmc2130_write(cs[i],0x10,0,15,current[i],current[i]); //0x10 IHOLD_IRUN
    tmc2130_write(cs[i],0x0,0,0,0,1); //address=0x0 GCONF EXT VREF
    //tmc2130_write(cs[i],0x11,0,0,0,0xA);
    tmc2130_write(cs[i],0x70,0,0b111,0x01,0xC8); //address=0x70 PWM_CONF //reset default=0x00050480
  }
}

void einsyrambo::setMotorCurrent(uint8_t x)
{
//PWM vref
  TCCR5B = (TCCR5B & ~(_BV(CS50) | _BV(CS51) | _BV(CS52))) | _BV(CS50);
  analogWrite(46, x);
  analogWrite(45, x);
  analogWrite(44, x);
}

void einsyrambo::init(){
  einsyrambo::portEnable(0);

  //set outputs
  DDRA = B11110000; //enable X-PA7, Y-PA6, Z-PA5, E0-PA4, E1-PA3
  DDRL = B01000111; //direction E1-PL7, E0-PL6, Z-PL2, X-PL1, Y-PL0
  DDRC = B00001111; //step E1-PC4, E0-PC3, Z-PC2, Y-PC1, X-PC0

  einsyrambo::setMotorCurrent(255);

  tmc2130_init();

  pinMode(X_MIN_PIN,INPUT_PULLUP);
  pinMode(Y_MIN_PIN,INPUT_PULLUP);
  pinMode(Z_MIN_PIN,INPUT_PULLUP);

  pinMode(X_MAX_PIN,INPUT_PULLUP);
  pinMode(Y_MAX_PIN,INPUT_PULLUP);
  pinMode(Z_MAX_PIN,INPUT_PULLUP);
}

void einsyrambo::portStep(){
  PORTC |= B00001111; //pin mask high
  delayMicroseconds(2);
  PORTC &= ~(B00001111); //pin mask low
  return;
}

void einsyrambo::portDirection(byte dir){
  if(dir)PORTL |=  B01000111;  //pin mask high
  else PORTL &= ~(B01000111); //pin mask low
  return;
}

void einsyrambo::portEnable(byte en){
  if(en)PORTA &= ~(B11110000); //pin mask low - enable a4982
  else PORTA |= B11110000; //pin mask high - disable a4982
  return;
}

void einsyrambo::portSetMicroSteps(byte ms){
  for(int i=0;i<4;i++)
  {
    tmc2130_chopconf(cs[i],0,ms);
    tmc2130_write(cs[i],0x10,0,15,23,23); //0x10 IHOLD_IRUN
  }
  delay(20);
}

void einsyrambo::sdinit() {}
void spiflash_init() {}

#endif
