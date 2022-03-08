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

void FLASH_WriteByte(uint32_t address, uint8_t data)
{

	FLASH->CR &= ~(FLASH_CR_PSIZE);

	FLASH->CR |= FLASH_TYPEPROGRAM_BYTE;
	FLASH->CR |= FLASH_CR_PG;

	*(volatile  uint8_t*)address = data;

	FLASH_WaitBusy();

	FLASH->CR &= ~(FLASH_CR_PG);
}

void FLASH_WriteData(uint32_t address, uint8_t data)
{
	FLASH_Unlock();

	FLASH_WriteByte(address+start_address, data);

	FLASH_Lock();
}

uint8_t FLASH_ReadData(uint32_t address)
{
  return *(uint8_t*) (address + start_address);
}

void FLASH_Erase(void)
{
	FLASH_Unlock();

	FLASH->CR |= FLASH_CR_SER;
	FLASH->CR |= FLASH_SECTOR11 & FLASH_CR_SNB_Msk;
	FLASH->CR |= FLASH_CR_STRT;
	FLASH_WaitBusy();

	FLASH_Lock();
}
