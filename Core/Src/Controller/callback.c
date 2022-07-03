/*
 * callback.c
 *
 *  Created on: 2021/04/21
 *      Author: sssho
 */
#include "Controller/mode.h"
#include "Controller/state.h"
#include "Controller/callback.h"

#include "System/sensing.h"
#include "System/log.h"
#include "System/flags.h"

#include"Hardware/interface_LED.h"

extern uint16_t tim_counter;

void callbackTIM(void)
{
	if(MF.FLAG.CALLBACK){	//	割込み処理フラグが無かったら計算はしない
		updateSensors();
		updateStatus();
		if(MF.FLAG.SAFETY)		judgeFailSafe();
		tim_counter = storeLog(tim_counter);
	}
}
