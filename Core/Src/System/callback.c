/*
 * callback.c
 *
 *  Created on: 2021/04/21
 *      Author: sssho
 */
#include"Controller/mode.h"

#include "System/callback.h"
#include"System/sensing.h"
#include"System/state.h"

#include"Hardware/interface_LED.h"

extern uint16_t tim_counter;

void callbackTIM(void)
{
	updateSensors();
	updateStatus();
	judgeFailSafe();
	tim_counter++;

}
