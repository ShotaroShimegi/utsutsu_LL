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
#define RETURN_GOAL_LENGTH 1
#define MAZE_SIZE 16
#define MAX_NODE 256

#define STRAIGHT 0x88
#define TURN_RIGHT 0x44
#define TURN_BACK 0x22
#define TURN_LEFT 0x11

//忘れた、意味
#define NORTH 0x00
#define EAST 	0x01
#define SOUTH 	0x02
#define WEST 	0x03

//内部情報回転方向
#define DIR_SPIN_FORWARD		0x00
#define DIR_SPIN_R90				0x01
#define DIR_SPIN_L90				0xff
#define DIR_SPIN_180				0x02

typedef struct{
	uint8_t x;
	uint8_t y;
	uint8_t dir;
}MAP_Mouse_Typedef;

typedef struct{
	uint16_t step;
	uint16_t route;
}MAP_Count_Typedef;

extern MAP_Mouse_Typedef point;
extern MAP_Mouse_Typedef goal;
extern MAP_Count_Typedef map_count;

extern 	uint16_t wall_vertical[MAZE_SIZE + 1];
extern	uint16_t wall_horizontal[MAZE_SIZE + 1];
extern uint8_t wall_map[MAZE_SIZE][MAZE_SIZE];
extern uint16_t step_map[MAZE_SIZE][MAZE_SIZE];
extern uint8_t route[MAX_NODE];

void deleteWallMap();
uint16_t makeStepMap(uint8_t);
void updateWallMap(uint8_t wall_info);

void PrintWallData();

MAP_Mouse_Typedef setMapStruct(uint8_t x, uint8_t y, uint8_t dir);

void MakeRoute_NESW();
void ConfRoute_NESW(uint8_t goal_size, uint8_t wall_data);

void advancePosition();
void changeDirection(uint8_t rotate_hex);

void saveWallMap(void);
void loadWallMap(void);

void prepareMapForSearch(void);

#endif /* INC_SYSTEM_MAP_H_ */
