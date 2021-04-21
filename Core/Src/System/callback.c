/*
 * callback.c
 *
 *  Created on: 2021/04/21
 *      Author: sssho
 */
#include "Peripheral/wrap_gpio.h"
#include "System/callback.h"

void callbackTIM(void){
	changeLED1Status(1);
}
