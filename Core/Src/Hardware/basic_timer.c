/*
 * basic_timer.c
 *
 *  Created on: Apr 21, 2021
 *      Author: sssho
 */

#include "Hardware/basic_timer.h"
#include "tim.h"

#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_utils.h"

/**
* @brief タイマ割込みのスタート
*/
void basicTimerStart(void)
{
	LL_TIM_EnableIT_UPDATE(TIM5);
	LL_TIM_EnableCounter(TIM5);
}

/**
* @brief タイマ割込みの停止
*/
void basicTimerPause(void)
{
	LL_TIM_DisableCounter(TIM5);
	LL_TIM_DisableIT_UPDATE(TIM5);
}

/**
* @brief　	特定時間待機する関数
* @param	ms	待機する時間 [msec]
*/
void waitMs(uint16_t ms)
{
	LL_mDelay(ms);
}

/**
* @brief　	特定時間待機する関数
* @param	us	待機する時間 [usec]
*/
void waitUs(uint16_t us)
{
	if(us > 999)	us = 999;
	volatile uint16_t count = TIM6->CNT + us;
	LL_TIM_EnableCounter(TIM6);
	while(TIM6->CNT < count);
	LL_TIM_DisableCounter(TIM6);
	TIM6->CNT = 0;
}
