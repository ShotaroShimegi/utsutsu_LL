/*
 * buzzer.h
 *
 *  Created on: 2021/07/16
 *      Author: Shotaro Shimegi
 */

#include"stdint.h"

#ifndef INC_HARDWARE_BUZZER_H_
#define INC_HARDWARE_BUZZER_H_

void enableMelody(void);
void disableMelody(void);
void Melody(uint16_t,uint16_t);

#endif /* INC_HARDWARE_BUZZER_H_ */
