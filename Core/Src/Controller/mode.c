/*
 * mode.c
 *
 *  Created on: 2022/02/28
 *      Author: sssho
 */
#include<stdio.h>

#include"Controller/mode.h"

#include"System/music.h"

#include"Hardware/encoder.h"
#include"Hardware/interface_LED.h"
#include"Hardware/buzzer.h"

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
	TIM3->CNT = 0;
	TIM4->CNT = 0;
	enableEncoder();

	//モード(0~15)を確定するwhileループ
	do{
		//エンコーダからの値取得と単位変換
		right_dial_val = (float)TIM4->CNT;
		left_dial_val = (float)TIM3->CNT;
		mode = (float)(right_dial_val /65535) * RIGHT_RESOLUTION;
		enter = (float)(left_dial_val / 65535) * LEFT_RESOLUTION;
		//前ループとの偏差からモード設定の変更と確定を判断
		error = mode - pre_mode;
		if(error != 0){
			printf(" mode:%2d, enter : %2d, %lf\n", mode,enter,left_dial_val);
			displayBinaryValueWithLEDs(mode);
			Melody(c6 + (60 * mode),100);
		}
		pre_mode = mode;
	}while(enter != 2);

	printf(" Enter : mode %2d\n",mode);
	return mode;
}


