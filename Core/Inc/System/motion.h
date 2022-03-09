/*
 * motion.h
 *
 *  Created on: Feb 21, 2022
 *      Author: sssho
 */
#include<stdint.h>

#ifndef INC_SYSTEM_MOTION_H_
#define INC_SYSTEM_MOTION_H_

//目標角度
#define 	ROT_ANGLE_R90 -90.0f			// [deg]
#define		ROT_ANGLE_L90 90.0f
#define 	ROT_ANGLE_180 180.0f
//目標距離
#define SET_MM 62.0f
#define HALF_MM 90.0f
#define ONE_MM 2.0f*HALF_MM

void driveAccelMotion(float dist, float out_velocity,uint8_t wall_ctrl_flag);
void spinMotion(float angle);
void slalomMotion(float angle);

uint8_t moveHalfSectionAccel(uint8_t wall_ctrl_flag,uint8_t wall_read_flag);
void moveHalfSectionDecel(uint8_t wall_ctrl_flag);
uint8_t moveOneSectionAccel(uint8_t wall_ctrl);
void fixPostureByWallSensor(void);
void spinRight180(void);
uint8_t moveSlalomR90(void);
uint8_t moveSlalomL90(void);
void backMotion(float dist);

#endif /* INC_SYSTEM_MOTION_H_ */
