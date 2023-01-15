/*
 * mode.c
 *
 *  Created on: 2022/02/28
 *      Author: sssho
 */
#include<stdio.h>
#include<math.h>

#include"Controller/mode.h"
#include"Controller/state.h"

#include"System/music.h"
#include"System/sensing.h"
#include"System/flags.h"

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
	 MF.FLAG.CALLBACK = OFF;
	TIM3->CNT = 0;
	TIM4->CNT = 0;
	enableEncoder();
	printf("Utsutsu : Select Mode \n");

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

	Melody(e7,100);
	Melody(f7,100);
	Melody(g7,100);

	disableEncoder();
	TIM3->CNT = 0;
	TIM4->CNT = 0;
	printf(" Enter : mode %2d\n",mode);
	return mode;
}

/*
 * @brief 前壁センサに手をかざすまで待機
 */
void waitStarting(void)
{
	//安全のためのモータが動かない用の設定
	shutdownMotors();

	MF.FLAG.CALLBACK = 1;
	printf("Ready???\n");
	while(1){
		if(sensor.wall_val[FF] > WALL_START_VALUE){
			Melody(e6,300);
			Melody(f6,300);
			Melody(g6,300);
			waitMs(1000);
			break;
		}
	}
	 MF.FLAG.CALLBACK = OFF;
	 mouse.angle = 0.0f;
}
/*
 * @brief 各種フェイルセーフをチェック、場合によっては呼び出す
 */
void judgeFailSafe(void)
{
	float error_angle = target.angle - mouse.angle;
	float error_velocity_r = target.velocity - sensor.encoder_vel_r;
	float error_velocity_l = target.velocity - sensor.encoder_vel_l;
	uint8_t error_code = 0;

//	if(readBatteryCheck() == 0)							error_code = 1;
	if( fabs(error_angle) > FAIL_ANGLE) 			error_code = 2;
	if(fabs(error_velocity_l) > FAIL_VELOCITY)	error_code = 3;
	if(fabs(error_velocity_r) > FAIL_VELOCITY)	error_code = 4;

	if(error_code != 0)		errorFunctions(error_code);
}
/*
 * @brief フェイルセーフ関数、呼び出されたら無限ループで終了
 */
void errorFunctions(uint8_t error_code)
{
	shutdownMotors();
	basicTimerPause();
	displayBinaryValueWithLEDs(0x00);
	changeLEDs(OFF, OFF, OFF, OFF, OFF);
	waitMs(500);
	MelodyMrLawrence();
	while(1){
		printf("Error Code : %d, target: %f, angle: %f\n",
				error_code,target.angle,mouse.angle);
		waitMs(1000);
	}
}
