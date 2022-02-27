/*
 * wall_sensor.h
 *
 *  Created on: 2021/07/16
 *      Author: Shotaro Shimegi
 */

#ifndef INC_HARDWARE_WALL_SENSOR_H_
#define INC_HARDWARE_WALL_SENSOR_H_

#include <stdint.h>

void startADCwithDMA(void);
void pauseADCwithDMA(void);

void DMA_CallbackTask(void);
uint16_t getWallADC(uint8_t number);

void changeFrontCenterLED(uint8_t state);
void changeFrontRightLED(uint8_t state);
void changeFrontLeftLED(uint8_t state);
void changeRightLED(uint8_t state);
void changeLeftLED(uint8_t state);

#endif /* INC_HARDWARE_WALL_SENSOR_H_ */
