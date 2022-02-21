/*
 * sensing.c
 *
 *  Created on: Feb 18, 2022
 *      Author: sssho
 */
#include"Hardware/ICM20689.h"
#include"Hardware/encoder.h"
#include"Hardware/wall_sensor.h"
#include"Hardware/basic_timer.h"
#include"Hardware/interface_LED.h"

#include"System/sensing.h"

volatile Sensing_Typedef sensor;


float getEncoderVelocity(int16_t count)
{
	return GAIN_ENCODER * (float) count;
}

int32_t addPulseToMileage(int32_t val, int16_t pulse1, int16_t pulse2)
{
	int32_t answer = val +  (pulse1+ pulse2)*0.5f;
	return answer;
}

float getMileage(void)
{
	return sensor.encoder_pulse_mileage*GAIN_ENCODER;
}

float addAngle(float val)
{
	return val + sensor.gyro_omega * TIME_STEP;
}

float getCenterVelocity(void)
{
	float val = (sensor.encoder_vel_r+sensor.encoder_vel_l)*0.5f;
	return val;
}

float getOmega(void)
{
	return sensor.gyro_omega;
}

float getAccel(void)
{
	return sensor.gyro_accel;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
// getOmegaOffset
//	@brief 角速度のオフセット値の平均値を取得する
// @param num　取得するサンプル数
//+++++++++++++++++++++++++++++++++++++++++++++++
float getOmegaOffset(uint16_t num)
{
	uint16_t i = 0;
	float sum = 0.0f;
	for(i=0;i<num;i++){
			sum += readGyroOmegaZ();
			waitMs(1);
	}
	sum = sum / num;
	return sum;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
// getAccelOffset
//	@brief 加速度のオフセット用平均値を取得する
// @param num　取得するサンプル数
//+++++++++++++++++++++++++++++++++++++++++++++++
float getAccelOffset(uint16_t num)
{
	uint16_t i = 0;
	float sum = 0.0f;
	for(i=0;i<num;i++){
			sum += readGyroAccelX();
			waitMs(1);
	}
	sum = sum / num;
	return sum;
}
//+++++++++++++++++++++++++++++++++++++++++++++++
// initSensors
//	@brief センサの初期化、mainループ前に呼び出す想定
//+++++++++++++++++++++++++++++++++++++++++++++++
void initSensors(void)
{
	initIMU();
	enableEncoder();
	startADCwithDMA();
}

//+++++++++++++++++++++++++++++++++++++++++++++++
// updateSensors
//	@brief センサ類の更新、主にタイマ割込みで呼び出す想定
//+++++++++++++++++++++++++++++++++++++++++++++++
void updataSensors(void)
{
	//IMU
	sensor.gyro_omega = readGyroOmegaZ() - sensor.gyro_omega_offset;
	sensor.gyro_accel = readGyroAccelX() - sensor.gyro_accel_offset;
	//Encoder
	int16_t pulse_l = -getEncoderData(TIM3);
	int16_t pulse_r = getEncoderData(TIM4);
	sensor.encoder_pulse_mileage = addPulseToMileage(sensor.encoder_pulse_mileage, pulse_r, pulse_l);
	sensor.encoder_vel_l = getEncoderVelocity(pulse_l);
	sensor.encoder_vel_r = getEncoderVelocity(pulse_r);
	//Wall Sensor
	changeFrontRightLED(ON);
	changeFrontCenterLED(ON);
	changeLeftLED(ON);
	waitUs(CHARG_TIME);
	sensor.wall_fr = getWallADC(WALL_ID_FR);
	sensor.wall_ff = getWallADC(WALL_ID_FF);
	sensor.wall_l = getWallADC(WALL_ID_L);
	changeFrontRightLED(OFF);
	changeFrontCenterLED(OFF);
	changeLeftLED(OFF);

	changeFrontLeftLED(ON);
	changeRightLED(ON);
	waitUs(CHARG_TIME);
	sensor.wall_fl = getWallADC(WALL_ID_FL);
	sensor.wall_r = getWallADC(WALL_ID_R);
	changeFrontLeftLED(OFF);
	changeRightLED(OFF);

}

//+++++++++++++++++++++++++++++++++++++++++++++++
// getOffsets
//	@brief IMU用のオフセットを取得する、主に走行前など自律移動開始時に呼び出す
//+++++++++++++++++++++++++++++++++++++++++++++++

void getOffsets(void)
{
	sensor.gyro_omega_offset = getAccelOffset(SAMPLE_NUMBER);
	sensor.gyro_omega_offset = getOmegaOffset(SAMPLE_NUMBER);
}
