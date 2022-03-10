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
	sensor.mileage_l_mm = (float)sensor.encoder_mileage_l * (float)GAIN_ENCODER;
	sensor.mileage_r_mm = (float)sensor.encoder_mileage_r * (float)GAIN_ENCODER;

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

void getWallSensorOffset(void)
{
	changeLEDs(ON, ON, ON, ON, ON);
	waitUs(CHARG_TIME);
	sensor.wall_fr_offset = getWallADC(WALL_ID_FR);
	sensor.wall_ff_offset = getWallADC(WALL_ID_FF);
	sensor.wall_l_offset = getWallADC(WALL_ID_L);
	sensor.wall_fl_offset = getWallADC(WALL_ID_FL);
	sensor.wall_r_offset = getWallADC(WALL_ID_R);
	changeLEDs(OFF, OFF, OFF, OFF, OFF);
}

void getWallSensor(void)
{
	changeLEDs(OFF, ON, ON, OFF, ON);
	waitUs(CHARG_TIME);
	sensor.wall_fr = getWallADC(WALL_ID_FR);
	sensor.wall_ff = getWallADC(WALL_ID_FF);
	sensor.wall_l = getWallADC(WALL_ID_L) ;
	changeLEDs(ON, OFF, OFF, ON, OFF);
	waitUs(CHARG_TIME);
	sensor.wall_fl = getWallADC(WALL_ID_FL);
	sensor.wall_r = getWallADC(WALL_ID_R) ;
	changeLEDs(OFF, OFF, OFF, OFF, OFF);
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
	if(sensor.wall_ff > WALL_BORDE_FF || sensor.wall_fr > WALL_BORDE_FR ||
			sensor.wall_fl > WALL_BORDE_FL)	val |= 0x88;
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

	sensor.encoder_mileage_l = 0;
	sensor.encoder_mileage_r = 0;
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
	sensor.pulse_l = (-1) * getEncoderData(LEFT);
	sensor.pulse_r = getEncoderData(RIGHT);
	sensor.encoder_mileage_l += sensor.pulse_l;
	sensor.encoder_mileage_r += sensor.pulse_r;
	sensor.encoder_vel_l = getEncoderVelocity(sensor.pulse_l);
	sensor.encoder_vel_r = getEncoderVelocity(sensor.pulse_r);
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
