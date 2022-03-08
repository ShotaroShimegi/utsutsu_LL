/*
 * mode.h
 *
 *  Created on: 2022/02/28
 *      Author: sssho
 */

#ifndef INC_SYSTEM_MODE_H_
#define INC_SYSTEM_MODE_H_

#include<stdint.h>

//モード選択関連の閾値
#define RIGHT_RESOLUTION 		16
#define LEFT_RESOLUTION 		3
#define WALL_START_VALUE 	130

//フェイルセーフ関連
#define FAIL_ANGLE 			30.0f
#define FAIL_VELOCITY		2.0f

uint8_t modeSelect(void);
void waitStarting(void);

void judgeFailSafe(void);
void testWallSensors(void);
void testEstimationSensors(void);

#endif /* INC_SYSTEM_MODE_H_ */
