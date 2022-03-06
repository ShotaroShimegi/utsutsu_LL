/*
 * flags.c
 *
 *  Created on: 2022/03/06
 *      Author: sssho
 */
#include "System/flags.h"

Flags_Typedef MF;

/**
* @brief 制御則の選択
* params 各制御フラグ
* 1 . 速度
* 2 . 角速度
* 3 . 角度
* 4 . 壁制御
*/
void setControlFlags(uint8_t vel, uint8_t omega, uint8_t angle, uint8_t wall)
{
	MF.FLAG.VCTRL = vel & 0x01;
	MF.FLAG.WCTRL = omega & 0x01;
	MF.FLAG.ACTRL = angle & 0x01;
	MF.FLAG.CTRL = wall & 0x01;
}

/**
* @brief 加減速の設定
* params 各制御フラグ
* 1 . 並進加速
* 2 . 並進減速
* 3 . 回転加速
* 4 . 回転減速
*/
void setAccelFlags(uint8_t accel, uint8_t decel, uint8_t omega_accel, uint8_t omega_decel)
{
	MF.FLAG.ACCEL = accel & 0x01;
	MF.FLAG.DECEL = decel & 0x01;
	MF.FLAG.WACCEL = omega_accel & 0x01;
	MF.FLAG.WDECEL = omega_decel & 0x01;
}
