/*
 * motion.h
 *
 *  Created on: Feb 21, 2022
 *      Author: sssho
 */

#ifndef INC_SYSTEM_MOTION_H_
#define INC_SYSTEM_MOTION_H_

void driveMotion(float dist, float out_velocity);
void turnMotion(float angle);
void slalomMotion(float angle, float out_velocity);


#endif /* INC_SYSTEM_MOTION_H_ */
