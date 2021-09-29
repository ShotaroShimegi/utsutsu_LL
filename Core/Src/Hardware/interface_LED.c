/*
 * interface_LED.c
 *
 *  Created on: 2021/07/17
 *      Author: sssho
 */

#include"Hardware/interface_LED.h"
#include"main.h"

/**
* @brief
* @param argument1
* @param (As many as the number of arguments)
* @return
* (@sa Functions to should refer to)
* @details
*/

void setLED1State(uint8_t state)	{
	if (state == 0) LED1_GPIO_Port->BSRR = LED1_Pin;
	else			LED1_GPIO_Port->BSRR = (LED1_Pin << 16);
}
void setLED2State(uint8_t state)	{
	if (state == 0) LED2_GPIO_Port->BSRR = LED2_Pin;
	else			LED2_GPIO_Port->BSRR = (LED2_Pin << 16);
}
void setLED3State(uint8_t state)	{
	if (state == 0) LED3_GPIO_Port->BSRR = LED3_Pin;
	else			LED3_GPIO_Port->BSRR = (LED3_Pin << 16);
}
void setLED4State(uint8_t state)	{
	if (state == 0) LED4_GPIO_Port->BSRR = LED4_Pin;
	else			LED4_GPIO_Port->BSRR = (LED4_Pin << 16);
}
void setLED5State(uint8_t state)	{
	if (state == 0) LED5_GPIO_Port->BSRR = LED5_Pin;
	else			LED5_GPIO_Port->BSRR = (LED5_Pin << 16);
}
