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
	static uint8_t i = 0;
	static float log[NUM_SAMPLE_ACCEL];
	float filtered_accel = 0.0f;
	if(i < NUM_SAMPLE_ACCEL - 1){
		log[i] = sensor.gyro_accel;
		i++;
		return -sensor.gyro_accel;
	}

	if(i == NUM_SAMPLE_ACCEL - 1){
		log[i] = sensor.gyro_accel;
		for(uint8_t j=0; j <NUM_SAMPLE_ACCEL;j++){
			filtered_accel += log[j];
		}
		filtered_accel /= NUM_SAMPLE_ACCEL;
		log[0] = log[1];
		log[1] = log[2];
		log[2] = log[3];
	}
	return -filtered_accel;
}

void getWallSensorOffset(void)
{
	changeLEDs(ON, ON, ON, ON, ON);
	waitUs(CHARG_TIME);
	sensor.wall_offset[FR] = getWallADC(FR);
	sensor.wall_offset[FF] = getWallADC(FF);
	sensor.wall_offset[L] = getWallADC(L);
	sensor.wall_offset[FL] = getWallADC(FL);
	sensor.wall_offset[R] = getWallADC(R);
	changeLEDs(OFF, OFF, OFF, OFF, OFF);
}

void getWallSensor(void)
{
	changeLEDs(OFF, ON, ON, OFF, ON);
	waitUs(CHARG_TIME);
	sensor.wall_val[FR] = getWallADC(FR);
	sensor.wall_val[FF] = getWallADC(FF);
	sensor.wall_val[L] = getWallADC(L) ;
	changeLEDs(ON, OFF, OFF, ON, OFF);
	waitUs(CHARG_TIME);
	sensor.wall_val[FL] = getWallADC(FL);
	sensor.wall_val[R] = getWallADC(R) ;
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
//	@brief 壁情報を2進数で出力
//+++++++++++++++++++++++++++++++++++++++++++++++
uint8_t getWallInfo(void)
{
	uint8_t val = 0x00;
	//----Check Front----
	if(sensor.wall_val[FR] > WALL_BORDE_FR || sensor.wall_val[FL] > WALL_BORDE_FL)	val |= 0x88;
	//----Check Right----
	if(sensor.wall_val[R] > WALL_BORDE_R)		val |= 0x44;
	//----Check Left----
	if(sensor.wall_val[L] > WALL_BORDE_L)		val |= 0x11;
	return val;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
// filterWallSensors
//	@brief 横壁センサの値に移動平均フィルタを掛ける
//+++++++++++++++++++++++++++++++++++++++++++++++
void filterWallSensors(){
	static uint16_t sensor_buff_right[FILTER_SAMPLE],sensor_buff_left[FILTER_SAMPLE];
	static uint8_t sample_count = 0;
	uint16_t sum_l,sum_r;

	if(sample_count < FILTER_SAMPLE){
// ==== 最初に配列の値を埋めるフェイズ ====
		sensor_buff_left[sample_count] = sensor.wall_val[L];
		sensor_buff_right[sample_count] = sensor.wall_val[R];
		sample_count++;
	}else{
// ==== 実際に移動平均フィルタの値を計算し代入====
		sum_l = sensor.wall_val[L],sum_r = sensor.wall_val[R];
		for (int i=0;i<FILTER_SAMPLE;i++) {
			sum_l += sensor_buff_left[i], sum_r += sensor_buff_right[i];
		}
		sensor.wall_val[L] = sum_l / (FILTER_SAMPLE + 1);
		sensor.wall_val[R] = sum_r /( FILTER_SAMPLE + 1);
// ==== 配列内の過去データを更新====
		for (int i=0;i<FILTER_SAMPLE - 1;i++) {
			sensor_buff_left[i] = sensor_buff_left[i+1];
			sensor_buff_right[i] = sensor_buff_right[i+1];
		}
		sensor_buff_left[FILTER_SAMPLE - 1] = sensor.wall_val[L];
		sensor_buff_right[FILTER_SAMPLE - 1] = sensor.wall_val[R];
	}
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
	filterWallSensors();				// 横壁センサのみ移動平均フィルタ
}

//+++++++++++++++++++++++++++++++++++++++++++++++
// getOffsets
//	@brief IMU用のオフセットを取得する、主に走行前など自律移動開始時に呼び出す
//+++++++++++++++++++++++++++++++++++++++++++++++
void getOffsets(void)
{
	sensor.gyro_accel_offset = getAccelOffset(SAMPLE_NUMBER_OFFSET);
	sensor.gyro_omega_offset = getOmegaOffset(SAMPLE_NUMBER_OFFSET);
	getWallSensorOffset();
}

