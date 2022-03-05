/*
 * motion.c
 *
 *  Created on: Feb 21, 2022
 *      Author: sssho
 */
#include<math.h>
#include<stdint.h>

#include "System/motion.h"
#include "System/state.h"
#include "System/sensing.h"

#include"Hardware/basic_timer.h"

void driveAccelMotion(float dist, float out_velocity)
{
	float input_mileage = mouse.mileage;
	//速度制御、角度制御、壁制御ON
	MF.FLAG.VCTRL = 1;
	MF.FLAG.ACTRL = 0;
	MF.FLAG.WCTRL = 1;
	MF.FLAG.CTRL = 0;

	//減速区間の計算
	float error = max.velocity - out_velocity;
	float offset = error*error *0.50f / max.accel * 1000;

	//引数があれな場合の例外処理
	if(dist < offset)	offset = dist * 0.50f;
	out_velocity = fabs(out_velocity);

	//加速して減速
	MF.FLAG.ACCEL = 1;
	while(mouse.mileage < input_mileage +dist - offset);
	MF.FLAG.ACCEL = 0;
	MF.FLAG.DECEL = 1;
	while(mouse.mileage < input_mileage +dist) {
		//出口速度に速度が到達したら強制終了
		if(target.velocity <= out_velocity){
			target.velocity = out_velocity;
			break;
		}
	}
	MF.FLAG.ACCEL = 0;
	MF.FLAG.DECEL = 0;
	target.velocity = out_velocity;

	if(out_velocity == 0.0f)	waitMs(200);
}

void spinMotion(float angle_deg)
{
	float input_angle = mouse.angle;

	//角速度制御、ON
	MF.FLAG.VCTRL = 0;
	MF.FLAG.ACTRL = 0;
	MF.FLAG.WCTRL = 1;
	MF.FLAG.CTRL = 0;

	//減速区間の計算
	float offset = max.omega*max.omega *0.50f
						/ max.omega_accel * CONVERT_TO_DEG;

	//引数があれな場合の例外処理
	if(fabs(angle_deg) < offset)	offset =  fabs(angle_deg) * 0.50f;
	target.velocity = 0.0f;
	target.omega = 0.0f;

	//加速して減速
	if(angle_deg > 0){
		MF.FLAG.WACCEL = 1;
		while(mouse.angle < input_angle + angle_deg - offset);
		MF.FLAG.WACCEL = 0;
		MF.FLAG.WDECEL = 1;
		while(mouse.angle < input_angle + angle_deg) {
			if(target.omega <= 0.0f){
				target.omega = 0.0f;
				break;
			}
		}
	}else{
		MF.FLAG.WDECEL = 1;
		while(mouse.angle > input_angle + angle_deg + offset);
		MF.FLAG.WACCEL = 1;
		MF.FLAG.WDECEL = 0;
		while(mouse.angle > input_angle + angle_deg) {
			if(target.omega >= 0.0f){
				target.omega = 0.0f;
				break;
			}
		}
	}
	MF.FLAG.WACCEL = 0;
	MF.FLAG.WDECEL = 0;
	target.omega = 0.0f;
	target.angle += angle_deg;
}

void slalomMotion(float angle, float out_velocity)
{

}
