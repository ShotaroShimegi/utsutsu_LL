/*
 * motion.c
 *
 *  Created on: Feb 21, 2022
 *      Author: sssho
 */
#include<math.h>

#include"System/motion.h"
#include"System/state.h"

void driveMotion(float dist, float out_velocity)
{
	float input_mileage = mouse.mileage;
	//速度制御、角度制御、壁制御ON
	MF.FLAG.VCTRL = 1;
	MF.FLAG.ACTRL = 1;
	MF.FLAG.WCTRL = 0;
	MF.FLAG.CTRL = 1;

	//減速区間の計算
	float error = max.velocity - out_velocity;
	float offset = error*error *0.50f / max.accel;

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
		if(mouse.velocity <= out_velocity)break;
	}

}

void turnMotion(float angle)
{

}

void slalomMotion(float angle, float out_velocity)
{

}
