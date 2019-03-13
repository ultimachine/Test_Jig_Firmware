#include <Arduino.h>
#include "pins.h"
#include "archim.h"
#include "Arduino_Due_SD_HSCMI.h"
#include <SPI.h>

#include <SPIFlash.h>

#ifdef BOARD_ARCHIM

#ifdef SDHSMCI_SUPPORT
  #include "Arduino_Due_SD_HSCMI.h"
  #include <SD_HSMCI.h>
#endif

void archim::init()
{
  archim::portEnable(0);
  archim::portSetMicroSteps(16);

  pinMode(X_MS1_PIN, OUTPUT); //microstep pin
  pinMode(Y_MS1_PIN, OUTPUT); //microstep pin
  pinMode(Z_MS1_PIN, OUTPUT); //microstep pin
  pinMode(E0_MS1_PIN, OUTPUT); //microstep pin
  pinMode(E1_MS1_PIN, OUTPUT); //microstep pin
  pinMode(X_MS2_PIN, OUTPUT); //microstep pin
  pinMode(Y_MS2_PIN, OUTPUT); //microstep pin
  pinMode(Z_MS2_PIN, OUTPUT); //microstep pin
  pinMode(E0_MS2_PIN, OUTPUT); //microstep pin
  pinMode(E1_MS2_PIN, OUTPUT); //microstep pin

  pinMode(X_MS3_PIN, OUTPUT); //microstep pin
  pinMode(Y_MS3_PIN, OUTPUT); //microstep pin
  pinMode(Z_MS3_PIN, OUTPUT); //microstep pin
  pinMode(E0_MS3_PIN, OUTPUT); //microstep pin
  pinMode(E1_MS3_PIN, OUTPUT); //microstep pin

  pinMode(X_ENABLE_PIN, OUTPUT); //microstep pin
  pinMode(Y_ENABLE_PIN, OUTPUT); //microstep pin
  pinMode(Z_ENABLE_PIN, OUTPUT); //microstep pin
  pinMode(E0_ENABLE_PIN, OUTPUT); //microstep pin
  pinMode(E1_ENABLE_PIN, OUTPUT); //microstep pin

  pinMode(X_STEP_PIN, OUTPUT); //microstep pin
  pinMode(Y_STEP_PIN, OUTPUT); //microstep pin
  pinMode(Z_STEP_PIN, OUTPUT); //microstep pin
  pinMode(E0_STEP_PIN, OUTPUT); //microstep pin
  pinMode(E1_STEP_PIN, OUTPUT); //microstep pin

  pinMode(X_DIR_PIN, OUTPUT); //microstep pin
  pinMode(Y_DIR_PIN, OUTPUT); //microstep pin
  pinMode(Z_DIR_PIN, OUTPUT); //microstep pin
  pinMode(E0_DIR_PIN, OUTPUT); //microstep pin
  pinMode(E1_DIR_PIN, OUTPUT); //microstep pin

  analogWrite(MOTOR_CURRENT_PWM_X_PIN,92);
  analogWrite(MOTOR_CURRENT_PWM_Y_PIN,92);
  analogWrite(MOTOR_CURRENT_PWM_Z_PIN,92);
  analogWrite(MOTOR_CURRENT_PWM_E0_PIN,92);
  analogWrite(MOTOR_CURRENT_PWM_E1_PIN,92);

  pinMode(X_MIN_PIN,INPUT_PULLUP);
  pinMode(Y_MIN_PIN,INPUT_PULLUP);
  pinMode(Z_MIN_PIN,INPUT_PULLUP);
  pinMode(X_MAX_PIN,INPUT_PULLUP);
  pinMode(Y_MAX_PIN,INPUT_PULLUP);
  pinMode(Z_MAX_PIN,INPUT_PULLUP);

}

void archim::portStep()
{
  digitalWrite( X_STEP_PIN,HIGH);
  digitalWrite( Y_STEP_PIN,HIGH);
  digitalWrite( Z_STEP_PIN,HIGH);
  digitalWrite(E0_STEP_PIN,HIGH);
  digitalWrite(E1_STEP_PIN,HIGH);

  delayMicroseconds(2);

  digitalWrite( X_STEP_PIN,LOW);
  digitalWrite( Y_STEP_PIN,LOW);
  digitalWrite( Z_STEP_PIN,LOW);
  digitalWrite(E0_STEP_PIN,LOW);
  digitalWrite(E1_STEP_PIN,LOW);
}

void archim::portDirection(byte dir){
  if(dir) {
    digitalWrite( X_DIR_PIN, HIGH);
    digitalWrite( Y_DIR_PIN, HIGH);
    digitalWrite( Z_DIR_PIN, HIGH);
    digitalWrite(E0_DIR_PIN, HIGH);
    digitalWrite(E1_DIR_PIN, HIGH);
  }
  else {
    digitalWrite( X_DIR_PIN, LOW);
    digitalWrite( Y_DIR_PIN, LOW);
    digitalWrite( Z_DIR_PIN, LOW);
    digitalWrite(E0_DIR_PIN, LOW);
    digitalWrite(E1_DIR_PIN, LOW);
  }
}

void archim::portEnable(byte en){
  if(en) {
    digitalWrite( X_ENABLE_PIN, LOW);
    digitalWrite( Y_ENABLE_PIN, LOW);
    digitalWrite( Z_ENABLE_PIN, LOW);
    digitalWrite(E0_ENABLE_PIN, LOW);
    digitalWrite(E1_ENABLE_PIN, LOW);
  }
  else {
    digitalWrite( X_ENABLE_PIN, HIGH);
    digitalWrite( Y_ENABLE_PIN, HIGH);
    digitalWrite( Z_ENABLE_PIN, HIGH);
    digitalWrite(E0_ENABLE_PIN, HIGH);
    digitalWrite(E1_ENABLE_PIN, HIGH);
  }
}

void set_modes0(byte msmode)
{
      digitalWrite( X_MS1_PIN, msmode);
      digitalWrite( Y_MS1_PIN, msmode);
      digitalWrite( Z_MS1_PIN, msmode);
      digitalWrite(E0_MS1_PIN, msmode);
      digitalWrite(E1_MS1_PIN, msmode);
}

void set_modes1(byte msmode)
{
      digitalWrite( X_MS2_PIN, msmode);
      digitalWrite( Y_MS2_PIN, msmode);
      digitalWrite( Z_MS2_PIN, msmode);
      digitalWrite(E0_MS2_PIN, msmode);
      digitalWrite(E1_MS2_PIN, msmode);
}

void set_modes2(byte msmode)
{
      digitalWrite( X_MS3_PIN, msmode);
      digitalWrite( Y_MS3_PIN, msmode);
      digitalWrite( Z_MS3_PIN, msmode);
      digitalWrite(E0_MS3_PIN, msmode);
      digitalWrite(E1_MS3_PIN, msmode);
}

/* drv8825 microstep modes table
mode2, mode1, mode0, step mode
0 0 0 Full step (2-phase excitation) with 71% current
0 0 1 1/2 step (1-2 phase excitation)
0 1 0 1/4 step (W1-2 phase excitation)
0 1 1 8 microsteps/step
1 0 0 16 microsteps/step
1 0 1 32 microsteps/step
1 1 0 32 microsteps/step
1 1 1 32 microsteps/step */
void archim::portSetMicroSteps(byte ms)
{
 switch(ms) {
  case 1: 
      set_modes2(LOW);
      set_modes1(LOW);
      set_modes0(LOW);
      break;
  case 2: 
      set_modes2(LOW);
      set_modes1(LOW);
      set_modes0(HIGH);
      break;   
  case 4:
      set_modes2(LOW);
      set_modes1(HIGH);
      set_modes0(LOW);
      break;
  case 8: 
      set_modes2(LOW);
      set_modes1(HIGH);
      set_modes0(HIGH);
      break;
  case 16: 
      set_modes2(HIGH);
      set_modes1(LOW);
      set_modes0(LOW);
      break;
  case 32: 
      set_modes2(HIGH);
      set_modes1(HIGH);
      set_modes0(HIGH);
      break;
 }
}

void archim::sdinit() {
  #ifdef SDHSMCI_SUPPORT
  SD.Init();
  if(sd_mmc_check(0) == SD_MMC_OK) {
    SerialUSB.println("42"); // sd ok
  } else {
    SerialUSB.println("1"); // sd failed
  }
  #else
    SerialUSB.println("2"); // sd not supported
  #endif
}


// --------------------------------------------------------------------------
// spiflash
// --------------------------------------------------------------------------
#define SPIFLASH_CS 86 //77  // Chip Select PIN
#define SPIFLASH_WRITE_ENABLE  0x06  //Write Enable (06h)
#define SPIFLASH_READ_STATUS   0x05  //Read Status Register (05h)
#define SPIFLASH_WRITE_IN_PROGRESS_MASK 0b00000001
#define SPIFLASH_PAGE_PROGRAM  0x02  //Page Program (PP) (02h)
#define SPIFLASH_READ_DATA     0x03  //Read Data (03h)
#define SPIFLASH_SECTOR_ERASE  0x20  //Sector Erase (SE) (20h)
#define SPIFLASH_READ_ID       0x90  //Read Manufacturer / Device ID

void spiflash_init()
{
  pinMode(SPIFLASH_CS,OUTPUT);
  digitalWrite(SPIFLASH_CS,HIGH);
  SPI.begin(SPIFLASH_CS);

  digitalWrite(SPIFLASH_CS,LOW);
  // MFG ID
  SPI.transfer(SPIFLASH_CS, SPIFLASH_READ_ID, SPI_CONTINUE);
  SPI.transfer(SPIFLASH_CS, 0x00, SPI_CONTINUE);
  SPI.transfer(SPIFLASH_CS, 0x00, SPI_CONTINUE);
  SPI.transfer(SPIFLASH_CS, 0x00, SPI_CONTINUE);
  //SerialUSB.print(PSTR("SPIFLASH_MFG_ID: ")); 
  /*SerialUSB.print(*/ SPI.transfer(SPIFLASH_CS, 0x00, SPI_CONTINUE) /*)*/;
  //SerialUSB.print(" "); 
  SerialUSB.println( SPI.transfer(SPIFLASH_CS, 0x00) );

  digitalWrite(SPIFLASH_CS,HIGH);
  pinMode(E0_STEP_PIN,OUTPUT);
  digitalWrite(E0_STEP_PIN,LOW);
}

uint8_t spiflash_busy()
{
  SPI.transfer(SPIFLASH_CS, SPIFLASH_READ_STATUS, SPI_CONTINUE);
  uint8_t busy = SPI.transfer(SPIFLASH_CS, 0x00) & 0b00000001;
  if(busy) SerialUSB.println(PSTR("SPIFLASH BUSY"));
  return(busy);
}

uint8_t spiflash_read_byte(long address)
{
  SPI.transfer(SPIFLASH_CS, SPIFLASH_READ_DATA, SPI_CONTINUE);
  SPI.transfer(SPIFLASH_CS, address >> 16, SPI_CONTINUE);
  SPI.transfer(SPIFLASH_CS, address >> 8, SPI_CONTINUE);
  SPI.transfer(SPIFLASH_CS, address, SPI_CONTINUE);
  uint8_t result = SPI.transfer(SPIFLASH_CS, 0x00);
  return result;  
}

void spiflash_erase(long address)
{
  //SerialUSB.println(PSTR("SPI FLASH ERASE!"));
  SPI.transfer(SPIFLASH_CS, SPIFLASH_WRITE_ENABLE);
  SPI.transfer(SPIFLASH_CS, SPIFLASH_SECTOR_ERASE, SPI_CONTINUE);
  SPI.transfer(SPIFLASH_CS, address >> 16, SPI_CONTINUE);
  SPI.transfer(SPIFLASH_CS, address >> 8, SPI_CONTINUE);
  SPI.transfer(SPIFLASH_CS, address);
  while(spiflash_busy());  
}

void spiflash_write_byte(long address, uint8_t value)
{
  while(spiflash_busy());
  SPI.transfer(SPIFLASH_CS, SPIFLASH_WRITE_ENABLE);
  SPI.transfer(SPIFLASH_CS, SPIFLASH_PAGE_PROGRAM, SPI_CONTINUE);
  SPI.transfer(SPIFLASH_CS, address >> 16, SPI_CONTINUE);
  SPI.transfer(SPIFLASH_CS, address >> 8, SPI_CONTINUE);
  SPI.transfer(SPIFLASH_CS, address, SPI_CONTINUE);
  SPI.transfer(SPIFLASH_CS, value); // Value to Write
}

void archim::setMotorCurrent(byte x)
{
  analogWrite(MOTOR_CURRENT_PWM_X_PIN,x);
  analogWrite(MOTOR_CURRENT_PWM_Y_PIN,x);
  analogWrite(MOTOR_CURRENT_PWM_Z_PIN,x);
  analogWrite(MOTOR_CURRENT_PWM_E0_PIN,x);
  analogWrite(MOTOR_CURRENT_PWM_E1_PIN,x);
}

SPIFlash spiflash(SPIFLASH_CS);

// Read JEDEC ID and do a simple erase/write/read test
void spiflash_init_new()
{
  spiflash.begin();
  spiflash.setClock(4000000);
  //MainSerial.print("JEDEC ID: ");
  MainSerial.println( spiflash.getJEDECID() );
}

void spiflash_write(byte data)
{
  spiflash.eraseSector(0);
  spiflash.writeByte(0, data); //addr,data
  MainSerial.println( spiflash.readByte(0) );
}


#endif // BOARD_ARCHIM
