/*
 * flags.h
 *
 *  Created on: 2022/03/06
 *      Author: sssho
 */
#include <stdint.h>

#ifndef INC_SYSTEM_FLAGS_H_
#define INC_SYSTEM_FLAGS_H_

typedef union{
	uint16_t FLAGS;
	struct mouse_flags{
		uint8_t SET :1;			//尻当てフラグ　(B0)
		uint8_t SCND :1;		//最短フラグ(B1)
		uint8_t FIRST :1;		//片道探索走行フラグ(B2)
		uint8_t CTRL :1;			//壁制御フラグ　(B3)
		uint8_t ACCEL :1;		//並進加速フラグ(B4)
		uint8_t DECEL :1;		//並進減速フラグ(B5)
		uint8_t WACCEL :1;	//回転加速フラグ(B6)
		uint8_t WDECEL :1;	//回転減速フラグ(B7)
		uint8_t VCTRL :1;		//並進制御フラグ(B8)
		uint8_t WCTRL :1;		//回転制御フラグ(B9)
		uint8_t SAFETY :1;		//安全解析フラグ(B10)
		uint8_t ACTRL :1;		//角度制御フラグ(B11)
		uint8_t FRONT :1;		//前制御フラグ　(B12)
		uint8_t RESV13 :1;		//予備フラグ　　(B13)
		uint8_t RESV14 :1;		//予備フラグ　　(B14)
		uint8_t RESV15 :1;		//予備フラグ　　(B15)
	}FLAG;
}Flags_Typedef;

void setControlFlags(uint8_t vel, uint8_t omega, uint8_t angle, uint8_t wall);
void setAccelFlags(uint8_t accel, uint8_t decel, uint8_t omega_accel, uint8_t omega_decel);

extern Flags_Typedef MF;

#endif /* INC_SYSTEM_FLAGS_H_ */
