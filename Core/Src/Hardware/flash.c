/*
 * flash.c
 *
 *  Created on: Feb 27, 2022
 *      Author: sssho
 */
#include"Hardware/flash.h"

#include <string.h>

volatile uint32_t start_address = 0x80E0000;
volatile uint32_t end_adress = 0x80FFFFF;

void FLASH_WaitBusy(void)
{
	while( ( (FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY) == 1 );
}


void FLASH_Erease(void)
{
	FLASH->CR |= FLASH_CR_SER;
	FLASH->CR |= FLASH_SECTOR11 & FLASH_CR_SNB_Msk;
	FLASH->CR |= FLASH_CR_STRT;
	FLASH_WaitBusy();
}

void FLASH_WriteByte(uint32_t address, uint8_t data)
{

	FLASH->CR &= ~(FLASH_CR_PSIZE);

	FLASH->CR |= FLASH_TYPEPROGRAM_BYTE;
	FLASH->CR |= FLASH_CR_PG;

	*(volatile  uint8_t*)address = data;

	FLASH_WaitBusy();

	FLASH->CR &= ~(FLASH_CR_PG);
}

void FLASH_WriteData(uint32_t address, uint8_t* data, uint32_t size)
{
	FLASH_Unlock();

	FLASH_Erease();

	do {
		FLASH_WriteByte(address, *data);
	}while(++address, ++data, --size);

	FLASH_Lock();
}

void FLASH_ReadData(uint32_t address, uint8_t* data, uint32_t size)
{
  memcpy(data, (uint8_t*)address, size);
}
