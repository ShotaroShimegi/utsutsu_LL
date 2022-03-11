/*
 * test.h
 *
 *  Created on: 2022/03/11
 *      Author: sssho
 */
#include<stdint.h>

#ifndef INC_CONTROLLER_TEST_H_
#define INC_CONTROLLER_TEST_H_

void prepareForTest(uint8_t safety_flag,uint8_t motor_enble);
void testWallSensors(void);
void testEncoders(void);
void testIMU(void);
void testMotions(void);

#endif /* INC_CONTROLLER_TEST_H_ */
