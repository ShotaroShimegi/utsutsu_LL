/*
 * encoder.h
 *
 *  Created on: 2021/07/16
 *      Author: Shotaro Shimegi
 */

#ifndef INC_HARDWARE_ENCODER_H_
#define INC_HARDWARE_ENCODER_H_

#include"tim.h"

#define LEFT 	0xf0
#define RIGHT 	0x0f

void enableEncoder(void);
void disableEncoder(void);
int16_t getEncoderData(uint8_t dir);

#endif /* INC_HARDWARE_ENCODER_H_ */
