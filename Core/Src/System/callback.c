/*
 * callback.c
 *
 *  Created on: 2021/04/21
 *      Author: sssho
 */

#include "System/callback.h"
#include"Hardware/interface_LED.h"

extern uint16_t tim_counter;

void callbackTIM(void){
	static uint16_t l_state;

	tim_counter++;

	if(tim_counter > 3000) 	l_state = 0;
	else								l_state = 1;

	setLED5State(l_state);

}
