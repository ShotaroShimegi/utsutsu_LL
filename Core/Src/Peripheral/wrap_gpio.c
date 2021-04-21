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
void changeLED2State(uint8_t state)	{
	if (state == 0) LED2_GPIO_Port->BSRR = LED2_Pin;
	else			LED2_GPIO_Port->BSRR = (LED2_Pin << 16);
}
void changeLED3State(uint8_t state)	{
	if (state == 0) LED3_GPIO_Port->BSRR = LED3_Pin;
	else			LED3_GPIO_Port->BSRR = (LED3_Pin << 16);
}
void changeLED4State(uint8_t state)	{
	if (state == 0) LED4_GPIO_Port->BSRR = LED4_Pin;
	else			LED4_GPIO_Port->BSRR = (LED4_Pin << 16);
}
void changeLED5State(uint8_t state)	{
	if (state == 0) LED5_GPIO_Port->BSRR = LED5_Pin;
	else			LED5_GPIO_Port->BSRR = (LED5_Pin << 16);
}

void changeFrontCenterStatus(uint8_t state){
	if (state == 0) FrontCenter_GPIO_Port->BSRR = FrontCenter_Pin;
	else			FrontCenter_GPIO_Port->BSRR = (FrontCenter_Pin << 16);
}
void changeFrontRightStatus(uint8_t state){
	if (state == 0) FrontRight_GPIO_Port->BSRR = FrontRight_Pin;
	else			FrontRight_GPIO_Port->BSRR = (FrontRight_Pin << 16);
}
void changeFrontLeftStatus(uint8_t state){
	if (state == 0) FrontLeft_GPIO_Port->BSRR = FrontLeft_Pin;
	else			FrontLeft_GPIO_Port->BSRR = (FrontLeft_Pin << 16);
}
void changeRightStatus(uint8_t state){
	if (state == 0) Right_GPIO_Port->BSRR = Right_Pin;
	else			Right_GPIO_Port->BSRR = (Right_Pin<< 16);
}
void changeLeftStatus(uint8_t state){
	if (state == 0) Left_GPIO_Port->BSRR = Left_Pin;
	else			Left_GPIO_Port->BSRR = (Left_Pin << 16);
}
