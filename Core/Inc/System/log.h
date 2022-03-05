/*
 * log.h
 *
 *  Created on: 2022/03/05
 *      Author: sssho
 */

#include<stdint.h>

#ifndef INC_SYSTEM_LOG_H_
#define INC_SYSTEM_LOG_H_

#define MAX_LOG	4000

typedef struct{
	float target_velocity;
	float target_omega;
	float real_velocity;
	float real_omega;
	float mileage;
	float angle;
}LOG_Typedef;

void storeLog(uint16_t number);

extern LOG_Typedef log[4000];


#endif /* INC_SYSTEM_LOG_H_ */
