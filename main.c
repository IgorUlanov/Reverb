/*
 * main.cpp
 *
 *  Created on: Sep 28, 2011
 *      Author: igor
 */

#include "main.h"
#include "init.h"
#include "subs.h"
#include "common.h"
#include "reverb.h"

uint16_t control_delay_last; // предыдущее значение регулировки задержки
uint16_t control_delay; // высчитанное значение регулировки задержки
uint16_t control_decay_last; // предыдущее значение регулировки усиления
uint16_t control_decay; // высчитанное значение регулировки усиления
uint16_t control_defuse_last; // предыдущее значение регулировки примешивания
uint16_t control_defuse; // высчитанное значение регулировки примешивания
uint16_t control_mode_last; // предыдущее значение положения регулятора режима
uint16_t control_mode; // высчитанное значение положения регулятора режима
uint8_t tx_spi[7]; // буфер для передачи управления на cs4221
uint_fast8_t step_spi; // счетчик переданных байт по spi1
uint_fast8_t count_spi; // количество передаваемых байт

volatile uint8_t control_ready; // флаг готовности регулировочных значений
extern uint16_t channel_0[CHANNEL_COUNT]; // массив с результатами измерений канал 10 (value*4)
extern uint16_t channel_1[CHANNEL_COUNT]; // массив с результатами измерений канал 11 (value*4)
extern uint16_t channel_2[CHANNEL_COUNT]; // массив с результатами измерений канал 12 (value*4)
extern uint16_t channel_3[CHANNEL_COUNT]; // массив с результатами измерений канал 13 (value*4)
extern ALGORITM ar;

int main (void)
{
Begin_config();
Audio_config();
Control_config();
Led_config();
Final_config();
tx_spi[0]=0x20; // adr
tx_spi[1]=0x81; // num registr
tx_spi[2]=0b01100100;
tx_spi[3]=0;
tx_spi[4]=0;
tx_spi[5]=32;
tx_spi[6]=0b01000000;
count_spi=7;
step_spi=0;
SPI_I2S_SendData(SPI1,tx_spi[step_spi++]);
ADC_SoftwareStartConvCmd(ADC1, ENABLE);
RESET_ADC=1; // сброс на cs4221
LED_FLASH=0;

while(1)
  {
    if (control_ready!=0)
      {
        // готовы измерения
        control_ready=0;
        // подсчет значения задержки
        uint32_t add=0;
        for (uint8_t i=0;i<CHANNEL_COUNT;i++) add+=channel_3[i];
        add=add/(CHANNEL_COUNT*4);
        control_delay=1+calc_hysteresis(add,56,&control_delay_last); // на выходе 0-36
         // значение затухания
        add=0;
        for (uint8_t i=0;i<CHANNEL_COUNT;i++) add+=channel_2[i];
        add=add/(CHANNEL_COUNT*4);
        control_decay=calc_hysteresis(add,56,&control_decay_last); // на выходе 0-36
        // значение примешивания
       add=0;
       for (uint8_t i=0;i<CHANNEL_COUNT;i++) add+=channel_0[i];
       add=add/(CHANNEL_COUNT*4);
       control_defuse=calc_hysteresis(add,56,&control_defuse_last); // на выходе 0-36
        // режим
        add=0;
        for (uint8_t i=0;i<CHANNEL_COUNT;i++) add+=channel_1[i];
        add=4095-add/(CHANNEL_COUNT*4);
        add=calc_hysteresis(add,(4096/EFFECTS),&control_mode_last)+1; // на выходе 1-8
        if (control_mode!=add)
          {
            // если смена режима, то обнуляем переменные.
            control_mode=100;
            for (uint16_t i=0;i<30000;i++) ar.delay_buf[i]=0;
            control_mode=add;
          }

      }
  }
return (0);
}

