/*
 * state.c
 *
 *  Created on: Feb 21, 2022
 *      Author: sssho
 */
#include"Controller/state.h"

#include"System/map.h"
#include"System/sensing.h"
#include"System/flags.h"

#include"Hardware/motor.h"

#include<math.h>

State_Typedef mouse;
State_Typedef target;
State_Typedef max;

PID_Typedef PID_left_velocity;
PID_Typedef PID_right_velocity;
PID_Typedef PID_wall_side;
PID_Typedef PID_wall_front_posture;
PID_Typedef PID_wall_front_distance;
PID_Typedef PID_omega;
PID_Typedef PID_angle;

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
/**
 * calculateTargetVelocity
* @brief MFの値から並進加速度の目標値を計算する
* @return 計算後の目標値
*/
float calculateTargetVelocity(void)
{
	float target_val = target.velocity;

	if(MF.FLAG.ACCEL == 1) 			target_val += max.accel * 0.001f;
	else if(MF.FLAG.DECEL == 1)	target_val -= max.accel * 0.001f;

	if(target_val > max.velocity)			target_val = max.velocity;
	else if(target_val < -max.velocity)	target_val = -max.velocity;

	return target_val;
}
/**
 * calculateTargetOmega
* @brief MFの値から並進加速度の目標値を計算する
* @return 計算後の目標値
*/
float calculateTagetOmega(void)
{
	float target_val = target.omega;

	if(MF.FLAG.WACCEL == 1) 			target_val += max.omega_accel * 0.001f;
	else if(MF.FLAG.WDECEL == 1)	target_val -= max.omega_accel * 0.001f;

	if(target_val > max.omega)			target_val = max.omega;
	else if(target_val < -max.omega)	target_val = -max.omega;

	return target_val;
}

State_Typedef setStatus(float angle,float curve,
									float mileage, float velocity, float accel)
{
	State_Typedef instance;
	instance.angle = angle;
	instance.omega = velocity / curve;
	instance.omega_accel = accel / curve;
	instance.inverse_curvature = curve;
	instance.mileage = mileage;
	instance.velocity = velocity;
	instance.accel = accel;

	return instance;
}

PID_Typedef setParameters(float gainP, float gainI, float gainD, float limitI, float limitPID)
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
	mouse = setStatus(0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	target = setStatus(0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	max = setStatus(0.0f, 0.055f, 0.0f, 0.50f, 4.0f);

	//MF
	MF.FLAGS = 0x00000000;

	//PID関連
	PID_left_velocity = setParameters(3.5f, 0.01f, 0.0f, 0.1f, 0.6f);
	PID_right_velocity = setParameters(3.5f, 0.01f, 0.0f, 0.1f, 0.6f);
//	PID_wall_side = setParameters(0.003f, 0.0f, 0.00f, 0.00f, 0.2f);
	PID_wall_side = setParameters(0.000f, 0.0f, 0.00f, 0.00f, 0.2f);
	PID_wall_front_posture = setParameters(0.002f, 0.0f, 0.002f, 0.0f,0.2f);
	PID_wall_front_distance = setParameters(0.0005f, 0.0f, 0.002f, 0.0f,0.2f);
	PID_omega = setParameters(0.06f, 0.002f, 0.0f, 0.1f, 0.3f);
	PID_angle = setParameters(0.10f, 0.0f, 0.04f, 0.1f, 0.2f);
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

	float error_right = (sensor.wall_r - sensor.wall_r_offset);
	float error_left = (sensor.wall_l - sensor.wall_l_offset);
	if(fabs(error_right) > SENSOR_DIF_BORDER)	error_right = 0.0f;
	if(fabs(error_left) > SENSOR_DIF_BORDER)		error_left = 0.0f;


	PID_wall_side.error = error_right - error_left;
	PID_wall_front_posture.error = (sensor.wall_fl - FRONT_BASE_FL) - (sensor.wall_fr - FRONT_BASE_FR);
	PID_wall_front_distance.error = (sensor.wall_fl - FRONT_BASE_FL) + (sensor.wall_fr - FRONT_BASE_FR);

	//PID計算
	if(MF.FLAG.ACTRL)	{
		tmp = calculatePID(&PID_angle);
		output_duty_r += tmp;
		output_duty_l -= tmp;
	}
	if(MF.FLAG.WCTRL){
		tmp = calculatePID(&PID_omega);
		output_duty_r += tmp;
		output_duty_l -= tmp;
	}
	if(MF.FLAG.CTRL && target.velocity > 0.20f){
		tmp = calculatePID(&PID_wall_side);
		output_duty_r += tmp;
		output_duty_l -= tmp;
	}
	if(MF.FLAG.VCTRL){
		output_duty_r += calculatePID(&PID_right_velocity);
		output_duty_l += calculatePID(&PID_left_velocity);
	}
	if(MF.FLAG.FRONT){
		tmp = calculatePID(&PID_wall_front_posture);
		output_duty_r += tmp;
		output_duty_l -= tmp;
		tmp = calculatePID(&PID_wall_front_distance);
		output_duty_r -= tmp;
		output_duty_l -= tmp;
	}
	driveMotors(output_duty_l, output_duty_r);
}
