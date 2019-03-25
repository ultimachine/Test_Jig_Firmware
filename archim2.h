#pragma once

#include "pins.h"

#ifdef BOARD_ARCHIM2

#ifndef __SAM3X8E__
  #error "Oops! Select 'Archim' in 'Tools > Board.'"
#endif

#define MainSerial SerialUSB

#define SDHSMCI_SUPPORT
#define SPIFLASH_SUPPORT

//For tmc2130.cpp tmc2130.h
#define HAVE_TMC2130_DRIVERS
#define TMC2130_USES_SW_SPI

#define TMC_SWSPI_MISO_PIN 26 //PD1 MISO
#define TMC_SWSPI_SCK_PIN  27 //PD2 SCK
#define TMC_SWSPI_MOSI_PIN 28 //PD3 MOSI

#define X_STEP_PIN         38 //PC6 X-STEP *
#define X_DIR_PIN          37 //PC5 X-DIR *
#define X_ENABLE_PIN       41 //PC9 X-EN
#define X_MIN_PIN          14 //PD4 MIN ES1
#define X_MAX_PIN          32 //PD10 MAX ES1
#define X_TMC2130_CS       39 //PC7 X_nCS
#define X_DIAG_PIN         59 //PA4 X_DIAG

#define Y_STEP_PIN         51 //PC12 Y-STEP *
#define Y_DIR_PIN          92 //PC11 Y-DIR -AddOns *
#define Y_ENABLE_PIN       49 //PC14 Y-EN *
#define Y_MIN_PIN          29 //PD6 MIN ES2
#define Y_MAX_PIN          15 //PD5 MAX ES2
#define Y_TMC2130_CS       50 //PC13 Y_nCS
#define Y_DIAG_PIN         48 //PC15 Y_DIAG

#define Z_STEP_PIN         46 //PC17 Z-STEP *
#define Z_DIR_PIN          47 //PC16 Z-DIR *
#define Z_ENABLE_PIN       44 //PC19 Z-EN *
#define Z_MIN_PIN          31 //PA7 MIN ES3
#define Z_MAX_PIN          30 //PD9 MAX ES3
#define Z_TMC2130_CS       45 //PC18 Z_nCS
#define Z_DIAG_PIN         36 //PC4 Z_DIAG

#define E0_STEP_PIN       107 //PB10 E1-STEP -AddOns *
#define E0_DIR_PIN         96 //PC10 E1-DIR -AddOns *
#define E0_ENABLE_PIN     105 //PB22 E1-EN -AddOns *
#define E0_TMC2130_CS     104 //PC20 E1_nCS -AddOns *
#define E0_DIAG_PIN        78 //PB23 E1_DIAG

#define E1_STEP_PIN        22 //PB26 E2_STEP *
#define E1_DIR_PIN         97 //PB24 E2_DIR -AddOns *
#define E1_ENABLE_PIN      18 //PA11 E2-EN
#define E1_TMC2130_CS      19 //PA10 E2_nCS
#define E1_DIAG_PIN        25 //PD0 E2_DIAG

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
