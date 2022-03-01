/*
 * mode.c
 *
 *  Created on: 2022/02/28
 *      Author: sssho
 */
#include<stdio.h>
#include<math.h>

#include"Controller/mode.h"

#include"System/music.h"
#include"System/state.h"
#include"System/sensing.h"

#include"Hardware/encoder.h"
#include"Hardware/interface_LED.h"
#include"Hardware/buzzer.h"
#include"Hardware/basic_timer.h"
#include"Hardware/wall_sensor.h"
#include"Hardware/motor.h"
#include"Hardware/battery.h"

#include"tim.h"

/*	modeSelect
 * @brief モード（0~15）をエンコーダから取得・確定させる
 * @return 確定したモードの値
 */
uint8_t modeSelect(void)
{
	static uint8_t mode, error;
	float right_dial_val, left_dial_val;
	static uint8_t pre_mode = 0xff;
	uint8_t enter = 0;

	//入力に使うエンコーダ用の初期化
	basicTimerPause();
	TIM3->CNT = 0;
	TIM4->CNT = 0;
	enableEncoder();
	printf("Select Mode \n");

	//モード(0~15)を確定するwhileループ
	do{
		//エンコーダからの値取得と単位変換
		right_dial_val = (float)TIM4->CNT;
		left_dial_val = (float)TIM3->CNT;
		mode = (float)(right_dial_val /65536) * RIGHT_RESOLUTION;
		enter = (float)(left_dial_val / 65536) * LEFT_RESOLUTION;
		//前ループとの偏差からモード設定の変更と確定を判断
		error = mode - pre_mode;
		if(error != 0){
			printf(" mode:%2d\n", mode);
			displayBinaryValueWithLEDs(mode);
			Melody(c6 + (60 * mode),100);
		}
		pre_mode = mode;
	}while(enter != 2);

	disableEncoder();
	TIM3->CNT = 0;
	TIM4->CNT = 0;
	printf(" Enter : mode %2d\n",mode);
	return mode;
}

void waitStarting(void)
{
	//安全のためのモータが動かない用の設定
	MF.FLAGS = 0x0000;
	shutdownMotors();

	basicTimerStart();
	printf("Ready???");
	while(1){
		if(sensor.wall_ff > WALL_START_VALUE){
			Melody(e6,300);
			Melody(f6,300);
			Melody(g6,300);
			waitMs(1000);
			break;
		}
	}
}

void judgeFailSafe(void)
{
	float error_angle = target.angle - mouse.angle;
	float error_velocity_r = target.velocity - sensor.encoder_vel_r;
	float error_velocity_l = target.velocity - sensor.encoder_vel_l;
	uint8_t error_code = 0;

//	if(readBatteryCheck() == 0)							error_code = 1;
	if(MF.FLAG.WCTRL != 1 && fabs(error_angle) > FAIL_ANGLE) 	error_code =2;
	if(fabs(error_velocity_l) > FAIL_VELOCITY)	error_code = 3;
	if(fabs(error_velocity_r) > FAIL_VELOCITY)	error_code = 4;

	if(error_code != 0){
		shutdownMotors();
		basicTimerPause();
		displayBinaryValueWithLEDs(0x00);
		changeFrontLeftLED(OFF);
		changeLeftLED(OFF);
		changeFrontCenterLED(OFF);
		changeRightLED(OFF);
		changeFrontRightLED(OFF);
		waitMs(500);
		MelodyMrLawrence();
		while(1){
			printf("Error Code : %d\n", error_code);
			waitMs(1000);
		}
	}

}

