#pragma once

#include "Arduino.h"

#include <stdint.h>

float nanoFaradsOnPin(uint8_t signalPin, float resistorValue);

uint32_t readDigitalPullupRiseTime(uint8_t signalPin);
