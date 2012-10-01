/*
 * interrupt_dma1_channel1.c
 *
 */

#include <stdint.h>
#include <stm32f10x.h>
#include "common.h"

volatile uint16_t control_buf[ADC_CONTROL_COUNT*2]; // массив c результатами измерения управления.
uint16_t channel_0[CHANNEL_COUNT]; // массив с результатами измерений канал 0 (value*4)
uint16_t channel_1[CHANNEL_COUNT]; // массив с результатами измерений канал 1 (value*4)
uint16_t channel_2[CHANNEL_COUNT]; // массив с результатами измерений канал 2 (value*4)
uint16_t channel_3[CHANNEL_COUNT]; // массив с результатами измерений канал 3 (value*4)
extern volatile uint8_t control_ready; // флаг готовности регулировочных значений

void DMA1_Channel1_IRQHandler(void)
{
  static uint_fast8_t channel_step;
  // Последовательность опроса - 1x5,2x5,3x5
  if (DMA1->ISR & DMA_ISR_HTIF1)
    {
      // первая половина массива заполнена
      channel_0[channel_step]=(control_buf[0]+control_buf[1]+control_buf[2]+control_buf[3]); // значение регулировки усиления после ацп
      channel_1[channel_step]=(control_buf[4]+control_buf[5]+control_buf[6]+control_buf[7]); // значение регулировки усиления после ацп
      channel_2[channel_step]=(control_buf[8]+control_buf[9]+control_buf[10]+control_buf[11]); // значение положения регулятора режима после ацп
      channel_3[channel_step]=(control_buf[12]+control_buf[13]+control_buf[14]+control_buf[15]); // резерв
    }
  else if ((DMA1->ISR & DMA_ISR_TCIF1)!=0)
    {
      // вторая половина массива заполнена
      channel_0[channel_step]=(control_buf[16]+control_buf[17]+control_buf[18]+control_buf[19]); // значение регулировки усиления после ацп
      channel_1[channel_step]=(control_buf[20]+control_buf[21]+control_buf[22]+control_buf[23]); // значение регулировки усиления после ацп
      channel_2[channel_step]=(control_buf[24]+control_buf[25]+control_buf[26]+control_buf[27]); // значение положения регулятора режима после ацп
      channel_3[channel_step]=(control_buf[28]+control_buf[29]+control_buf[30]+control_buf[31]); // резерв
    }
  channel_step++;
  if (channel_step>(CHANNEL_COUNT-1))
    {
      channel_step=0;
      control_ready=0xFF; // обработка значений регулировок.
    }
  DMA1->IFCR |= DMA_IFCR_CGIF1;
}
