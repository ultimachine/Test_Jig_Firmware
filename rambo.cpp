#include <Arduino.h>
#include "pins.h"
#include "rambo.h"
#include "digipot.h"

#ifdef BOARD_RAMBO

#define MainSerial Serial

void rambo::init(){
  rambo::portEnable(0);
  rambo::portSetMicroSteps(16);

  //set outputs
  DDRA = B11111000; //enable
  DDRL = B11000111; //direction
  DDRC = B00011111; //step

  //set inputs
  DDRJ = B00000000; //stepper monitors

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

  //pinMode(POWER_PIN, OUTPUT); //powersupply pin

  digipot::init();

  /*
  pinMode(X_REF,INPUT);
   pinMode(Y_REF,INPUT); 
   pinMode(Z_REF,INPUT);
   pinMode(E0_REF,INPUT);
   pinMode(E1_REF,INPUT);
   pinMode(MOS1,INPUT);
   pinMode(MOS2,INPUT);
   pinMode(MOS3,INPUT);
   pinMode(MOS4,INPUT);
   pinMode(MOS5,INPUT);
   pinMode(MOS6,INPUT);
   digitalWrite(MOS1,HIGH);  
   digitalWrite(MOS2,HIGH);  
   digitalWrite(MOS3,HIGH);  
   digitalWrite(MOS4,HIGH);  
   digitalWrite(MOS5,HIGH);  
   digitalWrite(MOS6,HIGH);  
   */
}

void rambo::portStep(){
  PORTC = B11111111;
  delayMicroseconds(1);
  PORTC = B11100000;
  return;
}

void rambo::portDirection(byte dir){
  if(dir)PORTL = B11111111;
  else PORTL = B00111000;
  return;
}

void rambo::portEnable(byte en){
  if(en)PORTA = B00000000;
  else PORTA = B11111000;
  return;
}

void rambo::portSetMicroSteps(byte ms){
  switch(ms) {
  case 1 : 
    {
      digitalWrite(X_MS1_PIN, LOW);
      digitalWrite(X_MS2_PIN, LOW);
      digitalWrite(Y_MS1_PIN, LOW);
      digitalWrite(Y_MS2_PIN, LOW);
      digitalWrite(Z_MS1_PIN, LOW);
      digitalWrite(Z_MS2_PIN, LOW);
      digitalWrite(E0_MS1_PIN, LOW);
      digitalWrite(E0_MS2_PIN, LOW);
      digitalWrite(E1_MS1_PIN, LOW);
      digitalWrite(E1_MS2_PIN, LOW);
      break;
    }
  case 2 : 
    {
      digitalWrite(X_MS1_PIN, HIGH);
      digitalWrite(X_MS2_PIN, LOW);
      digitalWrite(Y_MS1_PIN, HIGH);
      digitalWrite(Y_MS2_PIN, LOW);
      digitalWrite(Z_MS1_PIN, HIGH);
      digitalWrite(Z_MS2_PIN, LOW);
      digitalWrite(E0_MS1_PIN, HIGH);
      digitalWrite(E0_MS2_PIN, LOW);
      digitalWrite(E1_MS1_PIN, HIGH);
      digitalWrite(E1_MS2_PIN, LOW);
      break;   
    }
  case 4 : 
    {
      digitalWrite(X_MS1_PIN, LOW);
      digitalWrite(X_MS2_PIN, HIGH);
      digitalWrite(Y_MS1_PIN, LOW);
      digitalWrite(Y_MS2_PIN, HIGH);
      digitalWrite(Z_MS1_PIN, LOW);
      digitalWrite(Z_MS2_PIN, HIGH);
      digitalWrite(E0_MS1_PIN, LOW);
      digitalWrite(E0_MS2_PIN, HIGH);
      digitalWrite(E1_MS1_PIN, LOW);
      digitalWrite(E1_MS2_PIN, HIGH);
      break;  
    }
  case 16 : 
    {
      digitalWrite(X_MS1_PIN, HIGH);
      digitalWrite(X_MS2_PIN, HIGH);
      digitalWrite(Y_MS1_PIN, HIGH);
      digitalWrite(Y_MS2_PIN, HIGH);
      digitalWrite(Z_MS1_PIN, HIGH);
      digitalWrite(Z_MS2_PIN, HIGH);
      digitalWrite(E0_MS1_PIN, HIGH);
      digitalWrite(E0_MS2_PIN, HIGH);
      digitalWrite(E1_MS1_PIN, HIGH);
      digitalWrite(E1_MS2_PIN, HIGH);
      break;   
    }

  } 
}

void rambo::setMotorCurrent(uint8_t current)
{
  digipot::setMotorCurrent(current);
}

void rambo::sdinit() {}
void spiflash_init() {}

#endif
