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
#define SAMPLE_NUMBER_OFFSET 100
#define TIME_STEP 0.001f
#define NUM_SAMPLE_ACCEL 4

//For Encoder
#define RAD_WHEEL_mm 12.25f
#define DIA_PINION_mm	4.0f
#define DIA_SPUR_mm 		21.0f
#define Pi 						3.14159f
#define GAIN_ENCODER RAD_WHEEL_mm * DIA_PINION_mm / DIA_SPUR_mm * 2 * Pi / 4096

//For Wall Sensor
#define CHARG_TIME		40
#define WALL_BORDE_FR	50
#define WALL_BORDE_R	20
#define WALL_BORDE_FF	20
#define WALL_BORDE_L		30
#define WALL_BORDE_FL	50

#define WALL_BACK_FR 230
#define WALL_BACK_FL 230

#define WALL_TURN_VALUE 120
#define SLALOM_OFFSET_BEFORE 450
#define SLALOM_OFFSET_AFTER 230
#define FRONT_WALL_CONTROL 270

#define FILTER_SAMPLE 6

enum WALL_ID{
	FR,R,FF,L,FL
};

// センサデータ統合
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
	uint16_t wall_val[5];
	uint16_t wall_offset[5];
} Sensing_Typedef;

//　関数のプロトタイプ宣言
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
