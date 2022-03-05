/*
 * log.c
 *
 *  Created on: 2022/03/05
 *      Author: sssho
 */
#include"System/log.h"
#include"System/state.h"

LOG_Typedef log[MAX_LOG];

void storeLog(uint16_t number) {
	uint16_t count;
	if(number > MAX_LOG - 1)	count = MAX_LOG - 1;
	else									count = number;

	log[count].target_velocity = target.velocity;
	log[count].target_omega = target.omega;
	log[count].real_velocity = mouse.velocity;
	log[count].real_omega = mouse.omega;
	log[count].mileage = mouse.mileage;
	log[count].angle = mouse.angle;

}
