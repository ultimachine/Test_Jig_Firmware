#include "pins.h"
#include "rambo.h"
#include "minirambo.h"
#include "einsyrambo.h"
#include "archim.h"
#include <SPI.h>

#ifdef SDHSMCI_SUPPORT
  #include <SD_HSMCI.h>
#endif

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

#if defined( BOARD_ARCHIM )
namespace board = archim;
#elif defined( BOARD_RAMBO )
namespace board = rambo;
#elif defined( BOARD_MINIRAMBO )
namespace board = minirambo;
#elif defined( BOARD_EINSYRAMBO )
namespace board = einsyrambo;
#endif


//int sampleFrequency;

//unsigned int period;
//unsigned long testStart;
//uint8_t i, j;
//uint8_t posCounter[5] = { 0,0,0,0,0 };
//uint8_t consecutiveReads[5] = { 0,0,0,0,0 };
byte startReads = 0;
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

#ifdef BOARD_ARCHIM
  while(!MainSerial) { }
#endif

  MainSerial.println("1");
}

void finished(void){
  MainSerial.println("ok");
}

void loop()
{
  unsigned long stepsToHome = 0;
  unsigned long lastMicros;
  uint8_t current;

  //uint8_t a= PINJ;
  //MainSerial.print(PINJ,BIN);
  //MainSerial.println("");
  #ifdef START_PIN
  static unsigned long startMillis = millis();
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

      //Set Motor Current
      //Format: V<255>
    case 'V' :
      {
        if(isDigit(Serial.peek()))
        {
          current = Serial.parseInt();
          DEBUG_PRINT("setting motor current: ", current);
          board::setMotorCurrent(current);
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
       //SPI FLASH INIT - RETURNS MFG DEVICE ID: 20
      //Format: S
    case 'S' :
      if(isDigit(MainSerial.peek()))
      {
        byte data = MainSerial.parseInt();
        spiflash_write(data); //does a write then reads out the result
      } else {
        spiflash_init();
      }
      finished();
      break;

      // Test SD Card
    case 'D' :
      board::sdinit();
      finished();
      break;

#ifdef X_TMC2130_CS
    // Trinamic TMC2130 Set Diags Low Debug Test
    case 'Z' :
      diag0_low();
      finished();
      break;

      // Trinamic TMC2130 Set Diag State
      // T0 diag0 push_pull
      // T1 diag1_push_pull
      // T2 all diags active low
    case 'T' :
      {
      int diag_state=2;
      uint8_t cs[4] = { X_TMC2130_CS, Y_TMC2130_CS, Z_TMC2130_CS, E0_TMC2130_CS };
      if( isDigit( MainSerial.peek() ) ) diag_state = MainSerial.parseInt();

      switch  ( diag_state ) {
       case 0: // T0 diag0 push_pull
         //MainSerial.println("diagstate: 0");
         for(int i=0;i<4;i++) tmc2130_write(cs[i],0x0,0,0,0b00010000,0b10000000); //gconf address=0x0, bit12_diag0_int_pushpull bit7_diag0_stall
         break;
       case 1: // T1 diag1_push_pull
         //MainSerial.println("diagstate: 1");
         for(int i=0;i<4;i++) tmc2130_write(cs[i],0x0,0,0,0b00100001,0); //gconf address=0x0, bit13_diag1_pushpull bit8_diag1_stall
         break;
       case 2: // T2 all diags active low
         //MainSerial.println("diagstate: 2");
         for(int i=0;i<4;i++) tmc2130_write(cs[i],0x0,0,0,0,1); //address=0x0 GCONF EXT VREF
         break;
      }
      finished();
      break;
    }
#endif //X_TMC2130_CS
    default :
      break;
    }
  }
}

void diag0_low()
{
  #ifdef X_TMC2130_CS
  uint8_t cs[4] = { X_TMC2130_CS, Y_TMC2130_CS, Z_TMC2130_CS, E0_TMC2130_CS };
  for(int i=0;i<4;i++)
  {
    tmc2130_write(cs[i],0x0,0,0,0b00010000,0b10000000); //gconf address=0x0, bit12_diag0_int_pushpull bit7_diag0_stall
  }
  #endif
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
