/*
 * ICM20689.c
 *
 *  Created on: 2021/07/17
 *      Author: sssho
 */
#include"Hardware/ICM20689.h"
#include"main.h"

void changeLeftStatus(uint8_t state){
	if (state == 0) Left_GPIO_Port->BSRR = Left_Pin;
	else			Left_GPIO_Port->BSRR = (Left_Pin << 16);
}
