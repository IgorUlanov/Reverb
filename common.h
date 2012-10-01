/*
 * common.h
 *
 *  Created on: 02.08.2012
 *      Author: igor
 */

#ifndef COMMON_H_
#define COMMON_H_
#include <stm32f10x.h>

// PD6 - флеш (не светится - считывание параметров из флеш,
// светится - считывание параметров с регуляторов, мигает - запись параметров во флеш)
// PD4 - режим (светится - эффект включен, мигает - эффект выключен, не светится - нет питания)
// PD6 - отладка
#define LED_FLASH (*((__O unsigned long *) (PERIPH_BB_BASE + ((GPIOD_BASE+0x0C-PERIPH_BASE) * 0x20) + (6*4))))
#define LED_REGIM (*((__O unsigned long *) (PERIPH_BB_BASE + ((GPIOE_BASE+0x0C-PERIPH_BASE) * 0x20) + (14*4))))
#define RESET_ADC (*((__O unsigned long *) (PERIPH_BB_BASE + ((GPIOB_BASE+0x0C-PERIPH_BASE) * 0x20) + (7*4))))

#define ADC_CONTROL_COUNT 16
#define CHANNEL_COUNT 8 // величина массивов собирающих результат измерения регуляторов
#define EFFECTS 8 // количество эффектов
#endif /* COMMON_H_ */
