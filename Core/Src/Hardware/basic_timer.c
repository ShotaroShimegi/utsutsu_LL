/*
 * basic_timer.c
 *
 *  Created on: Apr 21, 2021
 *      Author: sssho
 */

#include "Hardware/basic_timer.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_utils.h"

void basicTimerCallback(void)
{
	return;
}

void basicTimerStart(void)
{
	LL_TIM_EnableIT_UPDATE(TIM6);
	LL_TIM_EnableCounter(TIM6);
}

void basicTimerPause(void)
{
	LL_TIM_DisableCounter(TIM6);
	LL_TIM_DisableIT_UPDATE(TIM6);
}

void WaitMs(uint16_t ms){
	LL_mDelay(ms);
}
