/*
 * sensing.c
 *
 *  Created on: Feb 18, 2022
 *      Author: sssho
 */
#include"System/sensing.h"

#include"Hardware/ICM20689.h"
#include"Hardware/encoder.h"
#include"Hardware/wall_sensor.h"
#include"Hardware/basic_timer.h"
#include"Hardware/interface_LED.h"

Sensing_Typedef sensor;

float getEncoderVelocity(int16_t count)
{
	return GAIN_ENCODER * (float) count;
}

float getCenterMileage(void)
{
	sensor.mileage_l_mm = sensor.encoder_mileage_l * GAIN_ENCODER;
	sensor.mileage_r_mm = sensor.encoder_mileage_r * GAIN_ENCODER;

	return (sensor.mileage_l_mm+ sensor.mileage_r_mm)*0.5f;
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

void getWallSensorOffset(void){
	changeFrontCenterLED(OFF);
	changeLeftLED(OFF);
	changeFrontLeftLED(OFF);
	changeRightLED(OFF);
	changeFrontRightLED(OFF);
	waitUs(CHARG_TIME);
	sensor.wall_fr_offset = getWallADC(WALL_ID_FR);
	sensor.wall_ff_offset = getWallADC(WALL_ID_FF);
	sensor.wall_l_offset = getWallADC(WALL_ID_L);
	sensor.wall_fl_offset = getWallADC(WALL_ID_FL);
	sensor.wall_r_offset = getWallADC(WALL_ID_R);
}

void getWallSensor(void)
{
	changeFrontRightLED(ON);
	changeFrontCenterLED(ON);
	changeLeftLED(ON);
	waitUs(CHARG_TIME);

	sensor.wall_fr = getWallADC(WALL_ID_FR);
	sensor.wall_ff = getWallADC(WALL_ID_FF);
	sensor.wall_l = getWallADC(WALL_ID_L) ;
	changeFrontRightLED(OFF);
	changeFrontCenterLED(OFF);
	changeLeftLED(OFF);

	changeFrontLeftLED(ON);
	changeRightLED(ON);
	waitUs(CHARG_TIME);

	sensor.wall_fl = getWallADC(WALL_ID_FL);
	sensor.wall_r = getWallADC(WALL_ID_R) ;
	changeFrontLeftLED(OFF);
	changeRightLED(OFF);
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
// getWallInfo
//	@brief 加速度のオフセット用平均値を取得する
// @param num　取得するサンプル数
//+++++++++++++++++++++++++++++++++++++++++++++++
uint8_t getWallInfo(void)
{
	uint8_t val = 0x00;
	//----Check Front----
	if(sensor.wall_ff > WALL_BORDE_FF)	val |= 0x88;
	//----Check Right----
	if(sensor.wall_r > WALL_BORDE_R)		val |= 0x44;
	//----Check Left----
	if(sensor.wall_l > WALL_BORDE_L)		val |= 0x11;
	return val;
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
void updateSensors(void)
{
	//IMU
	sensor.gyro_omega = readGyroOmegaZ() - sensor.gyro_omega_offset;
	sensor.gyro_accel = readGyroAccelX() - sensor.gyro_accel_offset;
	//Encoder
	int16_t pulse_l = -getEncoderData(TIM3);
	int16_t pulse_r = getEncoderData(TIM4);
	sensor.encoder_mileage_l += pulse_l;
	sensor.encoder_mileage_r += pulse_r;
	sensor.encoder_vel_l = getEncoderVelocity(pulse_l);
	sensor.encoder_vel_r = getEncoderVelocity(pulse_r);
	//Wall Sensor
	getWallSensor();
}

//+++++++++++++++++++++++++++++++++++++++++++++++
// getOffsets
//	@brief IMU用のオフセットを取得する、主に走行前など自律移動開始時に呼び出す
//+++++++++++++++++++++++++++++++++++++++++++++++

void getOffsets(void)
{
	sensor.gyro_accel_offset = getAccelOffset(SAMPLE_NUMBER);
	sensor.gyro_omega_offset = getOmegaOffset(SAMPLE_NUMBER);
	getWallSensorOffset();
}
