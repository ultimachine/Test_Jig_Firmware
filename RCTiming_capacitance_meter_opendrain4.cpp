
#include "RCTiming_capacitance_meter_opendrain4.h"
#include <stdint.h>

float nanoFaradsOnPin(uint8_t signalPin, float resistorValue) {
  return ((float)readDigitalPullupRiseTime(signalPin) / resistorValue) * 1000;
}

uint32_t readDigitalPullupRiseTime(uint8_t signalPin) {
  unsigned long startTime;
  unsigned long elapsedTime;
  
  /* Discharge */
  pinMode(signalPin, OUTPUT);
  digitalWrite(signalPin, LOW);

  /* Wait for discharge */
  delay(100);

  /* Start charging */
  pinMode(signalPin, INPUT);

  startTime = micros();

  /* Wait while still discharged */
  //while(analogRead(signalPin) < 648) { }
  while(digitalRead(signalPin) == LOW) { }

  elapsedTime = micros() - startTime;

  return elapsedTime;
}

/*

void setup(){
  Serial.begin(115200);             // initialize serial transmission for debugging
  Serial.println("Push Low then Check Rise Time Capacitance Meter");
}

float tachPullup = 3800.0F; //Ohms

float thermistorTotalPullup = 2370.0F + 2370.0F + 4700.0F;

void loop() {
  Serial.print("D79=");
  Serial.println( nanoFaradsOnPin(79, tachPullup) );
  Serial.print("D80=");
  Serial.println( nanoFaradsOnPin(80, tachPullup) );

  Serial.print("T0=");
  Serial.println( nanoFaradsOnPin(A0, thermistorTotalPullup) );
  Serial.print("T1=");
  Serial.println( nanoFaradsOnPin(A1, thermistorTotalPullup) );
  Serial.print("T3=");
  Serial.println( nanoFaradsOnPin(A2, thermistorTotalPullup) );
  Serial.print("ZT=");
  Serial.println( nanoFaradsOnPin(A3, thermistorTotalPullup) );

  delay(500);
}

*/
