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

void SearchSlalom(uint8_t goal_length)
{
//	uint8_t fix_flag = 0;
	uint8_t rotate_on_map = 0x00;
	uint8_t wall_info;
/*
	if(MF.FLAG. == 1)	InitializeMap();

	//====歩数等初期化====
	m_step = route_count = 0;							//歩数と経路カウンタの初期化
	WriteMap();											//地図の初期化
	MakeStepMap(goal_length);						//歩数図の初期化
	MakeRoute_NESW();								//最短経路探索(route配列に動作が格納される)
*/
	prepareMapForSearch();
	basicTimerStart();

/*	if(sensor.wall_ff > WALL_TURN_VALUE){
		Spin180();
		waitMs(100);
		updateDirection(DIR_SPIN_180);
		if(wall_ff.val > wall_ff.threshold)		FixPosition(0);
	}
*/

	MF.FLAG.CTRL = 0;

	if(goal.x == 0 && goal.y == 0)		driveAccelMotion(0.0f, max.velocity,OFF);
	else											driveAccelMotion(SET_MM,max.velocity,OFF);

	wall_info = moveHalfSectionAccel(0, 1);
	advancePosition();
	map_count.route++;
	ConfRoute_NESW(goal_length,wall_info);

	//====探索走行====
	do{
		switch(route[map_count.route++]){								//route配列によって進行を決定。経路カウンタを進める
			case STRAIGHT:
				wall_info = moveOneSectionAccel(ON);
				rotate_on_map = 0x00;
				break;

			case TURN_RIGHT:
				wall_info = moveSlalomR90();
				rotate_on_map = DIR_SPIN_R90;
				break;

			case TURN_BACK:
//				if(wall_ff.val > WALL_TURN_VALUE)		fix_flag = 1;
				moveHalfSectionDecel(0);
				spinRight180();
				waitMs(100);
				rotate_on_map = DIR_SPIN_180;
/*				if(fix_flag == 1){
					FixPosition(0);
					fix_flag = 0;
				}
*/
				wall_info = moveHalfSectionAccel(OFF, ON);
				break;
			case TURN_LEFT:
				wall_info = moveSlalomL90();
				rotate_on_map = DIR_SPIN_L90;
				break;
		}
		changeDirection(rotate_on_map);
		advancePosition();
		ConfRoute_NESW(goal_length,wall_info);
	}while(CheckGoal(point.x,point.y,goal_length) != GOAL_OK);

	moveHalfSectionDecel(OFF);
	basicTimerPause();
	waitMs(100);
//	if(MF.FLAG.SCND == 0)  SaveMapInEeprom();
	waitMs(100);
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
	else		{
		return GOAL_FAIL;
	}
}
