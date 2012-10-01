/*
 * eeprom.c
 *
 *  Created on: 28.09.2012
 *      Author: igor
 */
#include "eeprom.h"
#include "common.h"
#include <stm32f10x_flash.h>

const SETTING eeprom_setting[EFFECTS] __attribute__ ((section (".setting")))=
		{
				{30,30,30},
				{30,30,30},
				{30,30,30},
				{30,30,30},
				{30,30,30},
				{30,30,30},
				{30,30,30}
		};

uint8_t now;

void Search(void)
{
	const SETTING* adr=&eeprom_setting[0];
	uint_fast16_t count=2048/(sizeof(SETTING)*EFFECTS);
	uint_fast16_t step=0;
	for (uint_fast16_t i=1;i<count;i++)
		{
			step=i;
			if ((adr+i)->control_delay==0xFFFF) break;
		}
	now=step-1;
}

void Load(SETTING* adr)
{
	Search();
	const SETTING* flash=&eeprom_setting[0];
	*adr=*(flash+now);
}

void Save(SETTING* addr)
{
	Search();
	const SETTING* flash=&eeprom_setting[0];
	uint16_t* source_addr=(uint16_t*)addr;
	if (now>=((2048/(sizeof(SETTING)*EFFECTS))-1))
		{
			FLASH_ErasePage((uint32_t)flash);
		}
	else flash+=now+1;
	const uint16_t* dest_addr=(const uint16_t*)flash;
	FLASH_Unlock();
	for (uint_fast16_t i=0;i<(sizeof(SETTING)*EFFECTS)/2;i++)
		{
			FLASH_ProgramHalfWord((uint32_t)dest_addr,*source_addr);
			dest_addr++;
			source_addr++;
		}
	FLASH_Lock();
}
