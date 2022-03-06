/*
 * map.h
 *
 *  Created on: Feb 21, 2022
 *      Author: sssho
 */
#include<stdint.h>

#ifndef INC_SYSTEM_MAP_H_
#define INC_SYSTEM_MAP_H_

#define GOAL_X 7
#define GOAL_Y 7
#define GOAL_LENGTH 2
#define MAZE_SIZE 16

typedef struct{
	uint8_t x;
	uint8_t y;
	uint8_t dir;
}MAP_Mouse_Typedef;

extern MAP_Mouse_Typedef mouse_on_map;

extern 	uint16_t wall_vertical[MAZE_SIZE + 1];
extern	uint16_t wall_horizontal[MAZE_SIZE + 1];
extern uint8_t map[MAZE_SIZE][MAZE_SIZE];
extern uint8_t step_map[MAZE_SIZE][MAZE_SIZE];

void InitializeMap();
void MakeStepMap(uint8_t);
void WriteMap();

void ConvertMapIntoWall();
void PrintWallData();

#endif /* INC_SYSTEM_MAP_H_ */
