#include <Arduino.h>
#include "pins.h"
#include "minirambo.h"
//#include "digipot.h"

#ifdef BOARD_MINIRAMBO

#define MainSerial Serial

void minirambo::init(){
  minirambo::portEnable(0);
  minirambo::portSetMicroSteps(16);

  //set outputs
  DDRA = B11110000; //enable X-PA7, Y-PA6, Z-PA5, E0-PA4, E1-PA3
  DDRL = B01000111; //direction E1-PL7, E0-PL6, Z-PL2, X-PL1, Y-PL0
  DDRC = B00001111; //step E1-PC4, E0-PC3, Z-PC2, Y-PC1, X-PC0

  //set inputs
  //DDRJ = B00000000; //stepper monitors

  pinMode(X_MS1_PIN, OUTPUT); //microstep pin
  pinMode(Y_MS1_PIN, OUTPUT); //microstep pin
  pinMode(Z_MS1_PIN, OUTPUT); //microstep pin
  pinMode(E0_MS1_PIN, OUTPUT); //microstep pin
  //pinMode(E1_MS1_PIN, OUTPUT); //microstep pin
  pinMode(X_MS2_PIN, OUTPUT); //microstep pin
  pinMode(Y_MS2_PIN, OUTPUT); //microstep pin
  pinMode(Z_MS2_PIN, OUTPUT); //microstep pin
  pinMode(E0_MS2_PIN, OUTPUT); //microstep pin
  //pinMode(E1_MS2_PIN, OUTPUT); //microstep pin

  //pinMode(POWER_PIN, OUTPUT); //powersupply pin

  //digipot::init();

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

void minirambo::portStep(){
  PORTC |= B00001111; //pin mask high
  delayMicroseconds(1);
  PORTC &= ~(B00001111); //pin mask low
  return;
}

void minirambo::portDirection(byte dir){
  if(dir)PORTL |=  B01000111;  //pin mask high
  else PORTL &= ~(B01000111); //pin mask low
  return;
}

void minirambo::portEnable(byte en){
  if(en)PORTA &= ~(B11110000); //pin mask low - enable a4982
  else PORTA |= B11110000; //pin mask high - disable a4982
  return;
}

void minirambo::portSetMicroSteps(byte ms){
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
      //digitalWrite(E1_MS1_PIN, LOW);
      //digitalWrite(E1_MS2_PIN, LOW);
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
      //digitalWrite(E1_MS1_PIN, HIGH);
      //digitalWrite(E1_MS2_PIN, LOW);
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
      //digitalWrite(E1_MS1_PIN, LOW);
      //digitalWrite(E1_MS2_PIN, HIGH);
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
      //digitalWrite(E1_MS1_PIN, HIGH);
      //digitalWrite(E1_MS2_PIN, HIGH);
      break;   
    }

  } 
}

void minirambo::sdinit() {}
void spiflash_init() {}

#endif
