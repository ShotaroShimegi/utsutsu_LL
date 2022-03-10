/*
 * sensing.h
 *
 *  Created on: Feb 18, 2022
 *      Author: sssho
 */

#ifndef INC_SYSTEM_SENSING_H_
#define INC_SYSTEM_SENSING_H_

#include<stdint.h>

//For IMU
#define SAMPLE_NUMBER 100
#define TIME_STEP 0.001f

//For Encoder
#define RAD_WHEEL_mm 12.25f
#define DIA_PINION_mm	4.0f
#define DIA_SPUR_mm 		21.0f
#define Pi 						3.14159f
#define GAIN_ENCODER RAD_WHEEL_mm * DIA_PINION_mm / DIA_SPUR_mm * 2 * Pi / 4096

//For Wall Sensor
#define CHARG_TIME		40
#define WALL_ID_FR		0
#define WALL_ID_R		1
#define WALL_ID_FF		2
#define WALL_ID_L		3
#define WALL_ID_FL		4
#define WALL_BORDE_FR	50
#define WALL_BORDE_R	30
#define WALL_BORDE_FF	40
#define WALL_BORDE_L		40
#define WALL_BORDE_FL	50

#define WALL_BACK_FR 230
#define WALL_BACK_FL 230

typedef struct{
	float encoder_vel_r;		// Unit m/s
	float encoder_vel_l;
	int32_t encoder_mileage_r;
	int32_t encoder_mileage_l;
	int16_t pulse_r;
	int16_t pulse_l;
	float mileage_r_mm;
	float mileage_l_mm;
	float gyro_omega;
	float gyro_accel;
	float gyro_omega_offset;
	float gyro_accel_offset;
	uint16_t wall_fl;
	uint16_t wall_l;
	uint16_t wall_ff;
	uint16_t wall_r;
	uint16_t wall_fr;
	uint16_t wall_fl_offset;
	uint16_t wall_l_offset;;
	uint16_t wall_ff_offset;;
	uint16_t wall_r_offset;;
	uint16_t wall_fr_offset;;
} Sensing_Typedef;


void initSensors(void);
void updateSensors(void);
void getOffsets(void);

float getCenterMileage(void);
float addAngle(float val);
float getCenterVelocity(void);
float getOmega(void);
float getAccel(void);

uint8_t getWallInfo(void);

extern Sensing_Typedef sensor;

#endif /* INC_SYSTEM_SENSING_H_ */
