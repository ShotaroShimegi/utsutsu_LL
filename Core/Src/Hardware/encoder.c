/*
 * encoder.c
 *
 *  Created on: 2021/07/17
 *      Author: sssho
 */

#include "tim.h"
#include "Hardware/encoder.h"


/**
 * enableEncoder
* @brief　エンコーダ機能の有効化
*/
void enableEncoder(void)
{
	LL_TIM_EnableCounter(TIM3);
	LL_TIM_EnableCounter(TIM4);
}
/**
 * getEncoderData
* @brief エンコーダのパルスカウントを取得
* @param *TIMx 取得したいエンコーダタイマ、左:TIM3,  右: TIM4
* @return パルスカウント数
*/
int16_t getEncoderData(TIM_TypeDef *TIMx)
{
	int16_t count = 0;
	uint16_t unsigned_count = TIMx->CNT;

	TIMx->CNT = 0;

	if(unsigned_count > 32767)	count = -(65536 - unsigned_count);
	else										count = (int16_t)unsigned_count;

	return count;
}

