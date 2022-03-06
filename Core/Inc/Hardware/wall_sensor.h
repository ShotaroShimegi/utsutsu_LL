/*
 * wall_sensor.h
 *
 *  Created on: 2021/07/16
 *      Author: Shotaro Shimegi
 */

#ifndef INC_HARDWARE_WALL_SENSOR_H_
#define INC_HARDWARE_WALL_SENSOR_H_

#include <stdint.h>
#include <stdbool.h>

void startADCwithDMA(void);
void pauseADCwithDMA(void);

void DMA_CallbackTask(void);
uint16_t getWallADC(uint8_t number);

void changeLEDs(uint8_t fl,uint8_t l, uint8_t ff, uint8_t r,uint8_t fr);

#endif /* INC_HARDWARE_WALL_SENSOR_H_ */
