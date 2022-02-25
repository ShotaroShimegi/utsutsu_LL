/*
 * state.h
 *
 *  Created on: Feb 21, 2022
 *      Author: sssho
 */

#ifndef INC_SYSTEM_STATE_H_
#define INC_SYSTEM_STATE_H_

#include<stdint.h>

typedef struct {
	float angle;
	float omega;
	float omega_accel;
	float mileage;
	float velocity;
	float accel;
	uint8_t x;
	uint8_t y;
	uint8_t dir;
}State_Typedef;

typedef union{
	uint16_t FLAGS;
	struct mouse_flags{
		uint8_t SET :1;			//尻当てフラグ(B0)		(:1は1ビット分の意味、ビットフィールド)
		uint8_t SCND :1;		//二次走行フラグ(B1)
		uint8_t SEARCH :1;		//探索走行フラグ(B2)
		uint8_t CTRL :1;			//壁制御フラグ(B3)
		uint8_t ACCEL :1;			//並進加速フラグ(B4)
		uint8_t DECEL :1;			//並進減速フラグ(B5)
		uint8_t WACCEL :1;		//回転加速フラグ(B6)
		uint8_t WDECEL :1;		//回転減速フラグ(B7)
		uint8_t VCTRL :1;		//並進制御フラグ(B8)
		uint8_t WCTRL :1;		//回転制御フラグ(B9)
		uint8_t FAIL :1;			//フェイルセーフフラグ(B10)
		uint8_t ACTRL :1;		//角度制御フラグ(B11)
		uint8_t FRONT :1;		//前制御フラグ(B12)
		uint8_t RESV13 :1;		//予備フラグ(B13)
		uint8_t RESV14 :1;		//予備フラグ(B14)
		uint8_t RESV15 :1;		//予備フラグ(B15)
	}FLAG;
}Flags_Typedef;

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
PID_Typedef setPrameters(float gainP, float gainI, float gainD, float LimitI, float LimitPID);

void initMouseStatus(void);
void updateStatus(void);

extern State_Typedef mouse;
extern State_Typedef target;
extern State_Typedef max;
extern Flags_Typedef MF;
extern PID_Typedef PID_left_velocity;
extern PID_Typedef PID_right_velocity;
extern PID_Typedef PID_omega;
extern PID_Typedef PID_angle;

extern float d_out_r;
extern float d_out_l;

#endif /* INC_SYSTEM_STATE_H_ */
