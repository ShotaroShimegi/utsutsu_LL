/*
 * basic_timer.h
 *
 *  Created on: Apr 21, 2021
 *      Author: sssho
 */

#ifndef INC_HARDWARE_BASIC_TIMER_H_
#define INC_HARDWARE_BASIC_TIMER_H_

#include"stm32f405xx.h"
#include"stdint.h"

void basicTimerCallback(void);
void basicTimerStart(void);
void basicTimerPause(void);
void waitMs(uint16_t ms);

#endif /* INC_HARDWARE_BASIC_TIMER_H_ */
