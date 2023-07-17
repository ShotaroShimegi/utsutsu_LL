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

State_Typedef mouse;						// センサデータベースの実測値
State_Typedef target;						// 理想の状態値、制御目標値
State_Typedef max;							// 逐次更新される制御目標値の上限値
State_Typedef min;							// 逐次更新される制御目標値の下限値

Params_Typedef* param;					// 最短走行などで用いるもの、基本的には他のparamsを代入して使う
Params_Typedef param1;					// 探索走行ベースのパラメータ、1番遅い
Params_Typedef param2;					// 0.6m/s　ベースのパラメータ
Params_Typedef param3;					// 0.8m/s　ベースのパラメータ, MAX2.0m/s
Params_Typedef param4;					// 1.2m/s　ベースのパラメータ, MAX2.0m/s

PID_Typedef PID_left_velocity;
PID_Typedef PID_right_velocity;
PID_Typedef PID_wall_side;
PID_Typedef PID_wall_front_posture;
PID_Typedef PID_wall_front_distance;
PID_Typedef* PID_omega;
PID_Typedef PID_omega2022;
PID_Typedef PID_omega2023;
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
void calculateSensorError(void) {
	static float pre_val_r[DIFF_SAMPLE] = {0};
	static float pre_val_l[DIFF_SAMPLE] = {0};
	static uint8_t  pre_cnt = 0;

	//　センサ値の変化量を計算
	float dif_left = sensor.wall_val[L] - pre_val_l[0];
	float dif_right = sensor.wall_val[R] - pre_val_r[0];
	uint16_t fix_l,fix_r;
	float error_left,error_right;

//　横壁状態に応じて、壁判断値の補正項を計算）
	if(fabs(dif_left) > DIFF_BORDER)		fix_l = 10;
	else												fix_l = 0;
	if(fabs(dif_right) > DIFF_BORDER)	fix_r = 10;
	else												fix_r = 0;

	error_left = (sensor.wall_val[L] - sensor.wall_offset[L]);
	error_right = (sensor.wall_val[R] - sensor.wall_offset[R]);

//　横壁センサの偏差計算、壁状況に応じて
	if(sensor.wall_val[L] > WALL_BORDE_L + fix_l
			&& sensor.wall_val[R] > WALL_BORDE_R + fix_r){
		//　両壁アリ
		PID_wall_side.error = 1.5f*error_right - error_left;
	}else if(sensor.wall_val[L] <= WALL_BORDE_L + fix_l
			&& sensor.wall_val[R] <= WALL_BORDE_R + fix_r){
		//　両壁ナシ
		PID_wall_side.error = 0.0f;
	}else if(sensor.wall_val[L] > WALL_BORDE_L + fix_l){
		//　左壁のみアリ
		PID_wall_side.error =  - 2.0f*error_left;
	}else if(sensor.wall_val[R] > WALL_BORDE_R + fix_r){
		//　右壁のみアリ
		PID_wall_side.error = 2.0f*1.5f*error_right;
	}

	//　PID計算できる範囲で
	PID_wall_front_posture.error = (sensor.wall_val[FL] - FRONT_BASE_FL) - (sensor.wall_val[FR] - FRONT_BASE_FR);
	PID_wall_front_distance.error = (sensor.wall_val[FL] - FRONT_BASE_FL) + (sensor.wall_val[FR] - FRONT_BASE_FR);

	//　センサ値を保存, 配列化
	if(pre_cnt  < DIFF_SAMPLE){
		pre_val_l[pre_cnt] = sensor.wall_val[L];
		pre_val_r[pre_cnt++] = sensor.wall_val[R];
	}else if(pre_cnt >= DIFF_SAMPLE){
		for(int i=0;i<DIFF_SAMPLE - 1;i++){
			pre_val_l[i] = pre_val_l[i+1];
			pre_val_r[i] = pre_val_r[i+1];
		}
		pre_val_l[DIFF_SAMPLE - 1] = sensor.wall_val[L];
		pre_val_r[DIFF_SAMPLE - 1] = sensor.wall_val[R];
	}
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

	if(MF.FLAG.ACCEL == 1) 			target_val += target.accel * 0.001f;
	else if(MF.FLAG.DECEL == 1)	target_val -= target.accel * 0.001f;

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

	if(MF.FLAG.WACCEL == 1) 			target_val += target.omega_accel * 0.001f;
	else if(MF.FLAG.WDECEL == 1)	target_val -= target.omega_accel * 0.001f;

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
float fixTargetOmegaFromWallSensor(float error) {
// ======　偏差ー＞角速度変換制御　＝＝＝＝＝＝
	float gain, angle, omega, dif_a,dif_w,max_dif_w;
	static float pre_dif = 0.0f;
	static float pre_omega = 0.0f;
// 偏差の絶対値に対する理想の角度[deg]を算出し、angleに格納
// 角度の範囲はフェイルセーフと連動して線形化
	gain = FAIL_ANGLE / ABS_ERROR_RANGE; // [deg / sensor]
	angle = gain * error;
	dif_a = angle + target.angle - mouse.angle;
// angleに制御周期内で追従できる角速度を計算
//	omega = dif_a/ 0.1 * CONVERT_TO_RAD + 0.1f*(dif_a - pre_dif);
	omega = dif_a * CONVERT_TO_RAD + 0.05f*(dif_a - pre_dif);
	pre_dif = dif_a;

// 角加速度制限
	max_dif_w = max.omega_accel * 0.001f;
	dif_w = omega - pre_omega;
	if(dif_w> max_dif_w)				omega = pre_omega + max_dif_w;
	else if (dif_w < -max_dif_w)	omega = pre_omega - max_dif_w;

// 角速度制限
	if(omega > 1.0f)				omega = 1.0f;
	else if(omega < -1.0f)	omega = -1.0f;

	pre_omega = omega;
	return omega;
}

/**
 * 工事中
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
									float accel, float jerk, uint8_t state) {
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
Turn_Typedef setTurnParams(float velocity,float accel, float curve,
											float b_offset, float a_offset) {

	Turn_Typedef instance;
	instance.velocity = velocity;
	instance.accel = accel;
	instance.omega = velocity / curve;
	instance.omega_accel = accel / curve;
	instance.inverse_curvature = curve;
	instance.velocity = velocity;
	instance.accel = accel;
	instance.before_offset = b_offset;
	instance.after_offset = a_offset;
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
void initMouseStatus(void) {
	//実際の走行中に用いるState変数
	mouse = setStatus(0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,0);
	target = setStatus(0.0f, 0.055f, 0.0f, 0.00f, 4.0f, 0.05f,0);
	max = setStatus(0.0f, 0.055f, 0.0f, 0.45f, 4.0f, 0.05f,0);
	min = setStatus(0.0f, 0.055f, 0.0f,
							-max.velocity,-max.accel, -max.jerk,0);

	// パラメータ宣言
	param1.upper = setStatus(0.0f, 0.045f, 0.0f, 1.0f, 4.0f, 0.05,0);
	param1.downer = setStatus(0.0f, 0.045f, 0.0f,
			0.45f,4.0f, -max.jerk,0);
	param1.small_turn = setTurnParams(0.45f, 4.0f, 0.045f,15.0f,30.0f);
	param1.big_turn90 = setTurnParams(0.45f, 4.0f, 0.120f,38.0f,38.0f);
	param1.big_turn180 = setTurnParams(0.45f, 4.0f, 0.088f,85.0f,85.0f);
	param = &param1;

	// 0.6m/s ベース
	param2.upper = setStatus(0.0f, 0.040f, 0.0f, 1.0f, 4.0f, 0.05f,0);
	param2.downer = setStatus(0.0f, 0.040f, 0.0f,0.60f,4.0f, -max.jerk,0);
	param2.small_turn = setTurnParams(0.60f, 8.0f, 0.040f,13.0f,49.0f);
	param2.big_turn90 = setTurnParams(0.60f, 4.0f, 0.123f,5.0f,35.0f);
	param2.big_turn180 = setTurnParams(0.60f, 4.0f, 0.090f,50.0f,85.0f);

	// 0.8m/s ベース
	param3.upper = setStatus(0.0f, 0.040f, 0.0f, 1.5f, 10.0f, 0.05f,0);
	param3.downer = setStatus(0.0f, 0.040f, 0.0f,0.8f,10.0f, -max.jerk,0);
	param3.small_turn = setTurnParams(0.8f, 10.0f, 0.040f,0.0f,29.0f);
	param3.small_turn.omega = 15.0;
	param3.big_turn90 = setTurnParams(0.8f, 10.0f, 0.123f,5.0f,33.0f);
	param3.big_turn180 = setTurnParams(0.8f, 10.0f, 0.090f,50.0f,75.0f);

	// 0.8m/s ベース, 裏パラ
	param4.upper = setStatus(0.0f, 0.040f, 0.0f, 2.0f, 10.0f, 0.05f,0);
	param4.downer = setStatus(0.0f, 0.040f, 0.0f,0.80f,10.0f, -max.jerk,0);
	param4.small_turn = setTurnParams(0.80f, 10.0f, 0.040f,13.0f,49.0f);
	param4.big_turn90 = setTurnParams(0.80f, 10.0f, 0.123f,5.0f,35.0f);
	param4.big_turn180 = setTurnParams(0.80f, 10.0f, 0.090f,50.0f,85.0f);

	// MF
	MF.FLAGS = 0x00000000;
	// PID関連
	PID_left_velocity = setParameters(3.5f, 0.01f, 0.0f, 0.1f, 1.0f);
	PID_right_velocity = setParameters(3.5f, 0.01f, 0.0f, 0.1f, 1.0f);
//	PID_wall_side = setParameters(0.003f, 0.0f, 0.10f, 0.00f, 0.2f);
//	PID_wall_front_posture = setParameters(0.002f, 0.0f, 0.002f, 0.0f,0.2f);
	PID_wall_front_distance = setParameters(0.003f, 0.0f, 0.001f, 0.0f,0.2f);
	PID_omega2022 = setParameters(0.06f, 0.002f, 0.0f, 0.1f, 1.0f);						// 2022 全日本用パラメータ
	PID_omega2023 = setParameters(0.20f, 0.05f, 0.0f, 0.3f, 1.0f);						// 2023用パラメータ
	PID_angle = setParameters(0.10f, 0.0f, 0.04f, 0.1f, 0.2f);

	// 最初はインスタンスを渡しておく
	PID_omega = &PID_omega2022;
}

void updateStatus(void) {
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
		// 壁制御と姿勢制御の合体
		calculateSensorError();
		if(MF.FLAG.CTRL && (target.velocity > -0.30f)) {
			fix_omega =  fixTargetOmegaFromWallSensor(PID_wall_side.error);
		} else {
			fix_omega = 0.0f;
		}

		// 目標値生成
		target.accel = max.accel;
		target.velocity = calculateTargetVelocity();
		target.omega = calculateTagetOmega();
		//偏差計算
		PID_angle.error = target.angle - mouse.angle;
		PID_left_velocity.error = target.velocity - mouse.velocity;
		PID_right_velocity.error = target.velocity - mouse.velocity;
		PID_omega->error = target.omega + fix_omega - mouse.omega;
		//PID計算
		if(MF.FLAG.ACTRL)	{
			tmp = calculatePID(&PID_angle);
			output_duty_r += tmp, output_duty_l -= tmp;
		}
		if(MF.FLAG.WCTRL){
			tmp = calculatePID(PID_omega);
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
//
//	if(fabs(output_duty_r) > 1.0f || fabs(output_duty_l) > 1.0f)	{
//		float add = (output_duty_r + output_duty_l) *0.50f;		// 並進速度抽出
//		float sub = (output_duty_r - output_duty_l) *0.50f;		// 角速度抽出
//		add = add / (add + sub);
//		sub = sub/ (add + sub);
//		if(output_duty_r > output_duty_l){
//			output_duty_r = add + sub;
//			output_duty_l = add - sub;
//		}else{
//			output_duty_r = add - sub;
//			output_duty_l = add + sub;
//		}
//	}
	if(fabs(output_duty_r) > 1.0f) output_duty_r = output_duty_r/fabs(output_duty_r) *1.0f;
	if(fabs(output_duty_l) > 1.0f) output_duty_l = output_duty_l/fabs(output_duty_l) *1.0f;

	driveMotors(output_duty_l, output_duty_r);
}
