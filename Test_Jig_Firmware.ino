#include "digipot.h"
#include <SPI.h>

#define DEBUG 1 //send debug info over serial about what we are doing
#define DEBOUNCE 3

#define ENDSTOP_PIN 10
#define POWER_PIN 3
#define X_MS1_PIN 40
#define X_MS2_PIN 41
#define Y_MS1_PIN 69
#define Y_MS2_PIN 39
#define Z_MS1_PIN 68
#define Z_MS2_PIN 67
#define E0_MS1_PIN 65
#define E0_MS2_PIN 66
#define E1_MS1_PIN 63
#define E1_MS2_PIN 64

#if DEBUG == 0
#define DEBUG_PRINT(x,y) ;;
#endif
#if DEBUG == 1
#define DEBUG_PRINT(x,y) Serial.print(x); Serial.println(y);
#endif

#define UP 0
#define DOWN 1

char currentChar; //current char we are processing
uint8_t pin; //current pin we are playing with
uint32_t stepCount; //clamp
uint32_t stepFrequency; //clamp
uint8_t dutyCycle; //analog write
uint8_t monitorTime;
uint16_t sampleFrequency;
unsigned long lastMicros;
unsigned long period;
unsigned long stepsToHome = 0;
unsigned long testStart;
uint8_t i, j;
uint8_t posCounter[5] = {
  0,0,0,0,0};
uint8_t consecutiveReads[5] = {
  0,0,0,0,0};
unsigned long stepperCount[5][10];
char port;
char state; 

void setup()
{ 
  //http://arduino.cc/en/reference/serial
  Serial.begin(115200); 
  Serial.println("start");

  //setup pins
  pinMode(ENDSTOP_PIN, INPUT); //Endstop
  digitalWrite(ENDSTOP_PIN,HIGH); //turn on endstop pullups
  pinMode(POWER_PIN, OUTPUT); //powersupply pin

  //RAMBo
  DDRA = B11111111; //enable
  DDRL = B11111111; //direction
  DDRC = B11111111; //step
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

  ramboEnable(1);
  setMicroSteps(16);
  //init digipots
  digipotInit();
}

void loop()
{
  //uint8_t a= PINJ;
  //Serial.print(PINJ,BIN);
  //Serial.println("");
  if(Serial.available())
  {
    currentChar = Serial.read();
    DEBUG_PRINT("Recieved command : ", currentChar);

    switch (currentChar)
    {

      //Read Port or Pin
      //Format: R<pin> or R<port char>
      //Returns: <port int val>\n or <pin val>\n
    case 'R' : 
      {
        if(isAlpha(Serial.peek()))
        {
          port = Serial.read();
          DEBUG_PRINT("Reading port : ", port);
          Serial.println(getPin(port));
        }
        else if(isDigit(Serial.peek()))
        {
          pin = Serial.parseInt();
          DEBUG_PRINT("Reading pin : ", pin);
          Serial.println(digitalRead(pin));
        }
        finished();
        break; 
      }

      //Write Pin
      //Format: W<pin><'H' or 'L'>
    case 'W' : 
      {
        //Pin
        if(isDigit(Serial.peek()))
        {
          pin = Serial.parseInt();
          DEBUG_PRINT("Writing to pin : ", pin);
          switch (Serial.read()) {
            //High
          case 'H' : 
            {
              DEBUG_PRINT("Setting pin high",' ');
              digitalWrite(pin,1);
              break;
            }
            //L
          case 'L' : 
            {
              DEBUG_PRINT("Setting pin low",' ');
              digitalWrite(pin,0);
              break;
            }
          }
        }
        finished();
        break; 
      }

      //Home
      //Format: H<step frequency>
      //Returns: Steps taken to home
    case 'H' : 
      {
        if(isDigit(Serial.peek()))
        {
          stepFrequency = Serial.parseInt();
          DEBUG_PRINT("Homing at step frequency (hz) : ", stepFrequency);
          period = 1000000/stepFrequency;
          DEBUG_PRINT("Stepping every (us) : ", period);
          ramboDirection(DOWN);
          ramboEnable(1);
          stepsToHome = 0;
          lastMicros = micros();
          while(!digitalRead(ENDSTOP_PIN)){
            if ((micros()-lastMicros) >= period) 
            { 
              ramboStep(); 
              lastMicros = micros();
              stepsToHome++;
            }
          }
          Serial.println(stepsToHome);
        }
        finished();
        break;
      }

      //Analog Read -> A<pin>
    case 'A' : 
      {
        if(isDigit(Serial.peek()))
        {
          pin = Serial.parseInt();
          DEBUG_PRINT("Analog reading pin : ", pin);
          Serial.println(analogRead(pin));
        }
        finished();
        break;
      }

      //Monitor Stepper test -> M<time>F<frequency>
    case 'M' : 
      {
        if(isDigit(Serial.peek()))
        {
          monitorTime = Serial.parseInt();
          DEBUG_PRINT("Monitoring stepper test time (s) : ", monitorTime);
          if (Serial.peek() == 'F')
          {
            sampleFrequency = Serial.parseInt();
            DEBUG_PRINT("Monitoring at frequency (hz) : ", sampleFrequency);
            period = 1000000/sampleFrequency;
            DEBUG_PRINT("Monitoring every (us) : ", period);
            testStart = millis();
            lastMicros = micros();
            DEBUG_PRINT("Starting test", "");
            uint8_t lastPortSample = B11111111;
            for(i=0; i<=4; i++)
            { 
              posCounter[i] = 0; 
              consecutiveReads[i] = 0; 
            }
            for(i=0; i<=4; i++)
            {
              for(j = 0; j <= 9; j++) 
                stepperCount[i][j] = 0;
            }
            while(millis()-testStart <= monitorTime*1000){
              if ((micros()-lastMicros) >= period) 
              { 
                lastMicros = micros();
                uint8_t sample = PINJ;
                //for(i=0; i<=4; i++){
                if(((lastPortSample ^ sample) & B00000100) && consecutiveReads[0] >= DEBOUNCE){
                  posCounter[0]++;
                  consecutiveReads[0] = 0;
                }
                if(((lastPortSample ^ sample) & B00001000) && consecutiveReads[1] >= DEBOUNCE){
                  posCounter[1]++;
                  consecutiveReads[1] = 0;
                }
                if(((lastPortSample ^ sample) & B00010000) && consecutiveReads[2] >= DEBOUNCE){
                  posCounter[2]++;
                  consecutiveReads[2] = 0;
                }
                if(((lastPortSample ^ sample) & B00100000) && consecutiveReads[3] >= DEBOUNCE){
                  posCounter[3]++;
                  consecutiveReads[3] = 0;
                }
                if(((lastPortSample ^ sample) & B01000000) && consecutiveReads[4] >= DEBOUNCE){
                  posCounter[4]++;
                  consecutiveReads[4] = 0;
                }
                for(i=0; i<=4; i++) {
                  stepperCount[i][posCounter[i]]++;
                  consecutiveReads[i]++;
                }
                
                lastPortSample = PINJ;
              }
            }
            DEBUG_PRINT("Ending Test", "");
            for(i=0; i<=4; i++){
              for(j = 0; j <= 9; j++){
                if(j==0){
                  Serial.print("{");
                  Serial.print(stepperCount[i][j]);
                  Serial.print(",");
                  }
                else if(j==9){
                  Serial.print(stepperCount[i][j]);
                  Serial.println("}");
                }
                else {
                  Serial.print(stepperCount[i][j]);
                  Serial.print(",");
                }
              }
            }

            Serial.println(stepsToHome);
          }
        }
        finished();
        break;
      }

      //Write PWM -> P<pin>D<duty cycle>
    case 'P' : 
      {
        if(isDigit(Serial.peek()))
        {
          pin = Serial.parseInt();
          DEBUG_PRINT("Analog write pin : ", pin);
          if(Serial.peek() == 'D')
          {
            dutyCycle = Serial.parseInt();
            DEBUG_PRINT("Analog write duty cycle : ", dutyCycle);
            pinMode(pin,OUTPUT); 
            analogWrite(pin,dutyCycle);
          }
        }
        finished();
        break;
      }

      //Clamp board -> C<steps>F<frequency><Direction - D or U>
    case 'C' : 
      {
        if(isDigit(Serial.peek()))
        {
          stepCount = Serial.parseInt();
          DEBUG_PRINT("Clamping steps : ", stepCount);
          if (Serial.peek() == 'F')
          {
            stepFrequency = Serial.parseInt();
            DEBUG_PRINT("Clamp step frequency (hz) : ", stepFrequency);
            period = 1000000/stepFrequency;
            DEBUG_PRINT("Stepping every (us) : ", period);
            ramboDirection(UP);
            switch (Serial.read()) {
              //Up
            case 'U' : 
              {
                DEBUG_PRINT("Going up.",' ');
                ramboDirection(UP);
                break;
              }
              //Down
            case 'D' : 
              {
                DEBUG_PRINT("Going down.",' ');
                ramboDirection(DOWN);
                break;
              }
            }
            ramboEnable(1);
            stepsToHome = 0;
            lastMicros = micros();
            while(stepsToHome <= stepCount){
              if ((micros()-lastMicros) >= period) 
              { 
                ramboStep(); 
                lastMicros = micros();
                stepsToHome++;
              }
            }
          }
        }
        finished();
        break;
      }
    default :
      Serial.println("Unknown command"); 
      break;
    }
  }
}

uint8_t getPin(char c){
  switch(c){
  case 'A': 
    return PINA;
  case 'B': 
    return PINB;
  case 'C': 
    return PINC;
  case 'D': 
    return PIND;
  case 'E': 
    return PINE;
  case 'F': 
    return PINF;
  case 'G': 
    return PING;
  case 'H': 
    return PINH;
  case 'J': 
    return PINJ;
  case 'K': 
    return PINK;
  case 'L': 
    return PINL;
  }
}

inline void ramboStep(){
  PORTC = B11111111;
  delayMicroseconds(1);
  PORTC = B00000000;
  return;
}

inline void ramboDirection(uint8_t dir){
  if(dir)PORTL = B11111111;
  else PORTL = B00000000;
  return;
}

inline void ramboEnable(uint8_t en){
  if(en)PORTA = B00000000;
  else PORTA = B11111111;
  return;
}

inline void setMicroSteps(uint8_t ms){
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
    }

  } 
}

void finished(void){
  Serial.println("ok");
}





