/*
 * battery.c
 *
 *  Created on: 2022/02/28
 *      Author: sssho
 */
#include"Hardware/battery.h"

#include"main.h"

/**
* @brief　	バッテリーICのチェック
* @return	バッテリーICの返答、0が電圧降下、1がプルアップ正常
*/
uint8_t readBatteryCheck(void)
{
	return (Battery_GPIO_Port->IDR & Battery_Pin) == Battery_Pin;
}
