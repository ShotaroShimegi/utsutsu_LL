/*
 * state.h
 *
 *  Created on: Feb 21, 2022
 *      Author: sssho
 */

#ifndef INC_SYSTEM_STATE_H_
#define INC_SYSTEM_STATE_H_

#include<stdint.h>

#define CONVERT_TO_RAD 0.01745f	// Unit Convert [deg/s] -> [rad/s]
#define CONVERT_TO_DEG 57.2957f

#define		FRONT_BASE_FL 280
#define 	FRONT_BASE_FR 260

//新しい壁制御用のサムシング
#define SENSOR_DIF_BORDER 100.0f
#define GAIN_FIXER 			300.0f
#define GAIN_WALL_P		0.010f
//#define GAIN_WALL_D		0.0001f
#define GAIN_WALL_D		0.0f
#define ERROR_MAX 			1.0f

typedef struct {
	float angle;						// Unit [deg]
	float omega;						// Unit [rad/s]
	float omega_accel;				// Unit [rad/s/s]
	float inverse_curvature;		//	Unit [m] 曲率の値は直感的ではないので、半径を採用
	float mileage;						// Unit [mm]
	float velocity;						// Unit [m/s] = [mm/ms]
	float accel;						// Unit [m/s/s]
	float jerk;							// Unit [m/s/s/s]
	uint8_t run_state;
}State_Typedef;

typedef struct {
	float error;
	float pre_error;
	float gainP;
	float gainI;
	float gainD;
	float outputPID;
	float outputI;
	float LimitI;
	float LimitPID;
}PID_Typedef;

typedef struct {
	State_Typedef upper;
	State_Typedef downer;
}Params_Typedef;

float calculatePID(PID_Typedef *instance);
PID_Typedef setParameters(float gainP, float gainI, float gainD, float LimitI, float LimitPID);
void initMouseStatus(void);
void updateStatus(void);
float fixTargetOmegaFromWallSensor(float);

extern State_Typedef mouse;
extern State_Typedef target;
extern State_Typedef max;
extern State_Typedef min;

extern Params_Typedef param1;

extern PID_Typedef PID_left_velocity;
extern PID_Typedef PID_right_velocity;
extern PID_Typedef PID_omega;
extern PID_Typedef PID_angle;

extern float output_duty_r;
extern float output_duty_l;

#endif /* INC_SYSTEM_STATE_H_ */
