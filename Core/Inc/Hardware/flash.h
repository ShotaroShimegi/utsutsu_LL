/*
 * flash.h
 *
 *  Created on: Feb 27, 2022
 *      Author: sssho
 */

#ifndef INC_HARDWARE_FLASH_H_
#define INC_HARDWARE_FLASH_H_

#include <stdint.h>

#include "main.h"

#define FLASH_KEY1               0x45670123U
#define FLASH_KEY2               0xCDEF89ABU

#define FLASH_TYPEPROGRAM_BYTE  0x00000000U
#define FLASH_SECTOR11					0x58

__STATIC_INLINE void FLASH_Lock(void)
{
	FLASH->CR |= FLASH_CR_LOCK;

}

__STATIC_INLINE void FLASH_Unlock(void)
{
	FLASH->KEYR =  FLASH_KEY1;
	FLASH->KEYR =  FLASH_KEY2;
}

void FLASH_Erase(void);
void FLASH_WriteData(uint32_t address, uint8_t data);
uint8_t FLASH_ReadData(uint32_t address);

extern volatile uint32_t start_address;
extern volatile uint32_t end_adress;

#endif /* INC_HARDWARE_FLASH_H_ */
