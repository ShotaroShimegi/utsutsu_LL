/*
 * wall_sensor.h
 *
 *  Created on: 2021/07/16
 *      Author: Shotaro Shimegi
 */

#ifndef INC_HARDWARE_WALL_SENSOR_H_
#define INC_HARDWARE_WALL_SENSOR_H_

#include <stdint.h>

typedef enum {
	LOW,
	HIGH
} MY_LED_STATES;

void changeFrontCenterStatus(uint8_t state);
void changeFrontRightStatus(uint8_t state);
void changeFrontLeftStatus(uint8_t state);
void changeRightStatus(uint8_t state);
void changeLeftStatus(uint8_t state);

#endif /* INC_HARDWARE_WALL_SENSOR_H_ */
