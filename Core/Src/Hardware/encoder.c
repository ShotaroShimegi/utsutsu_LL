/*
 * encoder.c
 *
 *  Created on: 2021/07/17
 *      Author: sssho
 */

#include "tim.h"
#include "Hardware/encoder.h"


/**
* @brief
* @param argument1
* @param (As many as the number of arguments)
* @return
* (@sa Functions to should refer to)
* @details
*/

void enableEncoder(void)
{
	LL_TIM_EnableCounter(TIM3);
	LL_TIM_EnableCounter(TIM4);
}

int16_t getEncoderData(TIM_TypeDef *TIMx)
{
	int16_t count = 0;
	uint16_t unsigned_count = TIMx->CNT;

	TIMx->CNT = 0;

	if(unsigned_count > 32767)	count = -(65535 - unsigned_count);
	else										count = (int16_t)unsigned_count;

	return count;
}

