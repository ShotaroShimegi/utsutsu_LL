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
#define 	ROT_ANGLE_R180 -180.0f
//目標距離
#define SET_MM 62.0f
#define FIX_MM 40.0f
#define HALF_MM 90.0f
#define ONE_MM 2.0f*HALF_MM

void driveAccelMotion(float dist, float out_velocity,uint8_t wall_ctrl_flag,uint8_t wall_check);

void driveTrapezoidal(float distance,float vel_max,float vel_min, float accel);
void driveSlalomFree(float theta, float omega_max, float omega_accel);

void spinMotion(float angle);
void slalomMotion(float angle_deg, float omega, float omega_accel);

uint8_t moveHalfSectionAccel(uint8_t wall_ctrl_flag,uint8_t wall_read_flag);
void moveHalfSectionDecel(uint8_t wall_ctrl_flag);
uint8_t moveOneSectionAccel(uint8_t wall_ctrl);
void keepDistanceFromWall(void);
void spinRight180(void);
uint8_t moveSlalomR90(void);
uint8_t moveSlalomL90(void);
void backMotion(float dist);
// 大回りターン
void moveBigSlalomR90() ;
void moveBigSlalomL90() ;
void moveBigSlalomR180() ;
void moveBigSlalomL180() ;
// 斜めターン
void moveSlalomR45in();
void moveSlalomR45out();
void moveSlalomL45in();
void moveSlalomL45out();
void moveSlalomRV90out();
void moveSlalomLV90in();
void moveSlalomR135in();
void moveSlalomR135out();
void moveSlalomL135in();
void moveSlalomL135out();

#endif /* INC_SYSTEM_MOTION_H_ */
