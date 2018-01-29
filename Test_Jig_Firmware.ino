#include "digipot.h"
#include "pins.h"
#include "rambo.h"
#include <SPI.h>

#define DEBUG 0 //send debug info over serial about what we are doing
#define DEBOUNCE 3

#if DEBUG == 0
#define DEBUG_PRINT(x,y) 
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
unsigned long startMillis;
unsigned long lastMicros;
unsigned long period;
unsigned long stepsToHome = 0;
unsigned long testStart;
uint8_t i, j;
uint8_t posCounter[5] = {0,0,0,0,0};
uint8_t consecutiveReads[5] = {0,0,0,0,0};
byte startReads = 0;
unsigned long stepperCount[5][10];
char port;
char state; 

void setup()
{ 
  //http://arduino.cc/en/reference/serial
  Serial.begin(115200); 

  //setup pins
  pinMode(ENDSTOP_PIN, INPUT); //Endstop
  digitalWrite(ENDSTOP_PIN,HIGH); //turn on endstop pullups
  pinMode(START_PIN, INPUT); //Start Pin
  digitalWrite(START_PIN,HIGH); //Start Pin pullups  
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

 
  startMillis = millis();
  rambo::portEnable(0);
  rambo::portSetMicroSteps(16);
  //init digipots
  digipot::init();
}

void loop()
{
  //uint8_t a= PINJ;
  //Serial.print(PINJ,BIN);
  //Serial.println("");
  uint8_t current;
  if(!digitalRead(START_PIN))
  {
    startReads++;
    if(startReads >= DEBOUNCE && millis()-startMillis >= 1000){
      Serial.println("start");
      startReads = 0;
      startMillis = millis();
    }
  }
  if(Serial.available())
  {
    currentChar = Serial.read();
    DEBUG_PRINT("Recieved command : ", currentChar);
    delay(10);
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
          rambo::portEnable(1);
          stepsToHome = 0;
          lastMicros = micros();

          //if we are already at the endstop move upwards until we are not
          if(digitalRead(ENDSTOP_PIN)){
            rambo::portDirection(UP);
            while(digitalRead(ENDSTOP_PIN) || stepsToHome <=1000){
              if(!digitalRead(ENDSTOP_PIN)) stepsToHome++;
              if ((micros()-lastMicros) >= period) 
              { 
                rambo::portStep(); 
                lastMicros = micros();
              } 
            }
          }
          stepsToHome =0;
          rambo::portDirection(DOWN);
          while(!digitalRead(ENDSTOP_PIN)){
            if ((micros()-lastMicros) >= period) 
            { 
              rambo::portStep(); 
              lastMicros = micros();
              stepsToHome++;
            }
          }
          Serial.println(stepsToHome);
        }
        rambo::portEnable(0);
        finished();
        break;
      }

      //Analog Read
      //Format: A<pin>
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

      //Set Microsteps
      //Format: U<microsteps>
    case 'U' : 
      {
        if(isDigit(Serial.peek()))
        {
          pin = Serial.parseInt();
          DEBUG_PRINT("setting microsteps : ", pin);
          rambo::portSetMicroSteps(pin);
        }
        finished();
        break;
      }

      //Set Digital Trimpot
      //Format: V<255>
    case 'V' : 
      {
        if(isDigit(Serial.peek()))
        {
          current = Serial.parseInt();
          DEBUG_PRINT("setting digital trimpot: ", current);
          digipot::setMotorCurrent(current);
        }
        finished();
        break;
      }


      //Monitor Stepper test
      //Format: M<time>F<frequency>
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
                for(i=0; i<=4; i++){
                  if(((lastPortSample ^ sample) & (B00000100<<i)) && consecutiveReads[i] >= DEBOUNCE){
                    posCounter[i]++;
                    consecutiveReads[i] = 0;
                  }
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
          }
        }
        finished();
        break;
      }

      //Write PWM
      //Format: P<pin>D<duty cycle>
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

      //Clamp board
      //Format: C<steps>F<frequency><Direction - D or U>
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
            rambo::portDirection(UP);
            switch (Serial.read()) {
              //Up
            case 'U' : 
              {
                DEBUG_PRINT("Going up.",' ');
                rambo::portDirection(UP);
                break;
              }
              //Down
            case 'D' : 
              {
                DEBUG_PRINT("Going down.",' ');
                rambo::portDirection(DOWN);
                break;
              }
            }
            rambo::portEnable(1);
            stepsToHome = 0;
            lastMicros = micros();
            while(stepsToHome <= stepCount){
              if ((micros()-lastMicros) >= period) 
              { 
                rambo::portStep(); 
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

void finished(void){
  Serial.println("ok");
}







