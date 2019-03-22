#ifndef EINSYRAMBO_H
#define EINSYRAMBO_H

#include <Arduino.h>

#ifdef BOARD_EINSYRAMBO

#ifndef __AVR_ATmega2560__
  #error "Oops!  Make sure you have 'Arduino Mega 2560' or 'Rambo' selected from the 'Tools -> Boards' menu."
#endif

#define SPIFLASH_SUPPORT

#define MainSerial Serial

// EINSY RAMBO
#define X_STEP_PIN          37
#define X_DIR_PIN           49
#define X_MIN_PIN           12
#define X_MAX_PIN           30
#define X_ENABLE_PIN        29


#define Y_STEP_PIN          36
#define Y_DIR_PIN           48
#define Y_MIN_PIN           11
#define Y_MAX_PIN           24
#define Y_ENABLE_PIN        28


#define Z_STEP_PIN          35
#define Z_DIR_PIN           47
#define Z_MIN_PIN           10
#define Z_MAX_PIN           23
#define Z_ENABLE_PIN        27

#define E0_STEP_PIN         34
#define E0_DIR_PIN          43
#define E0_ENABLE_PIN       26


#define X_TMC2130_CS 41
#define Y_TMC2130_CS 39
#define Z_TMC2130_CS 67
#define E0_TMC2130_CS 66

#define SPIFLASH_CS 32 //PC5, P2 header pin1

//#define EINSY01
#ifdef EINSY01 //Also compatible with MiniRambo 1.3a
  // EINY 0.1- EINSY-0.3
  #define SDSS              53 // EINY 0-.1 - EINSY 0.3
  #define LCD_PINS_ENABLE 18
  #define LCD_PINS_D4     19
#else
  // EINSY 0.4+
  #define SDSS                77 // EINSY 0.4
  #define LCD_PINS_ENABLE 61
  #define LCD_PINS_D4 59
#endif


//#define ENDSTOP_PIN 15
//#define START_PIN 14
//#define POWER_PIN 3
//#define X_MS1_PIN 40
//#define X_MS2_PIN 41
//#define Y_MS1_PIN 69
//#define Y_MS2_PIN 39
//#define Z_MS1_PIN 68
//#define Z_MS2_PIN 67
//#define E0_MS1_PIN 65
//#define E0_MS2_PIN 66
//#define E1_MS1_PIN 63
//#define E1_MS2_PIN 64
//#define DIGIPOTSS_PIN 38
#define X_REF A8
#define Y_REF A6
#define Z_REF A5
#define E0_REF A4
#define E1_REF A3
//#define MOS1 44 //PL5
//#define MOS2 32 //PC5
//#define MOS3 45 //PL4
#define MOS4 31 //PC6
#define MOS5 46 //PL3
#define MOS6 30 //PC7
/*
#define MOS1 44 //PL5
#define MOS2 32 //PC5
#define MOS3 45 //PL4
#define MOS4 31 //PC6
#define MOS5 46 //PL3
#define MOS6 30 //PC7
*/

namespace einsyrambo{
  void init();
  void portStep();
  void portDirection(byte dir);
  void portEnable(byte en);
  void portSetMicroSteps(byte ms);
  void setMotorCurrent(uint8_t x);

  void sdinit();
}

void spiflash_init();
void tmc2130_write(uint8_t chipselect, uint8_t address,uint8_t wval1,uint8_t wval2,uint8_t wval3,uint8_t wval4);
void spiflash_write(byte data);

#endif

#endif
