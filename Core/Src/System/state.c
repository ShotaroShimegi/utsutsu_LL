/*
 * state.c
 *
 *  Created on: Feb 21, 2022
 *      Author: sssho
 */

#include"Hardware/motor.h"

#include"System/state.h"
#include"System/map.h"
#include"System/sensing.h"

State_Typedef mouse;
State_Typedef target;
State_Typedef max;
Flags_Typedef MF;
PID_Typedef PID_left_velocity;
PID_Typedef PID_right_velocity;
PID_Typedef PID_wall;
PID_Typedef PID_omega;
PID_Typedef PID_angle;
float d_out_r;
float d_out_l;

/**
 * calculatePID
* @brief PID構造体の計算をし、出力を行う。errorは入力されている前提
* @param instance  計算したい項目用のPIDパラメータ
* @return 出力値
*/
float calculatePID(PID_Typedef *instance)
{
	instance->outputI += instance->gainI*instance->error;
	if(instance->outputI > instance->LimitI)			instance->	outputI = instance->LimitI;
	else if(instance->outputI < - instance->LimitI)  instance->	outputI = - instance->LimitI;

	instance->outputPID = instance->gainP * instance->error
			+ instance->gainD*(instance->error - instance->pre_error)
			+ instance->outputI;

	if(instance->outputPID > instance->LimitPID)			instance->	outputPID = instance->LimitPID;
	else if(instance->outputPID < - instance->LimitPID)  	instance->	outputPID = - instance->LimitPID;

	instance->pre_error = instance->error;

	return instance->outputPID;

}

float calculateTargetVelocity(void)
{
	float target_val = target.velocity;

	if(MF.FLAG.ACCEL == 1) 			target_val += target.accel;
	else if(MF.FLAG.DECEL == 1)	target_val -= target.accel;

	if(target_val > max.velocity)			target_val = max.velocity;
	else if(target_val < -max.velocity)	target_val = -max.velocity;

	return target_val;
}

float calculateTagetOmega(void)
{
	float target_val = target.omega;

	if(MF.FLAG.WACCEL == 1) 			target_val += target.omega_accel;
	else if(MF.FLAG.WDECEL == 1)	target_val -= target.omega_accel;

	if(target_val > max.omega)			target_val = max.omega;
	else if(target_val < -max.omega)	target_val = -max.omega;

	return target_val;
}

State_Typedef setStatus(float angle,float omega, float omega_accel,
									float mileage, float velocity, float accel,
									uint8_t x, uint8_t y, uint8_t dir )
{
	State_Typedef instance;
	instance.angle = angle;
	instance.omega = omega;
	instance.omega_accel = omega_accel;
	instance.mileage = mileage;
	instance.velocity = velocity;
	instance.accel = accel;
	instance.x = x;
	instance.y = y;
	instance.dir = dir;

	return instance;
}

PID_Typedef setPrameters(float gainP, float gainI, float gainD, float limitI, float limitPID)
{
	PID_Typedef instance;
	instance.gainP = gainP;
	instance.gainI = gainI;
	instance.gainD = gainD;
	instance.LimitI = limitI;
	instance.LimitPID = limitPID;
	return instance;
}


/**
 * initStatus
* @brief 初期化関連の実行関数
* @return 出力値
*/
void initMouseStatus(void)
{
	//State関連
	mouse = setStatus(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0x00);
	target = setStatus(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0x00);
	max = setStatus(0.0f, 0.0f, 0.0f, 0.0f, 0.50f, 0.40f, GOAL_X, GOAL_Y, 0x00);

	//MF
	MF.FLAGS = 0x00000000;

	//PID関連
	PID_left_velocity = setPrameters(3.5f, 0.01f, 0.0f, 0.1f, 0.6f);
	PID_right_velocity = setPrameters(3.5f, 0.01f, 0.0f, 0.1f, 0.6f);
	PID_wall = setPrameters(0.30f, 0.0f, 0.00f, 0.00f, 0.2f);
	PID_omega = setPrameters(0.06f, 0.002f, 0.0f, 0.1f, 0.2f);
	PID_angle = setPrameters(0.10f, 0.0f, 0.02f, 0.1f, 0.2f);
}

void updateStatus(void)
{
	//出力用の変数
	float output_duty_r = 0.0f;
	float output_duty_l = 0.0f;
	float tmp = 0.0f;

	//センサ値からステータス
	mouse.angle = addAngle(mouse.angle);
	mouse.omega = getOmega()*CONVERT_TO_RAD;
	mouse.mileage = getCenterMileage();
	mouse.velocity = getCenterVelocity();

	//目標値生成
	target.velocity = calculateTargetVelocity();
	target.omega = calculateTagetOmega();

	//偏差計算
	PID_angle.error = target.angle - mouse.angle;
	PID_left_velocity.error = target.velocity - mouse.velocity;
	PID_right_velocity.error = target.velocity - mouse.velocity;
	PID_omega.error = target.omega - mouse.omega;
	PID_wall.error = (sensor.wall_r - sensor.wall_r_offset) - (sensor.wall_l - sensor.wall_l_offset);

	//PID計算
	if(MF.FLAG.ACTRL)	{
		tmp = calculatePID(&PID_angle);
		output_duty_r -= tmp;
		output_duty_l += tmp;
	}
	if(MF.FLAG.WCTRL){
		tmp = calculatePID(&PID_omega);
		output_duty_r -= tmp;
		output_duty_l += tmp;
	}
	if(MF.FLAG.CTRL){
		output_duty_r -= calculatePID(&PID_wall);
		output_duty_l += calculatePID(&PID_wall);
	}
	output_duty_r += calculatePID(&PID_right_velocity);
	output_duty_l += calculatePID(&PID_left_velocity);

	driveMotors(output_duty_l, output_duty_r);

	d_out_l = output_duty_l;
	d_out_r = output_duty_r;

}
