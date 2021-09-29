/*
 * motor.h
 *
 *  Created on: Jul 25, 2021
 *      Author: sssho
 */

#ifndef INC_SYSTEM_MOTOR_H_
#define INC_SYSTEM_MOTOR_H_

#include<stdint.h>

void initMotors(void);
void setMotorDirection(float r_duty, float l_duty);
float dutyConverter(float max, float min, float check_val);
uint8_t driveMotors(float r_duty, float l_duty);
uint8_t shutdownMotors(void);

#endif /* INC_SYSTEM_MOTOR_H_ */
