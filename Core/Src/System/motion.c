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
	//速度制御、角度制御、壁制御
	setControlFlags(1, 1, 0, wall_ctrl_flag & 0x01);
	// 目標値入力
	target.angle = mouse.angle;
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
	}else {
		while(mouse.mileage < input_mileage + dist){
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
* @brief 汎用直進関数
* @param1 直進距離 [mm]
* @param2 許容する最大速度、距離と加速度から本当にここまで出すかは計算される
* @param3 出口並進速度 [m/s], 止まりたいなら0.0f、ただし距離が短すぎるとこの速度で走り続ける
* @param4 加速度、ぶっちゃけ前の名残、消したい
*/
void driveTrapezoidal(float dist,float vel_max,float vel_min, float accel)
{
	float input_mileage = mouse.mileage;
	float vel_error = vel_max - vel_min;
	float offset;
	uint8_t max_flag = 0;

// オフセット計算
	offset = vel_error*vel_error *0.50f / max.accel * 1000;
	offset += 10;
	target.omega = 0;

	if(2.0f*offset > dist || 1.5f*ONE_MM > dist)	{
		//Change Max Speed
		max.velocity = vel_min;
		min.velocity = 0.0f;
	}else{
		max.velocity = vel_max;
		min.velocity = 0.0f;
		max_flag = 1;
	}

	// 速度制御、角速度制御、加速度ON
	setControlFlags(ON, ON, OFF, OFF);
	setAccelFlags(1, 0, 0, 0);

	//----Go Forward----
	if(dist >= 0.0f){
		while(mouse.mileage + offset < input_mileage + dist){
			if(target.velocity ==  max.velocity) {
				vel_error = target.velocity - vel_min;
				offset = 0.5f * 1000 * vel_error * vel_error / accel;
				offset += 1000 * vel_min * vel_error / accel;
				offset += 10;
			}
			if(sensor.wall_val[FF]> FRONT_WALL_CONTROL)	break;
		}
		if(max_flag == 1)	{
			min.velocity = vel_min;
			setAccelFlags(1, 0, 0, 0);
		}
		while(mouse.mileage < input_mileage  + dist){
			if(sensor.wall_val[FF] > FRONT_WALL_CONTROL)	break;
			if((max_flag == 1) &&( target.velocity == min.velocity)) break;
		}
	} else{
		while(mouse.mileage > input_mileage + dist);
	}
}

/**
* @brief 汎用スラローム関数、斜めにまで対応させる
* @param1 目標追加角度 [deg]
* @param2 出口並進速度 [m/s], 止まりたいなら0.0f
* @param3 壁制御の有無
*/
void driveSlalomFree(float theta, float omega_max, float omega_accel) {
	float input_angle = mouse.angle;
	float offset =  omega_max*omega_max*0.5 / omega_accel
					* CONVERT_TO_DEG;									// [rad/s]
	//====　目標値設定　====
	target.omega = 0.0f, target.omega_accel = omega_accel;
	max.omega = omega_max;

	// 速度制御、角速度制御、加速ON
	setControlFlags(ON, ON, OFF, OFF);

	if(theta > 0){				//Left Turn
		setAccelFlags(ON, OFF, ON, OFF);
		while(mouse.angle + offset < input_angle + theta);
		setAccelFlags(ON, OFF, OFF, ON);
		while(mouse.angle < input_angle + theta ){
			if(target.omega <= 0.0f) break;
		}
	} else if(theta < 0){		//Right Turn
		setAccelFlags(ON, OFF, OFF, ON);
		while(mouse.angle - offset > input_angle + theta);
		setAccelFlags(ON, OFF, ON, OFF);
		while(mouse.angle > input_angle + theta){
			if(target.omega >= 0.0f) break;
		}
	}
	target.angle += theta;
}

/*======================
* @brief 	その場で超信地旋回する
* @param 	回転角度 [deg], 正方向はCCW
=======================*/
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
			if(target.omega >= 0.0f) {
				target.omega = 0.0f;
				break;
			}
		}
	}
	setAccelFlags(0, 0, 0, 0);
	target.omega = 0.0f;
	target.angle += angle_deg;
}

void slalomMotion(float angle_deg) {
	float input_angle = mouse.angle;

	//角速度制御、ON
	setControlFlags(ON, ON, OFF, OFF);

	//減速区間の計算
	float offset = max.omega*max.omega *0.50f
						/ max.omega_accel * CONVERT_TO_DEG;

	//引数があれな場合の例外処理
	if(fabs(angle_deg) < offset)	offset =  fabs(angle_deg) * 0.50f;
	target.omega = 0.0f;

	//加速して減速
	if(angle_deg > 0){		// 左ターン
		setAccelFlags(1, 0, 1, 0);
		while(mouse.angle < input_angle + angle_deg - offset);
		setAccelFlags(1, 0, 0, 1);
		while(mouse.angle < input_angle + angle_deg) {
			if(target.omega <= 0.0f){
				target.omega = 0.0f;
				break;
			}
		}
	}else{					// 	右ターン
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

/*======================
 * keepDistanceFromWall()
* @brief 前壁からの距離を保つ位置制御を1秒間かける
=======================*/
void keepDistanceFromWall(void)
{
	MF.FLAG.FRONT = 1;
	setControlFlags(OFF, OFF, OFF, OFF);
	waitMs(1000);
	MF.FLAG.FRONT = 0;
	setControlFlags(ON, ON, OFF, OFF);
}

void spinRight180(void)
{
	spinMotion(ROT_ANGLE_R180);
	waitMs(100);
}

uint8_t moveSlalomR90(void)
{
	uint8_t wall_info = 0x00;
	MF.FLAG.SAFETY = OFF;
	driveAccelMotion(30.0f, max.velocity, OFF);
	slalomMotion(ROT_ANGLE_R90);
	MF.FLAG.SAFETY = ON;
	driveAccelMotion(20.0f, max.velocity, OFF);
	wall_info = getWallInfo();
	return wall_info;
}

uint8_t moveSlalomL90(void)
{
	uint8_t wall_info = 0x00;

	MF.FLAG.SAFETY = OFF;
	driveAccelMotion(30.0f, max.velocity, OFF);
	slalomMotion(ROT_ANGLE_L90);
	MF.FLAG.SAFETY = ON;

	driveAccelMotion(20.0f, max.velocity, OFF);
	wall_info = getWallInfo();
	return wall_info;
}

void backMotion(float dist)
{
	float input_mileage = mouse.mileage;
	float max_tmp = max.velocity;
	//速度制御、角度制御、壁制御OFF
	setControlFlags(1, 0, 0, 0);

	//減速区間の計算
	max.velocity = 0.20f;
	float offset = max.velocity*max.velocity *0.50f / max.accel * 1000;

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
	max.velocity = max_tmp;
	waitMs(500);
}
/*
void BigSlalomR90()
{
	MF.FLAG.CTRL = 1;
	DriveAccel(params_now.big90_before,FRONT_CONTROL_FLAG);
	MF.FLAG.CTRL = 0;
	DriveSlalomFree(-90,params_now.big90_omega_max, params_now.big90_omega_accel, big90_omega_accel_time);
	MF.FLAG.CTRL = 1;
	DriveAccel(params_now.big90_after,0);
}

void BigSlalomL90()
{
	MF.FLAG.CTRL = 1;
	DriveAccel(params_now.big90_before,FRONT_CONTROL_FLAG);
	MF.FLAG.CTRL = 0;
	DriveSlalomFree(90,params_now.big90_omega_max, params_now.big90_omega_accel, big90_omega_accel_time);
	MF.FLAG.CTRL = 1;
	DriveAccel(params_now.big90_after,0);
}

void BigSlalomR180()
{
	MF.FLAG.CTRL = 1;
	DriveAccel(params_now.big180_before,FRONT_CONTROL_FLAG);
	MF.FLAG.CTRL = 0;
	DriveSlalomFree(-180,params_now.big180_omega_max, params_now.big180_omega_accel, big180_omega_accel_time);
	MF.FLAG.CTRL = 1;
	DriveAccel(params_now.big180_after,0);
}

void BigSlalomL180()
{
	MF.FLAG.CTRL = 1;
	SetMotionDirection(FORWARD);
	DriveAccel(params_now.big180_before,FRONT_CONTROL_FLAG);
	MF.FLAG.CTRL = 0;
	DriveSlalomFree(180,params_now.big180_omega_max, params_now.big180_omega_accel, big180_omega_accel_time);
	MF.FLAG.CTRL = 1;
	DriveAccel(params_now.big180_after,0);
	GetWallData();
}
*/
