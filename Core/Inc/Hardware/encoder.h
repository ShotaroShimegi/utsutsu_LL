/*
 * encoder.h
 *
 *  Created on: 2021/07/16
 *      Author: Shotaro Shimegi
 */

#ifndef INC_HARDWARE_ENCODER_H_
#define INC_HARDWARE_ENCODER_H_

#include"tim.h"

#define		LEFT		0xf0
#define		RIGHT	0x0f

void enableEncoder(void);
int16_t getEncoderData(TIM_TypeDef *TIMx);

#endif /* INC_HARDWARE_ENCODER_H_ */
