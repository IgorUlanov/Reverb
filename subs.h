/*
 * subs.h
 *
 *  Created on: 10.11.2011
 *      Author: igor
 */

#ifndef SUBS_H_
#define SUBS_H_
#include <stdint.h>

uint16_t calc_hysteresis(uint16_t x, uint16_t div, uint16_t *adr_last);
  /* функция устраняет дрожание границы благодаря использованию гистерезиса,
   * который равен половине остатка округляемого значения.
   * входные параметры:
   * х - входное значение 0..4095
   * adr_last - адрес предыдущего значения
   * div - значение на которое округляется
   */

#endif /* SUBS_H_ */
