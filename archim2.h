#pragma once

#include "pins.h"

#ifdef BOARD_ARCHIM2

#define MainSerial SerialUSB

#define SDHSMCI_SUPPORT
#define SPIFLASH_SUPPORT

#define X_STEP_PIN         40 //PC8 STEP1
#define X_DIR_PIN          59 //PA4 DIR1 (Analog Input 5)
#define X_ENABLE_PIN       41 //PC9 EN1
#define X_MIN_PIN          14 //PD4 MIN ES1
#define X_MAX_PIN          32 //PD10 MAX ES1

#define Y_STEP_PIN         49 //PC14 STEP2
#define Y_DIR_PIN          47 //PC16 DIR2
#define Y_ENABLE_PIN       48 //PC15 EN2
#define Y_MIN_PIN          29 //PD6 MIN ES2
#define Y_MAX_PIN          15 //PD5 MAX ES2

#define Z_STEP_PIN         36 //PC4 STEP Z
#define Z_DIR_PIN         107 //PB10 DIR Z
#define Z_ENABLE_PIN       96 //PC10 EN Z -AddOns
#define Z_MIN_PIN          31 //PA7 MIN ES3
#define Z_MAX_PIN          30 //PD9 MAX ES3

#define E0_STEP_PIN        78 //PB23 STEP3
#define E0_DIR_PIN         22 //PB26 DIR3
#define E0_ENABLE_PIN      97 //PB24 EN3 -Addons

#define E1_STEP_PIN        26 //PD1 STEP4 
#define E1_DIR_PIN         27 //PD2 DIR4
#define E1_ENABLE_PIN      28 //PD3 EN4

#define SPIFLASH_CS 86 //77  // Chip Select PIN

namespace archim2{
  void init();
  void portStep();
  void portDirection(byte dir);
  void portEnable(byte en);
  void portSetMicroSteps(byte ms);
  void sdinit();
  void setMotorCurrent(byte x);
}

void spiflash_init();
uint8_t spiflash_read_byte(long address);
void spiflash_write_byte(long address, uint8_t value);

void tmc2130_write(uint8_t chipselect, uint8_t address,uint8_t wval1,uint8_t wval2,uint8_t wval3,uint8_t wval4);

void spiflash_write(byte data);

#endif
