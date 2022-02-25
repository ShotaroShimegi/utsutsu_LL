/*
 * motor.h
 *
 *  Created on: 2021/07/16
 *      Author: Shotaro Shimegi
 */

#ifndef INC_HARDWARE_MOTOR_H_
#define INC_HARDWARE_MOTOR_H_

#define MAX_DUTY 0.20f
#define MIN_DUTY 0.00f

void driveMotors(float r_duty, float l_duty);
void enableMotors(void);
void shutdownMotors(void);

#endif /* INC_HARDWARE_MOTOR_H_ */
