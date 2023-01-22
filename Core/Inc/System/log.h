/*
 * log.h
 *
 *  Created on: 2022/03/05
 *      Author: sssho
 */

#include<stdint.h>

#ifndef INC_SYSTEM_LOG_H_
#define INC_SYSTEM_LOG_H_

#define MAX_LOG	2000

typedef struct{
	float target_velocity;
	float target_omega;
	float target_accel;
	float real_velocity;
	float real_omega;
	float real_accel;
	float mileage;
	float angle;
	int16_t left_sensor;
	int16_t right_sensor;
	float l_duty;
	float r_duty;
}LOG_Typedef;

uint16_t storeLog(uint16_t number);

extern LOG_Typedef log[MAX_LOG];

#endif /* INC_SYSTEM_LOG_H_ */
