/*
 * wrap_gpio.c
 *
 *  Created on: 2021/03/21
 *      Author: sssho
 */

#include <stdint.h>
#include "main.h"
#include "Peripheral/wrap_gpio.h"

void changeLED1State(uint8_t state)	{
	if (state == 0) LED1_GPIO_Port->BSRR = LED1_Pin;
	else			LED1_GPIO_Port->BSRR = (LED1_Pin << 16);
}


