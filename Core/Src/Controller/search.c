/*
 * search.c
 *
 *  Created on: Feb 21, 2022
 *      Author: sssho
 */
#include "Controller/search.h"
#include "Controller/state.h"

#include"System/map.h"
#include"System/route.h"
#include"System/flags.h"
#include"System/music.h"
#include"System/motion.h"
#include"System/sensing.h"

#include"Hardware/basic_timer.h"
#include"Hardware/interface_LED.h"
#include"Hardware/motor.h"

#include<stdio.h>

void rotateSafteyR180(void){
	MF.FLAG.SAFETY = OFF;
	spinRight180();
	mouse.angle = 0.0f;
	target.angle = 0.0f;
	MF.FLAG.SAFETY = ON;
}

void prepareStateForSearching(void)
{
	if(MF.FLAG.FIRST){
		MF.FLAG.CTRL = OFF;
		mouse.mileage = 0.0f;
		MF.FLAG.SAFETY = OFF;
		target.angle = 0.0f;
		mouse.angle = 0.0f;
		MF.FLAG.SAFETY = ON;
	}
}

void searchMazeBySlalom(uint8_t goal_length)
{
	uint8_t rotate_on_map = 0x00;
	uint8_t wall_info;
	uint8_t set_flag = 0x00;

	prepareMapForSearch();
	prepareStateForSearching();
	MF.FLAG.CALLBACK = 1;
	mouse.run_state = 1;
	if(sensor.wall_val[FF] > WALL_TURN_VALUE){
		rotateSafteyR180();
		changeDirection(DIR_SPIN_180);
	}

	if(goal.x == GOAL_X && goal.y == GOAL_Y)	driveAccelMotion(SET_MM,max.velocity,ON);
	wall_info = moveHalfSectionAccel(ON, ON);
	advancePosition();
	map_count.route++;
	ConfRoute_NESW(goal_length,wall_info);

	//====探索走行====
	do{
		switch(route[map_count.route]){
		//route配列によって進行を決定。経路カウンタを進める
			case STRAIGHT:
				wall_info = moveOneSectionAccel(ON);
				rotate_on_map = 0x00;
				break;

			case TURN_RIGHT:
				wall_info = moveSlalomR90();
				rotate_on_map = DIR_SPIN_R90;
				changeDirection(rotate_on_map);
				break;

			case TURN_BACK:
				moveHalfSectionDecel(ON);
				if(sensor.wall_val[FR] > WALL_BACK_FL && sensor.wall_val[FR] > WALL_BACK_FR)	set_flag = 1;
				rotateSafteyR180();
				if(set_flag == 1){
					set_flag = 0;
					backMotion(SET_MM * 0.9f);
					driveAccelMotion(SET_MM,max.velocity,ON);
				}
				rotate_on_map = DIR_SPIN_180;
				changeDirection(rotate_on_map);
				wall_info = moveHalfSectionAccel(ON, ON);
				break;
			case TURN_LEFT:
				wall_info = moveSlalomL90();
				rotate_on_map = DIR_SPIN_L90;
				changeDirection(rotate_on_map);
				break;
		}
		map_count.route++;
		advancePosition();
		displayBinaryValueWithLEDs(wall_info);
		ConfRoute_NESW(goal_length,wall_info);

	}while(CheckGoal(point.x,point.y,goal_length) != GOAL_OK);

//	＝＝＝＝ゴール処理＝＝＝＝
	moveHalfSectionDecel(ON);
	MF.FLAG.CALLBACK = OFF;
	mouse.run_state = OFF;
	shutdownMotors();
	waitMs(200);
	if(MF.FLAG.SCND == 0)  saveWallMap();
	MF.FLAG.FIRST = 0;
	MelodyGoal();
}

uint8_t CheckGoal(uint8_t x, uint8_t y, uint8_t length)
{
	uint8_t goal_check_x = GOAL_FAIL;
	uint8_t goal_check_y = GOAL_FAIL;
	uint8_t i = 0;
	uint8_t x_flag = 0;
	uint8_t y_flag = 0;
	uint8_t new_goal_x = 0;
	uint8_t new_goal_y = 0;

	for(i=0;i<length;i++){
		new_goal_x = goal.x + i;
		new_goal_y = goal.y + i;

		if((x == new_goal_x)&& (x_flag == 0)){
			goal_check_x = GOAL_OK;
			x_flag = 1;
		}else if(x_flag == 0){
			goal_check_x = GOAL_FAIL;
		}
		if((y == new_goal_y) && (y_flag == 0)){
			goal_check_y = GOAL_OK;
			y_flag = 1;
		}else if(y_flag == 0){
			goal_check_y = GOAL_FAIL;
		}
	}

	if((goal_check_x == GOAL_OK) && (goal_check_y == GOAL_OK)) {
		MF.FLAG.FIRST = 0;
		return GOAL_OK;
	}
	else{
		return GOAL_FAIL;
	}
}

