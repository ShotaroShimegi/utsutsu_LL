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

#define SENSOR_DIF_BORDER 10.0f

typedef struct {
	float angle;						// Unit [deg]
	float omega;						// Unit [rad/s]
	float omega_accel;				// Unit [rad/s/s]
	float inverse_curvature;		//	Unit [m] 曲率の値は直感的ではないので、半径を採用
	float mileage;						// Unit [mm]
	float velocity;						// Unit [m/s] = [mm/ms]
	float accel;						// Unit [m/s/s]
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

float calculatePID(PID_Typedef *instance);
PID_Typedef setParameters(float gainP, float gainI, float gainD, float LimitI, float LimitPID);
void initMouseStatus(void);
void updateStatus(void);

extern State_Typedef mouse;
extern State_Typedef target;
extern State_Typedef max;

extern PID_Typedef PID_left_velocity;
extern PID_Typedef PID_right_velocity;
extern PID_Typedef PID_omega;
extern PID_Typedef PID_angle;

#endif /* INC_SYSTEM_STATE_H_ */
