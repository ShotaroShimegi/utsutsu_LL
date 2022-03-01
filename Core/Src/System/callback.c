/*
 * callback.c
 *
 *  Created on: 2021/04/21
 *      Author: sssho
 */

#include"Hardware/interface_LED.h"

#include "System/callback.h"
#include"System/sensing.h"
#include"System/state.h"

extern uint16_t tim_counter;

void callbackTIM(void)
{
	updateSensors();
	updateStatus();

	tim_counter++;

}
