/*
 * search.h
 *
 *  Created on: Feb 21, 2022
 *      Author: sssho
 */
#include <stdint.h>


#ifndef INC_SYSTEM_SEARCH_H_
#define INC_SYSTEM_SEARCH_H_

#define GOAL_OK 0
#define GOAL_FAIL 1

#define WALL_TURN_VALUE 120

void rotateSafteyR180(void);
void searchMazeBySlalom(uint8_t);
uint8_t CheckGoal(uint8_t,uint8_t,uint8_t);

#endif /* INC_SYSTEM_SEARCH_H_ */
