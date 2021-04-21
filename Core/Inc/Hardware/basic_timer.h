/*
 * basic_timer.h
 *
 *  Created on: Apr 21, 2021
 *      Author: sssho
 */

#ifndef INC_HARDWARE_BASIC_TIMER_H_
#define INC_HARDWARE_BASIC_TIMER_H_

#include"stm32f405xx.h"

void basicTimerCallback(void);
void basicTimerStart(void);
void basicTimerPause(void);

#endif /* INC_HARDWARE_BASIC_TIMER_H_ */
