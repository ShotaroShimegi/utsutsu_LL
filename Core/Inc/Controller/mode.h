/*
 * mode.h
 *
 *  Created on: 2022/02/28
 *      Author: sssho
 */

#ifndef INC_SYSTEM_MODE_H_
#define INC_SYSTEM_MODE_H_

#include<stdint.h>

#define RIGHT_RESOLUTION 		15
#define LEFT_RESOLUTION 		3

uint8_t modeSelect(void);
void checkWallSensor(void);
void checkSensors(void);

#endif /* INC_SYSTEM_MODE_H_ */
