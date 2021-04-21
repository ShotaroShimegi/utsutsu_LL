/*
 * wrap_tim.h
 *
 *  Created on: 2021/04/21
 *      Author: sssho
 */

#ifndef INC_PERIPHERAL_WRAP_TIM_H_
#define INC_PERIPHERAL_WRAP_TIM_H_

#include"stm32f405xx.h"

void timerCountStart(TIM_TypeDef *TIMx);
void timerCountPause(TIM_TypeDef *TIMx);
void pwmOutputStart(TIM_TypeDef *TIMx,uint32_t Channels);
void changeOutputPulse(TIM_TypeDef *TIMx, uint16_t pulse);
void changeOutputPeriod(TIM_TypeDef *TIMx, uint16_t period);

#endif /* INC_PERIPHERAL_WRAP_TIM_H_ */
