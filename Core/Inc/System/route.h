/*
 * route.h
 *
 *  Created on: 2022/03/06
 *      Author: sssho
 */
#include<stdint.h>

#ifndef INC_SYSTEM_ROUTE_H_
#define INC_SYSTEM_ROUTE_H_

//行動の定義
#define R90 -1
#define L90 -2
#define BIG_R90 -3
#define BIG_L90 -4
#define BIG_R180 -5
#define BIG_L180 -6

extern int8_t pass[1024];
extern uint16_t pass_end_count;

void MakePass(void);
void ReadPass(void);
void ShowPass(void);

#endif /* INC_SYSTEM_ROUTE_H_ */
