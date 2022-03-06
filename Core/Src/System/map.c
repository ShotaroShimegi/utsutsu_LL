/*
 * map.c
 *
 *  Created on: Feb 21, 2022
 *      Author: sssho
 */
#include"System/map.h"

#include"stdio.h"

MAP_Mouse_Typedef mouse_on_map;

uint16_t wall_vertical[MAZE_SIZE + 1];
uint16_t wall_horizontal[MAZE_SIZE + 1];
uint8_t map[MAZE_SIZE][MAZE_SIZE];
uint8_t step_map[MAZE_SIZE][MAZE_SIZE];

void ConvertMapIntoWall()
{
	uint8_t x,y;
	//絶対壁を配置
	wall_horizontal[MAZE_SIZE] = 0xffff;
	wall_vertical[MAZE_SIZE] = 0xffff;

	for(y=0;y<MAZE_SIZE;y++){
		for(x=0;x<MAZE_SIZE;x++){
			wall_horizontal[y] += (((map[y][x] & 0x02) >> 1) << x);
			wall_vertical[x] +=  ((map[y][x] & 0x01) << y);
		}
	}
}

void printHorizontalWallData()
{
	uint8_t y;
	printf("horizontal\n");
	for(y=16;y>=0;y--){
		printf("%d, %#x\n",y,wall_horizontal[y]);
	}

}
void printVerticalWallData()
{
	uint8_t x;
	printf("vertical\n");
		for(x=0;x<17;x++){
			printf("%d, %#x\n",x,wall_vertical[x]);
		}
}

/*
 * @brief 壁情報表示用関数
 */
void printWallData()
{
	int8_t x,y;
	for(y=MAZE_SIZE;y>0;y--){
		printf("+");
		for(x=0;x<MAZE_SIZE;x++){
			if(wall_horizontal[y] & (0x01 << x))		printf("---");
			else													printf("   ");
			printf("+");
		}
		printf("\n");
		for(x=0;x<MAZE_SIZE+1;x++){
			if(wall_vertical[x] & (0x01 << (y-1)))	printf("|   ");
			else													printf("    ");
		}
		printf("\n");
	}
	printf("+");
	for(x=0;x<MAZE_SIZE;x++){
		if(wall_horizontal[0] & (0x01 << x))	printf("---");
		else												printf("   ");
		printf("+");
	}
	printf("\n");
}
