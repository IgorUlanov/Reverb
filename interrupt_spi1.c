/*
 * interrupt_spi1.c
 *
 *  Created on: 20.09.2012
 *      Author: igor
 */

#include <stdint.h>
#include <stm32f10x.h>
#include "common.h"

extern uint8_t tx_spi[7]; // буфер для передачи управления на cs4221
extern uint_fast8_t step_spi; // счетчик переданных байт по spi1
extern uint_fast8_t count_spi; // количество передаваемых байт

void SPI1_IRQHandler(void)
{
  if (SPI_I2S_GetITStatus(SPI1,SPI_I2S_IT_TXE)==SET)
		{
			if (step_spi<count_spi) SPI1->DR=tx_spi[step_spi++];
			else SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_TXE,DISABLE);
		}
}
