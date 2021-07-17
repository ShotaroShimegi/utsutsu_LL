/*
 * wall_sensor.c
 *
 *  Created on: 2021/07/17
 *      Author: sssho
 */

#include"Hardware/wall_sensor.h"
#include"main.h"

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
