/*
 * eeprom.h
 *
 *  Created on: 28.09.2012
 *      Author: igor
 */

#ifndef EEPROM_H_
#define EEPROM_H_
#include <stdint.h>

typedef struct
{
	uint16_t control_delay; // высчитанное значение регулировки задержки
	uint16_t control_decay; // высчитанное значение регулировки усиления
	uint16_t control_defuse; // высчитанное значение регулировки примешивания
}SETTING;

#endif /* EEPROM_H_ */
