/*
 * log.c
 *
 *  Created on: 2022/03/05
 *      Author: sssho
 */
#include"Controller/state.h"
#include"System/sensing.h"


#include"System/log.h"

LOG_Typedef log [MAX_LOG];

//+++++++++++++++++++++++++++++++++++++++++++++++
// storeLog
//	@brief   ログデータを配列に格納、書込みはしていない
// @param 記録する配列要素番号、領域外参照に注意
// @return カウントがオーバーフローしてたら0を返す、正常ならインクリメント
//+++++++++++++++++++++++++++++++++++++++++++++++
uint16_t storeLog(uint16_t number) {
	uint16_t count;
	if(number > MAX_LOG - 1)	return number;
	else									count = number;

	log[count].target_velocity = target.velocity;
	log[count].target_omega = target.omega;
	log[count].target_accel = target.accel;
	log[count].real_velocity = mouse.velocity;
	log[count].real_omega = mouse.omega;
	log[count].real_accel = mouse.accel;
	log[count].mileage = mouse.mileage;
	log[count].angle = mouse.angle;
	log[count].left_sensor = sensor.wall_val[L];
	log[count].right_sensor = sensor.wall_val[R];

	return count + 1;
}
