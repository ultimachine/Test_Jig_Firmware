#pragma once

#include <Arduino.h>

#define BOARD_ARCHIM2
//#define BOARD_ARCHIM
//#define BOARD_RAMBO
//#define BOARD_MINIRAMBO
//#define BOARD_EINSYRAMBO

#if defined ( BOARD_ARCHIM2 )
	#include "archim2.h"
#elif defined ( BOARD_ARCHIM )
	#include "archim.h"
#elif defined ( BOARD_RAMBO )
	#include "rambo.h"
#elif defined ( BOARD_MINIRAMBO )
	#include "minirambo.h"
#elif defined ( BOARD_EINSYRAMBO )
	#include "einsyrambo.h"
#endif
