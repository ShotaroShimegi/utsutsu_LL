/*
 * motion.c
 *
 *  Created on: Feb 21, 2022
 *      Author: sssho
 */
#include<math.h>

#include"Controller/state.h"

#include "System/motion.h"
#include "System/sensing.h"
#include "System/flags.h"

#include"Hardware/basic_timer.h"
#include"Hardware/interface_LED.h"

/**
* @brief 加減速して前進移動
* @param1 直進距離 [mm]
* @param2 出口並進速度 [m/s], 止まりたいなら0.0f
* @param3 壁制御の有無
*/
void driveAccelMotion(float dist, float out_velocity,uint8_t wall_ctrl_flag)
{
	float input_mileage = mouse.mileage;
	//速度制御、角度制御、壁制御ON
	setControlFlags(1, 1, 0, wall_ctrl_flag & 0x01);

	//減速区間の計算
	float error = max.velocity - out_velocity;
	float offset = error*error *0.50f / max.accel * 1000;

	//引数があれな場合の例外処理
	if(dist < offset)	offset = dist * 0.50f;
	out_velocity = fabs(out_velocity);

	//加速して減速
	setAccelFlags(1, 0, 0, 0);
	while(mouse.mileage < input_mileage + dist - offset);
	if(out_velocity != max.velocity){
		setAccelFlags(0, 1, 0, 0);
		while(mouse.mileage < input_mileage +dist) {
			//出口速度に速度が到達したら強制終了
			if(target.velocity <= out_velocity){
				target.velocity = out_velocity;
				break;
		}
	}
}
	setAccelFlags(0, 0, 0, 0);
	setControlFlags(1, 0, 0, 0);
	target.velocity = out_velocity;
	if(out_velocity == 0.0f)	waitMs(200);
}

/**
* @brief その場で超信地旋回する
* @param1 回転角度 [deg], 正方向はCCW
*/
void spinMotion(float angle_deg)
{
	float input_angle = mouse.angle;

	//角速度制御、ON
	setControlFlags(1, 1, 0, 0);

	//減速区間の計算
	float offset = max.omega*max.omega *0.50f
						/ max.omega_accel * CONVERT_TO_DEG;

	//引数があれな場合の例外処理
	if(fabs(angle_deg) < offset)	offset =  fabs(angle_deg) * 0.50f;
	target.velocity = 0.0f;
	target.omega = 0.0f;

	//加速して減速
	if(angle_deg > 0){
		setAccelFlags(0, 0, 1, 0);
		while(mouse.angle < input_angle + angle_deg - offset);
		setAccelFlags(0, 0, 0, 1);
		while(mouse.angle < input_angle + angle_deg) {
			if(target.omega <= 0.0f){
				target.omega = 0.0f;
				break;
			}
		}
	}else{
		setAccelFlags(0, 0, 0, 1);
		while(mouse.angle > input_angle + angle_deg + offset);
		setAccelFlags(0, 0, 1, 0);
		while(mouse.angle > input_angle + angle_deg) {
			if(target.omega >= 0.0f){
				target.omega = 0.0f;
				break;
			}
		}
	}
	setAccelFlags(0, 0, 0, 0);
	target.omega = 0.0f;
	target.angle += angle_deg;
}

void slalomMotion(float angle_deg)
{
	float input_angle = mouse.angle;

	//角速度制御、ON
	setControlFlags(1, 1, 0, 0);

	//減速区間の計算
	float offset = max.omega*max.omega *0.50f
						/ max.omega_accel * CONVERT_TO_DEG;

	//引数があれな場合の例外処理
	if(fabs(angle_deg) < offset)	offset =  fabs(angle_deg) * 0.50f;
	target.omega = 0.0f;

	//加速して減速
	if(angle_deg > 0){
		setAccelFlags(1, 0, 1, 0);
		while(mouse.angle < input_angle + angle_deg - offset);
		setAccelFlags(1, 0, 0, 1);
		while(mouse.angle < input_angle + angle_deg) {
			if(target.omega <= 0.0f){
				target.omega = 0.0f;
				break;
			}
		}
	}else{
		setAccelFlags(1, 0, 0, 1);
		while(mouse.angle > input_angle + angle_deg + offset);
		setAccelFlags(1, 0, 1, 0);
		while(mouse.angle > input_angle + angle_deg) {
			if(target.omega >= 0.0f){
				target.omega = 0.0f;
				break;
			}
		}
	}
	setAccelFlags(1, 0, 0, 0);
	target.omega = 0.0f;
	target.angle += angle_deg;
}

uint8_t moveHalfSectionAccel(uint8_t wall_ctrl_flag,uint8_t wall_read_flag)
{
	driveAccelMotion(90.0f, max.velocity, wall_ctrl_flag);
	if(wall_read_flag & 0x01)		return getWallInfo();
	else									return 0;
}

void moveHalfSectionDecel(uint8_t wall_ctrl_flag)
{
	driveAccelMotion(90.0f, 0.0f, wall_ctrl_flag);
}

uint8_t  moveOneSectionAccel(uint8_t wall_ctrl)
{
	uint8_t wall_info = 0x00;
	moveHalfSectionAccel(wall_ctrl & 0x01, OFF);
	wall_info = moveHalfSectionAccel(wall_ctrl & 0x01,ON);
	return wall_info;
}

void fixPostureByWallSensor(void)
{
	MF.FLAG.FRONT = 1;
	setControlFlags(OFF, OFF, OFF, OFF);
	//todo 前壁制御関連
	waitMs(500);
	MF.FLAG.FRONT = 0;
	setControlFlags(ON, ON, OFF, OFF);
	waitMs(200);
	setControlFlags(OFF, OFF, OFF, OFF);
}

void spinRight180(void)
{
	spinMotion(ROT_ANGLE_180);
}

uint8_t moveSlalomR90(void)
{
	uint8_t wall_info = 0x00;
//	driveAccelMotion(30.0f, max.velocity, OFF);
	slalomMotion(ROT_ANGLE_R90);
	driveAccelMotion(20.0f, max.velocity, OFF);
	wall_info = getWallInfo();
	return wall_info;
}

uint8_t moveSlalomL90(void)
{
	uint8_t wall_info = 0x00;
//	driveAccelMotion(30.0f, max.velocity, OFF);
	slalomMotion(ROT_ANGLE_L90);
	driveAccelMotion(20.0f, max.velocity, OFF);
	wall_info = getWallInfo();
	return wall_info;
}

void backMotion(float dist)
{
	float input_mileage = mouse.mileage;
	//速度制御、角度制御、壁制御ON
	setControlFlags(1, 1, 0, 0);

	//減速区間の計算
	float offset = dist * 0.5f;

	//加速して減速
	setAccelFlags(0, 1, 0, 0);
	while(mouse.mileage > input_mileage - dist + offset);
	setAccelFlags(1, 0, 0, 0);
	while(mouse.mileage > input_mileage - dist) {
		//出口速度に速度が到達したら強制終了
		if(target.velocity >= 0.0f){
			target.velocity = 0.0f;
			break;
		}
	}
	setAccelFlags(0, 0, 0, 0);
	setControlFlags(1, 0, 0, 0);
	target.velocity = 0.0f;
	waitMs(200);
}
