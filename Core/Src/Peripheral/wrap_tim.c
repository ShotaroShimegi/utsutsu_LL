/*
 * wrap_tim.c
 *
 *  Created on: 2021/04/21
 *      Author: sssho
 */

#include "Peripheral/wrap_tim.h"
#include "stm32f4xx_ll_tim.h"

void timerCountStart(TIM_TypeDef *TIMx)
{
	LL_TIM_EnableIT_UPDATE(TIMx);
	LL_TIM_EnableCounter(TIMx);
}

void timerCountPause(TIM_TypeDef *TIMx)
{
	LL_TIM_DisableCounter(TIMx);
	LL_TIM_DisableIT_UPDATE(TIMx);
}

void pwmOutputStart(TIM_TypeDef *TIMx, uint32_t Channels)
{
	LL_TIM_CC_EnableChannel(TIMx, Channels);
	LL_TIM_EnableCounter(TIMx);
}

void changeOutputPulse(TIM_TypeDef *TIMx, uint16_t pulse)
{
	LL_TIM_OC_SetCompareCH1(TIMx, pulse);
}
void changeOutputPeriod(TIM_TypeDef *TIMx, uint16_t period)
{
	LL_TIM_SetCounter(TIMx, period);
}
