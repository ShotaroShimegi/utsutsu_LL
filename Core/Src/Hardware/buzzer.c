/*
 * buzzer.c
 *
 *  Created on: 2021/07/17
 *      Author: sssho
 */

#include "Hardware/buzzer.h"
#include"Hardware/basic_timer.h"

#include"stm32f4xx_ll_tim.h"

/**
* @brief
* @param argument1
* @param (As many as the number of arguments)
* @return
* (@sa Functions to should refer to)
* @details
*/

void enableMelody(void){
	LL_TIM_CC_EnableChannel(TIM8, LL_TIM_CHANNEL_CH4);
	LL_TIM_EnableCounter(TIM8);

	LL_TIM_EnableAllOutputs(TIM8);

}

void disableMelody(void){
	LL_TIM_CC_DisableChannel(TIM8, LL_TIM_CHANNEL_CH4);
	LL_TIM_DisableCounter(TIM8);
}

void stopMelody(void){
	LL_TIM_OC_SetCompareCH4(TIM8, 0);
}

void Melody(uint16_t hz, uint16_t ms)
{
	LL_TIM_SetAutoReload(TIM8,100000/hz);
	LL_TIM_OC_SetCompareCH4(TIM8, 95000 / hz );
	LL_TIM_SetCounter(TIM8, 0);
	waitMs(ms);
	stopMelody();
}
