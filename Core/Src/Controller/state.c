/*
 * state.c
 *
 *  Created on: Feb 21, 2022
 *      Author: sssho
 */
#include"Controller/state.h"
#include"Controller/mode.h"

#include"System/map.h"
#include"System/sensing.h"
#include"System/flags.h"

#include"Hardware/motor.h"

#include<math.h>

State_Typedef mouse;
State_Typedef target;
State_Typedef max;
State_Typedef min;

Params_Typedef param1;

PID_Typedef PID_left_velocity;
PID_Typedef PID_right_velocity;
PID_Typedef PID_wall_side;
PID_Typedef PID_wall_front_posture;
PID_Typedef PID_wall_front_distance;
PID_Typedef PID_omega;
PID_Typedef PID_angle;

float output_duty_r = 0.0f;
float output_duty_l = 0.0f;
float fix_omega = 0.0f;

/**
* fixTargetOmegaFromWallSensor
* @brief 壁センサの統合偏差から角速度目標値への補正量を計算
* @param error -> 壁センサの偏差
* @return 角速度目標値への補正項
*/
void calculateSensorError(void)
{
	static float pre_wall_val_r = 0.0f;
	static float pre_wall_val_l = 0.0f;

	//　センサ値の変化量を計算
	float dif_left = sensor.wall_val[L] - pre_wall_val_l;
	float dif_right = sensor.wall_val[R] - pre_wall_val_r;

//　横壁状態に応じて、制御目標値（.wall_offsetの補正項を計算）

//　横壁センサの偏差計算、壁状況に応じて
	float error_left = (sensor.wall_val[L] - sensor.wall_offset[L]);
	float error_right = (sensor.wall_val[R] - sensor.wall_offset[R]);


//　PID計算できる範囲で
	PID_wall_side.error = 1.5f*error_right - error_left;
	PID_wall_front_posture.error = (sensor.wall_val[FL] - FRONT_BASE_FL) - (sensor.wall_val[FR] - FRONT_BASE_FR);
	PID_wall_front_distance.error = (sensor.wall_val[FL] - FRONT_BASE_FL) + (sensor.wall_val[FR] - FRONT_BASE_FR);
//　センサ値を保存
	pre_wall_val_l = sensor.wall_val[L];
	pre_wall_val_r = sensor.wall_val[R];

}

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
* @brief 　並進加速度の目標値を計算する
* @return 計算後の目標値
*/
float calculateTargetVelocity(void)
{
	float target_val = target.velocity;

	if(MF.FLAG.ACCEL == 1) 			target_val += max.accel * 0.001f;
	else if(MF.FLAG.DECEL == 1)	target_val -= max.accel * 0.001f;

	if(target_val > max.velocity)			target_val = max.velocity;
	else if(target_val < min.velocity)		target_val = min.velocity;

	return target_val;
}
/**
 * calculateTargetOmega
* @brief 	MFの値から回転速度の目標値を計算する
* @return 計算後の目標値
*/
float calculateTagetOmega(void)
{
	float target_val = target.omega;

	if(MF.FLAG.WACCEL == 1) 			target_val += max.omega_accel * 0.001f;
	else if(MF.FLAG.WDECEL == 1)	target_val -= max.omega_accel * 0.001f;

	if(target_val > max.omega)			target_val = max.omega;
	else if(target_val < min.omega)	target_val = min.omega;

	return target_val;
}

/**
* fixTargetOmegaFromWallSensor
* @brief 壁センサの統合偏差から角速度目標値への補正量を計算
* @param error -> 壁センサの偏差
* @return 角速度目標値への補正項
*/
float fixTargetOmegaFromWallSensor(float error)
{
/*
// カスケード接続で角速度を操ろうとした。偏差の符号変化しないと角速度0のままになるので廃止。
 	float fix_error = error / GAIN_FIXER;
	static float pre_fix_error = 0.0f;

	if(fix_error > ERROR_MAX)				fix_error = ERROR_MAX;
	else if(fix_error < -ERROR_MAX)		fix_error = -ERROR_MAX;

	float fix_val = GAIN_WALL_P * fix_error + GAIN_WALL_D * (fix_error - pre_fix_error);

	pre_fix_error = fix_error;
	return fix_val;
*/

// ======　偏差ー＞角速度変換制御　＝＝＝＝＝＝
	float gain, angle, omega, dif;
	static float pre_dif = 0.0f;
// 偏差の絶対値に対する理想の角度[deg]を算出し、angleに格納
// 角度の範囲はフェイルセーフと連動して線形化
	gain = FAIL_ANGLE / ABS_ERROR_RANGE; // [deg / sensor]
	angle = gain * error;
	dif = angle + target.angle - mouse.angle;
// angleに制御周期内で追従できる角速度を計算
// 角速度の値が大きすぎたら制限
	omega = dif/ 0.1 * CONVERT_TO_RAD + 0.1f*(dif - pre_dif);
	pre_dif = dif;
/*
	if(omega > 1.0f)		omega = 1.0f;
	else if(omega < -0.10f)	omega = -1.0f;
*/
	return omega;
}

/**
 * calculateTargetAccelAndVelocity
* @brief 　並進加速度、並進速度の目標値を計算する
* @return 計算後の目標値
*/
float calculateTargetAccelAndVelocity(void)
{
	float target_velocity = target.velocity;

	if(MF.FLAG.WACCEL || MF.FLAG.WDECEL)	return target.velocity;

	/*
	 * 加速度をフィードフォワード的に計算する式
	 */

	if(MF.FLAG.ACCEL == 1) 			target.accel = max.accel;
	else if(MF.FLAG.DECEL == 1)	target.accel = -max.accel;

	target_velocity += target.accel * 0.001f;

	if(target_velocity > max.velocity)			target_velocity = max.velocity;
	else if(target_velocity < -min.velocity)	target_velocity = -max.velocity;

	return target_velocity;
}

State_Typedef setStatus(float angle,float curve,
									float mileage, float velocity,
									float accel, float jerk, uint8_t state)
{
	State_Typedef instance;
	instance.angle = angle;
	instance.omega = velocity / curve;
	instance.omega_accel = accel / curve;
	instance.inverse_curvature = curve;
	instance.mileage = mileage;
	instance.velocity = velocity;
	instance.accel = accel;
	instance.jerk = jerk;
	instance.run_state = state;

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
	mouse = setStatus(0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,0);
	target = setStatus(0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,0);
	max = setStatus(0.0f, 0.055f, 0.0f, 0.50f, 4.0f, 0.05,0);
	min = setStatus(0.0f, 0.055f, 0.0f,
							-max.velocity,-max.accel, -max.jerk,0);

	// パラメータ宣言
	param1.upper = setStatus(0.0f, 0.055f, 0.0f, 0.50f, 4.0f, 0.05,0);
	param1.downer = 	min = setStatus(0.0f, 0.055f, 0.0f,
			-max.velocity,-max.accel, -max.jerk,0);
	//MF
	MF.FLAGS = 0x00000000;

	//PID関連
	PID_left_velocity = setParameters(3.5f, 0.01f, 0.0f, 0.1f, 0.6f);
	PID_right_velocity = setParameters(3.5f, 0.01f, 0.0f, 0.1f, 0.6f);
//	PID_wall_side = setParameters(0.003f, 0.0f, 0.10f, 0.00f, 0.2f);
	PID_wall_front_posture = setParameters(0.002f, 0.0f, 0.002f, 0.0f,0.2f);
	PID_wall_front_distance = setParameters(0.0005f, 0.0f, 0.002f, 0.0f,0.2f);
	PID_omega = setParameters(0.06f, 0.002f, 0.0f, 0.1f, 0.3f);
	PID_angle = setParameters(0.10f, 0.0f, 0.04f, 0.1f, 0.2f);
}

void updateStatus(void)
{
	//出力用の変数
	float tmp = 0.0f;
	fix_omega = 0.0f;
	output_duty_l = 0.0f;
	output_duty_r = 0.0f;

	//センサ値からステータスを更新
	mouse.angle = addAngle(mouse.angle);
	mouse.omega = getOmega()*CONVERT_TO_RAD;
	mouse.accel = getAccel();
	mouse.mileage = getCenterMileage();
	mouse.velocity = getCenterVelocity();

// ==== 現行の制御体制====
	if(MF.FLAG.NEW == 0){
//		float error_right = (sensor.wall_val[R] - sensor.wall_offset[R]);
//		float error_left = (sensor.wall_val[L] - sensor.wall_offset[L]);
//
//		PID_wall_side.error = 1.5f*error_right - error_left;
//		PID_wall_front_posture.error = (sensor.wall_val[FL] - FRONT_BASE_FL) - (sensor.wall_val[FR] - FRONT_BASE_FR);
//		PID_wall_front_distance.error = (sensor.wall_val[FL] - FRONT_BASE_FL) + (sensor.wall_val[FR] - FRONT_BASE_FR);

		calculateSensorError();

		//壁制御と姿勢制御の合体
		if(MF.FLAG.CTRL && (target.velocity > 0.0f)) {
			fix_omega =  fixTargetOmegaFromWallSensor(PID_wall_side.error);
		} else {
			fix_omega = 0.0f;
		}

		//目標値生成
		target.velocity = calculateTargetVelocity();
		target.omega = calculateTagetOmega();

		//偏差計算
		PID_angle.error = target.angle - mouse.angle;
		PID_left_velocity.error = target.velocity - mouse.velocity;
		PID_right_velocity.error = target.velocity - mouse.velocity;
		PID_omega.error = target.omega + fix_omega - mouse.omega;

		//PID計算
		if(MF.FLAG.ACTRL)	{
			tmp = calculatePID(&PID_angle);
			output_duty_r += tmp, output_duty_l -= tmp;
		}
		if(MF.FLAG.WCTRL){
			tmp = calculatePID(&PID_omega);
			output_duty_r += tmp, output_duty_l -= tmp;
		}
		if(MF.FLAG.VCTRL){
			output_duty_r += calculatePID(&PID_right_velocity);
			output_duty_l += calculatePID(&PID_left_velocity);
		}
		if(MF.FLAG.FRONT){
			tmp = calculatePID(&PID_wall_front_distance);
			output_duty_r -= tmp, output_duty_l -= tmp;
		}

	}else if(MF.FLAG.NEW == 1){
		// 新制御用の項目、工事中
	}
	driveMotors(output_duty_l, output_duty_r);
}
