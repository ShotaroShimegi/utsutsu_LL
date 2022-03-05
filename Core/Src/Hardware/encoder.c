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
	TIM3->CNT = 0;
	TIM4->CNT = 0;
	LL_TIM_EnableCounter(TIM3);
	LL_TIM_EnableCounter(TIM4);
}
/* disableEncoder
* @brief　エンコーダ機能の有効化
*/
void disableEncoder(void)
{
	LL_TIM_DisableCounter(TIM3);
	LL_TIM_DisableCounter(TIM4);

}

/**
 * getEncoderData
* @brief エンコーダのパルスカウントを取得
* @param *TIMx 取得したいエンコーダタイマ、左:TIM3,  右: TIM4
* @return パルスカウント数
*/
int16_t getEncoderData(uint8_t dir)
{
	int16_t count = 0;
	uint16_t unsigned_count;
	if(dir == 0xf0)	{
		unsigned_count= TIM3->CNT;
		TIM3->CNT = 0;
	}else if(dir == 0x0f){
		unsigned_count= TIM4->CNT;
		TIM4->CNT = 0;
	}

	if(unsigned_count > 32767)	count = (int16_t)(65536 - unsigned_count) * -1;
	else										count = (int16_t)unsigned_count;

	return count;
}

