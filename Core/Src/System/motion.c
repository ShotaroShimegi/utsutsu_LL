/*
 * motion.c
 *
 *  Created on: Feb 21, 2022
 *      Author: sssho
 */
#include<math.h>
#include<stdint.h>

#include"Controller/state.h"

#include "System/motion.h"
#include "System/sensing.h"

#include"Hardware/basic_timer.h"

/**
* @brief 加減速して前進移動
* @param1 直進距離 [mm]
* @param2 出口並進速度 [m/s], 止まりたいなら0.0f
*/
void driveAccelMotion(float dist, float out_velocity)
{
	float input_mileage = mouse.mileage;
	//速度制御、角度制御、壁制御ON
	setControlFlags(1, 1, 0, 0);

	//減速区間の計算
	float error = max.velocity - out_velocity;
	float offset = error*error *0.50f / max.accel * 1000;

	//引数があれな場合の例外処理
	if(dist < offset)	offset = dist * 0.50f;
	out_velocity = fabs(out_velocity);

	//加速して減速
	setAccelFlags(1, 0, 0, 0);
	while(mouse.mileage < input_mileage +dist - offset);
	setAccelFlags(0, 1, 0, 0);
	while(mouse.mileage < input_mileage +dist) {
		//出口速度に速度が到達したら強制終了
		if(target.velocity <= out_velocity){
			target.velocity = out_velocity;
			break;
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
