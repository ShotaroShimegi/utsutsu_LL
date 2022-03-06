/*
 * route.c
 *
 *  Created on: 2022/03/06
 *      Author: sssho
 */
#include<stdio.h>

#include"System/route.h"
#include"System/map.h"
#include"System/sensing.h"
#include"System/flags.h"
#include"System/music.h"
#include"System/motion.h"

#include"Hardware/basic_timer.h"
#include "Hardware/motor.h"
#include"Hardware/interface_LED.h"

void ReadPass(void)
{
	uint16_t i = 0;
	uint8_t buff;

	MF.FLAG.CTRL = 1;
	DriveAccel(SET_MM,FRONT_CONTROL_FLAG);

	while(i != pass_end_count){
		if(pass[i] > 0)	{
			buff = 0x00;
			LedDisplay(&buff);
			MF.FLAG.CTRL = 1;
			DriveTrapezoid(HALF_MM *(float)pass[i],params_now.big_vel_max,params_now.vel_max,params_now.accel);
		}else{
			switch(pass[i]){
				case R90:
					buff = 0x10;
					LedDisplay(&buff);
					SlalomR90();
					break;
				case L90:
					buff = 0x01;
					LedDisplay(&buff);
					SlalomL90();
					break;
				case BIG_R90:
					buff = 0x08;
					LedDisplay(&buff);

					BigSlalomR90();
					break;
				case BIG_L90:
					buff = 0x02;
					LedDisplay(&buff);

					BigSlalomL90();
					break;
				case BIG_R180:
					buff = 0x18;
					LedDisplay(&buff);
					BigSlalomR180();
					break;
				case BIG_L180:
					buff = 0x03;
					LedDisplay(&buff);

					BigSlalomL180();
					break;
				default:
					FailSafe();
					break;
			}
		}
		i++;
	}
	HalfSectionDecel();
	printf("PASS END\n");
	shutdownMotors();
	basicTimerPause();
	MelodyGoal();

}

void MakePass(void)
{
	uint16_t j = 0;
	uint16_t pass_count = 0;
	uint16_t route_count = 1;
//	uint16_t show_counter;
	uint16_t big_turn_flag = 0;
	uint16_t normal_turn_flag = 0;
	uint16_t return_flag = 0;
	int8_t half_straight_count = 0;				//First Action is Always STRAIGHT

	for(pass_count=0;pass_count<1024;pass_count++){
		pass[pass_count] = 100;
	}
	half_straight_count = 1;
	pass_count = 0;

	j = route_count;
	while(route[j] == STRAIGHT){
		half_straight_count += 2;
		j++;
	}
	pass[pass_count] = half_straight_count;
	pass_count++;

	half_straight_count = 0;
	if((j-1) > 0)	route_count = j-1;			//Next is Straight
	else			route_count = j;

	normal_turn_flag = 1;

	if(route[route_count+1] == 0xff)	return_flag = 1;

	while(1){

/*		printf("route_count : %d\n",route_count);
		printf("half_straight_count : %d\n",half_straight_count);

		for(show_counter=0;show_counter<255;show_counter++){
			if(route[show_counter] == 0xff) break;
			printf("route[%d] = %#x\n",show_counter,route[show_counter]);
		}
		for(show_counter=0;show_counter<1024;show_counter++){
				if(pass[show_counter] == 100) break;
				printf("pass[%d] = %d\n",show_counter,pass[show_counter]);
		}
		printf("\n");
*/
		switch(route[route_count]){
			case STRAIGHT:

				if((route[route_count+1] == TURN_RIGHT) && (route[route_count+2] == STRAIGHT)){

					if((pass_count > 0) && (pass[pass_count-1] > 0))	pass[pass_count-1] -= 1;
					else if(pass[pass_count-1] == -1 || pass[pass_count-1] == -2){
						pass[pass_count] = 1;
						pass_count++;
					}

					pass[pass_count] = BIG_R90;
					pass_count++;
					route_count += 2;
					big_turn_flag = 1;
					normal_turn_flag = 0;
					break;
				}else if((route[route_count+1] == TURN_RIGHT) && (route[route_count+2] == TURN_RIGHT) && (route[route_count+3] == STRAIGHT)){

					if((pass_count > 0) && (pass[pass_count-1] > 0))	pass[pass_count-1] -= 1;
					else if(pass[pass_count-1] == -1 || pass[pass_count-1] == -2){
						pass[pass_count] = 1;
						pass_count++;
					}

					pass[pass_count] = BIG_R180;

					pass_count++;
					route_count += 3;
					big_turn_flag = 1;
					normal_turn_flag = 0;
					break;
				}else if((route[route_count+1] == TURN_LEFT) && (route[route_count+2] == STRAIGHT)){
					if((pass_count > 0) && (pass[pass_count-1] > 0))	pass[pass_count-1] -= 1;
					else if(pass[pass_count-1] == -1 || pass[pass_count-1] == -2){
						pass[pass_count] = 1;
						pass_count++;
					}
					pass[pass_count] = BIG_L90;
					pass_count++;
					route_count += 2;
					big_turn_flag = 1;
					normal_turn_flag = 0;
					break;
				}else if((route[route_count+1] == TURN_LEFT) && (route[route_count+2] == TURN_LEFT) && (route[route_count+3] == STRAIGHT)){
					if((pass_count > 0) && (pass[pass_count-1] > 0))	pass[pass_count-1] -= 1;
					else if(pass[pass_count-1] == -1 || pass[pass_count-1] == -2){
						pass[pass_count] = 1;
						pass_count++;
					}
					pass[pass_count] = BIG_L180;
					pass_count++;
					route_count += 3;
					big_turn_flag = 1;
					normal_turn_flag = 0;
					break;
				}else if(normal_turn_flag == 1){
					route_count++;
					normal_turn_flag = 0;
					break;
				}

				j = route_count;
				while(route[j] == STRAIGHT){
					if(big_turn_flag == 1){
						half_straight_count  = 1;
						big_turn_flag = 0;
					}else{
						half_straight_count += 2;
					}
					j++;
				}
				pass[pass_count] = half_straight_count;
				pass_count++;

				half_straight_count = 0;
				route_count = j - 1;
				normal_turn_flag = 1;

				if(route[route_count+1] == 0xff)	return_flag = 1;

				break;

			case TURN_RIGHT:
				normal_turn_flag = 0;
				half_straight_count = 0;
				pass[pass_count] = R90;
				pass_count++;
				route_count++;
				break;

			case TURN_LEFT:
				normal_turn_flag = 0;
				half_straight_count = 0;
				pass[pass_count] = L90;
				pass_count++;
				route_count++;
				break;

			default:
				pass_end_count = pass_count;
				printf("pass_end_count : %d\n",pass_end_count);
				return;
				break;
		}
		if(return_flag == 1)	break;
	}
	pass_end_count = pass_count;
	printf("pass_end_count : %d\n",pass_end_count);
	return;
}

void ShowPass(void){
	uint16_t i;

	for(i=0;i<256;i++){
		if(route[i] == 0xff) break;
		printf("route[%d] : %x\n",i,route[i]);
	}
	printf("\n");

	for(i=0;i<1024;i++){
		if(pass[i] == 100) break;
		printf("pass[%d] : %d\n",i,pass[i]);
	}
	printf("End Print\n");
}

