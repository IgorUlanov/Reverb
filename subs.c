/*
 * subs.cpp
 *
 *  Created on: 10.11.2011
 *      Author: igor
 */
#include "subs.h"


uint16_t calc_hysteresis(uint16_t x, uint16_t div, uint16_t *adr_last)
{
  /* функция устраняет дрожание границы благодаря использованию гистерезиса,
   * который равен половине остатка округляемого значения.
   * входные параметры:
   * х - входное значение 0..4095
   * adr_last - адрес предыдущего значения
   * div - значение на которое округляется
   *
   */
	uint16_t low=0;
	uint16_t last=*adr_last;
  if (last!=0) low=last-div/2;
  uint16_t high=4095;
  if (last<(4096-div*2)) high=last+div*2+div/2;
  if (x<low) {if (last!=0) last-=div*2;} else if (x>high) if (last<(4096-div*2)) last+=div*2;
  *adr_last=last;
  return last/(div*2);
}

