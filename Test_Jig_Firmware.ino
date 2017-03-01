#include "pins.h"
#include "rambo.h"
#include "archim.h"
#include <SPI.h>

#define DEBUG 0 //send debug info over serial about what we are doing
#define DEBOUNCE 2

#if DEBUG == 0
#define DEBUG_PRINT(x,y) 
#endif
#if DEBUG == 1
#define DEBUG_PRINT(x,y) MainSerial.print(x); MainSerial.println(y);
#endif

#define UP 0
#define DOWN 1

#ifdef BOARD_ARCHIM
namespace board = archim;
#endif
#ifdef BOARD_RAMBO
namespace board = rambo;
#endif


//int sampleFrequency;

//unsigned int period;
//unsigned long testStart;
//uint8_t i, j;
//uint8_t posCounter[5] = { 0,0,0,0,0 };
//uint8_t consecutiveReads[5] = { 0,0,0,0,0 };
//byte startReads = 0;
//unsigned long stepperCount[5][5];
//char port;
//char state; 

void setup()
{
  MainSerial.begin(115200); 

  board::init(); //set microstep mode pins to outputs

  //setup pins
  #ifdef ENDSTOP_PIN
  pinMode(ENDSTOP_PIN, INPUT_PULLUP); //Endstop
  #endif

  #ifdef START_PIN
  pinMode(START_PIN, INPUT_PULLUP); //Start Pin pullups  
  #endif

  while(!MainSerial) { }

  MainSerial.println("1");
}

void loop()
{
  unsigned long stepsToHome = 0;
  unsigned long lastMicros;
  static char currentChar;

  static unsigned long startMillis = millis();

  //uint8_t a= PINJ;
  //MainSerial.print(PINJ,BIN);
  //MainSerial.println("");
  #ifdef START_PIN
  if(!digitalRead(START_PIN))
  {
    startReads++;
    if(startReads >= DEBOUNCE && millis()-startMillis >= 1000){
      MainSerial.println("start");
      startReads = 0;
      startMillis = millis();
    }
  }
  #endif
  if(MainSerial.available())
  {
    char currentChar = MainSerial.read();
    DEBUG_PRINT("Recieved command : ", currentChar);
    delay(10);
    switch (currentChar)
    {
      //Read Pin but turn pullups on
      //Format: Q<pin>
      //Returns: <pin val>\n
    case 'Q' : 
      {
        if(isDigit(MainSerial.peek())){
          uint8_t pin = MainSerial.parseInt();
          pinMode(pin,INPUT_PULLUP);
          MainSerial.println(digitalRead(pin));
        }
        finished();
        break; 
      }

      //Analog Read
      //Format: A<pin>
    case 'A' :
      {
        if(isDigit(MainSerial.peek()))
        {
          uint8_t pin = MainSerial.parseInt();
          //DEBUG MODE BREAKS THERMISTOR READING, LOL HAHAHA
          //DEBUG_PRINT("Analog reading pin : ", pin);
          MainSerial.println(analogRead(pin));
        }
        finished();
        break;
      }

      //Read Port or Pin
      //Format: R<pin> or R<port char>
      //Returns: <port int val>\n or <pin val>\n
    case 'R' : 
      {
        /*
        if(isAlpha(MainSerial.peek()))
        {
          port = MainSerial.read();
          DEBUG_PRINT("Reading port : ", port);
          MainSerial.println(getPin(port));
        }
        else */
        if(isDigit(MainSerial.peek())){
          uint8_t pin = MainSerial.parseInt();
          pinMode(pin,INPUT);
          MainSerial.println(digitalRead(pin));
        }
        finished();
        break; 
      }

      //Write Pin
      //Format: W<pin><'H' or 'L'>
    case 'W' : 
      {
        //Pin
        if(isDigit(MainSerial.peek()))
        {
          uint8_t pin = MainSerial.parseInt();
          DEBUG_PRINT("Writing to pin : ", pin);
          pinMode(pin,OUTPUT);
          switch (MainSerial.read()) {
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
      /**
    case 'H' : 
      {
        #ifdef BOARD_ARCHIM
        using namespace archim;
        #endif
        #ifdef BOARD_RAMBO
        using namespace rambo;
        #endif
        if(isDigit(MainSerial.peek()))
        {
          stepFrequency = MainSerial.parseInt();
          DEBUG_PRINT("Homing at step frequency (hz) : ", stepFrequency);
          period = 1000000/stepFrequency;
          DEBUG_PRINT("Stepping every (us) : ", period);
          portEnable(1);
          stepsToHome = 0;
          lastMicros = micros();

          //if we are already at the endstop move upwards until we are not
          if(digitalRead(ENDSTOP_PIN)){
            board->portDirection(UP);
            while(digitalRead(ENDSTOP_PIN) || stepsToHome <=1000){
              if(!digitalRead(ENDSTOP_PIN)) stepsToHome++;
              if ((micros()-lastMicros) >= period) 
              { 
                portStep(); 
                lastMicros = micros();
              } 
            }
          }
          stepsToHome =0;
          portDirection(DOWN);
          while(!digitalRead(ENDSTOP_PIN)){
            if ((micros()-lastMicros) >= period) 
            { 
              portStep(); 
              lastMicros = micros();
              stepsToHome++;
            }
          }
          MainSerial.println(stepsToHome);
        }
        portEnable(0);
        finished();
        break;
      }
      */

      //Set Microsteps
      //Format: U<microsteps>
    case 'U' : 
      {
        if(isDigit(MainSerial.peek()))
        {
          uint8_t pin = MainSerial.parseInt();
          DEBUG_PRINT("setting microsteps : ", pin);
          board::portSetMicroSteps(pin);
        }
        finished();
        break;
      }


      //Monitor Stepper test
      //Format: M<pin to watch>F<frequency>
      /*
    case 'M' : 
      {
        if(isDigit(MainSerial.peek()))
        {
          pin = MainSerial.parseInt();
          DEBUG_PRINT("Watching pin : ", pin);
          if (MainSerial.peek() == 'F')
          {
            sampleFrequency = MainSerial.parseInt();
            DEBUG_PRINT("Monitoring at frequency (hz) : ", sampleFrequency);
            period = 1000000/sampleFrequency;
            DEBUG_PRINT("Monitoring every (us) : ", period);
            testStart = millis();
            uint8_t lastPortSample = B11111100;
            for(i=0; i<=4; i++)
            { 
              posCounter[i] = 0; 
              consecutiveReads[i] = 0; 
            }
            for(i=0; i<=4; i++)
            {
              for(j = 0; j <= 4; j++) 
                stepperCount[i][j] = 0;
            }
            pinMode(pin,INPUT);
            digitalWrite(pin,HIGH);
            while(digitalRead(pin));
            lastMicros = micros();
            while(!digitalRead(pin)){
              if ((micros()-lastMicros) >= period) 
              { 
                lastMicros = micros();
                uint8_t sample = PINJ & B11111100;
                for(i=0; i<=4; i++){
                  if(((lastPortSample ^ sample) & (B00000100<<i)) && consecutiveReads[i] >= DEBOUNCE){
                    posCounter[i]++;
                    consecutiveReads[i] = 0;
                  }
                  stepperCount[i][posCounter[i]]++;
                  consecutiveReads[i]++;
                }

                lastPortSample = sample;
              }
            }
            DEBUG_PRINT("Ending Test", "");
            for(i=0; i<=4; i++){
              for(j = 0; j <= 4; j++){
                if(j==0){
                  MainSerial.print("{");
                  MainSerial.print(stepperCount[i][j]);
                  MainSerial.print(",");
                }
                else if(j==4){
                  MainSerial.print(stepperCount[i][j]);
                  MainSerial.println("}");
                }
                else {
                  MainSerial.print(stepperCount[i][j]);
                  MainSerial.print(",");
                }
              }
            }
            pin = -1;
          }
        }
        finished();
        break;
      }
      */

      //Write PWM
      //Format: P<pin>D<duty cycle>
    case 'P' : 
      {
        if(isDigit(MainSerial.peek()))
        {
          uint8_t pin = MainSerial.parseInt();
          DEBUG_PRINT("Analog write pin : ", pin);
          if(MainSerial.peek() == 'D')
          {
            uint8_t dutyCycle = MainSerial.parseInt();
            DEBUG_PRINT("Analog write duty cycle : ", dutyCycle);
            pinMode(pin,OUTPUT); 
            analogWrite(pin,dutyCycle);
          }
        }
        finished();
        break;
      }

      //Clamp board
      //Format: C<steps>F<frequency><Direction - D or U>P<pin to signal>
    case 'C' : 
      {
        unsigned int period;
        int stepFrequency;
        int8_t pin;

        if(isDigit(MainSerial.peek()))
        {
          int stepCount = MainSerial.parseInt();
          DEBUG_PRINT("Clamping steps : ", stepCount);
          if (MainSerial.peek() == 'F')
          {
            stepFrequency = MainSerial.parseInt();
            DEBUG_PRINT("Clamp step frequency (hz) : ", stepFrequency);
            period = 1000000/stepFrequency;
            DEBUG_PRINT("Stepping every (us) : ", period);
            board::portDirection(UP);
            switch (MainSerial.read()) {
              //Up
            case 'U' : 
              {
                DEBUG_PRINT("Going up.",' ');
                board::portDirection(UP);
                break;
              }
              //Down
            case 'D' : 
              {
                DEBUG_PRINT("Going down.",' ');
                board::portDirection(DOWN);
                break;
              }
            }
            board::portEnable(1);
            stepsToHome = 0;
            if(MainSerial.peek() == 'P'){
              pin = MainSerial.parseInt();
              pinMode(pin,OUTPUT);
              digitalWrite(pin,HIGH);
              delay(20);
            }
            lastMicros = micros();
            while(stepsToHome <= stepCount){
              if ((micros()-lastMicros) >= period) 
              { 
                board::portStep(); 
                lastMicros = micros();
                stepsToHome++;
              }
            }
            if(pin != -1){
              delay(20);
              digitalWrite(pin, LOW);
              pin = -1;
            }
          }
        }
        finished();
        break;
      }

    default :
      break;
    }
  }
}

/*
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
*/

void finished(void){
  MainSerial.println("ok");
}









