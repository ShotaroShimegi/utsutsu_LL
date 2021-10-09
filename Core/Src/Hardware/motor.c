/*
 * motor.c
 *
 *  Created on: 2021/07/17
 *      Author: sssho
 */
#include <stdint.h>
#include <stdio.h>

#include "main.h"
#include"Hardware/motor.h"
#include<Hardware/interface_LED.h>

#define ENABLE 	1
#define DISABLE	0

uint8_t setMotorDriverState(uint8_t state){
	if(state == ENABLE){
		LL_GPIO_SetOutputPin(STBY_GPIO_Port, STBY_Pin);
		return ENABLE;
	}else if(state == DISABLE){
		LL_GPIO_ResetOutputPin(STBY_GPIO_Port, STBY_Pin);
		return DISABLE;
	}
	return 0xff;
}

/**
* @brief モータ関連の初期化
*/
void initMotors(void){

	uint8_t md_enable = setMotorDriverState(ENABLE);
	printf("md = %d\r\n",md_enable);

	LL_GPIO_SetOutputPin(LEFT_DIR1_GPIO_Port, LEFT_DIR1_Pin);
	LL_GPIO_ResetOutputPin(LEFT_DIR2_GPIO_Port, LEFT_DIR2_Pin);

	LL_GPIO_SetOutputPin(RIGHT_DIR1_GPIO_Port,RIGHT_DIR1_Pin);
	LL_GPIO_ResetOutputPin(RIGHT_DIR2_GPIO_Port, RIGHT_DIR2_Pin);

	LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH2);
	LL_TIM_EnableCounter(TIM2);
//	LL_TIM_SetAutoReload(TIM2,100-1);

	LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);
	LL_TIM_EnableCounter(TIM1);
//	LL_TIM_SetAutoReload(TIM1,100-1);

	LL_TIM_OC_SetCompareCH2(TIM2, 50);
	LL_TIM_OC_SetCompareCH1(TIM1, 50);

	LL_TIM_EnableAllOutputs(TIM2);
	LL_TIM_EnableAllOutputs(TIM1);


}

/**
* @brief　デューティ比によってモータの回転方向を設定
* @param r_duty 右側モータのデューティ比
* @param l_duty 左側モータのデューティ比
*/
void setMotorDirection(float r_duty, float l_duty){

	if (l_duty < 0){
		LEFT_DIR1_GPIO_Port->BSRR = (LEFT_DIR1_Pin << 16);
		LEFT_DIR2_GPIO_Port->BSRR = LEFT_DIR2_Pin;
	}else{
		LEFT_DIR1_GPIO_Port->BSRR = LEFT_DIR1_Pin;
		LEFT_DIR2_GPIO_Port->BSRR = (LEFT_DIR2_Pin << 16);
	}

	if (r_duty < 0){
		RIGHT_DIR1_GPIO_Port->BSRR = (RIGHT_DIR1_Pin << 16);
		RIGHT_DIR2_GPIO_Port->BSRR = RIGHT_DIR2_Pin;
	}else{
		RIGHT_DIR1_GPIO_Port->BSRR = RIGHT_DIR1_Pin;
		RIGHT_DIR2_GPIO_Port->BSRR = (RIGHT_DIR2_Pin << 16);
	}
}

/**
* @brief 最大・最小の範囲で値が範囲外なら再設定して出力
* @param 	max 		最大値
* @param 	min 		最小値
* @param 	check_val 	チェックしたい変数
* @return 	return		再設定した出力
*/

float dutyConverter(float max, float min, float check_val) {
	if(check_val < 0)			check_val = -check_val;

	if (check_val > max) 		return max;
	else if (check_val < min)	return min;
	else						return check_val;
}

/**
* @brief 最大・最小の範囲で値が範囲外なら再設定して出力
* @param 	r_duty		右のデューティ
* @param 	l_duty 		左のデューティ
* @return   上手くいったら　0　返す
*/

void driveMotors(float r_duty, float l_duty){

	setMotorDirection(r_duty, l_duty);

	r_duty = dutyConverter(0.8f, 0.01f, r_duty);
	l_duty = dutyConverter(0.8f, 0.01f, l_duty);

	LL_TIM_OC_SetCompareCH1(TIM1, (100-1)*l_duty);
	LL_TIM_OC_SetCompareCH2(TIM2, (100-1)*r_duty);

	LL_TIM_SetCounter(TIM1, 0);
	LL_TIM_SetCounter(TIM2, 0);

}

/**
* @brief 	走行用ドライブモータを停止する
* @return   上手くいったら　0　返す
*/
void shutdownMotors(void) {

	uint8_t md_enable = setMotorDriverState(DISABLE);

	LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH1);
	LL_TIM_CC_DisableChannel(TIM2, LL_TIM_CHANNEL_CH2);
	LL_TIM_DisableCounter(TIM1);
	LL_TIM_DisableCounter(TIM2);

}
