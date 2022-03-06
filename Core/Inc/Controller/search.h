/*
 * search.h
 *
 *  Created on: Feb 21, 2022
 *      Author: sssho
 */
#include <stdint.h>


#ifndef INC_SYSTEM_SEARCH_H_
#define INC_SYSTEM_SEARCH_H_

#define STRAIGHT 0x88
#define TURN_RIGHT 0x44
#define TURN_BACK 0x22
#define TURN_LEFT 0x11

#define GOAL_OK 0
#define GOAL_FAIL 1

void SearchSlalom(uint8_t);
uint8_t CheckGoal(uint8_t,uint8_t,uint8_t);

#endif /* INC_SYSTEM_SEARCH_H_ */
