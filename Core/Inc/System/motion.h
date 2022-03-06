/*
 * motion.h
 *
 *  Created on: Feb 21, 2022
 *      Author: sssho
 */

#ifndef INC_SYSTEM_MOTION_H_
#define INC_SYSTEM_MOTION_H_

#define ROT_ANGLE_R90 -90.0f			//Target Angle [degree]
#define	ROT_ANGLE_L90 90.0f
#define ROT_ANGLE_180 180.0f
#define SET_MM 62.0f

#define HALF_MM 90.0f
#define ONE_MM 2.0f*HALF_MM

void driveAccelMotion(float dist, float out_velocity);
void spinMotion(float angle);
void slalomMotion(float angle);

#endif /* INC_SYSTEM_MOTION_H_ */
