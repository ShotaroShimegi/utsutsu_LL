/*
 * motion.h
 *
 *  Created on: Feb 21, 2022
 *      Author: sssho
 */

#ifndef INC_SYSTEM_MOTION_H_
#define INC_SYSTEM_MOTION_H_

void driveAccelMotion(float dist, float out_velocity);
void spinMotion(float angle);
void slalomMotion(float angle);

#endif /* INC_SYSTEM_MOTION_H_ */
