/*
 * route.h
 *
 *  Created on: 2022/03/06
 *      Author: sssho
 */
#include<stdint.h>

#ifndef INC_SYSTEM_ROUTE_H_
#define INC_SYSTEM_ROUTE_H_

#define NORTH 0x00
#define EAST 	0x01
#define SOUTH 	0x02
#define WEST 	0x03

#define DIR_SPIN_R90	0x01
#define DIR_SPIN_L90	0xff
#define DIR_SPIN_180	0x02

void MakeRoute_NESW();
void ConfRoute_NESW(uint8_t);

void updatePosition();
void updateDirection(uint8_t);

#endif /* INC_SYSTEM_ROUTE_H_ */
