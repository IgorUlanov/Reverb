#include <stdint.h>
#include <stm32f10x.h>
#include "common.h"

uint16_t delay_in_index; // индекс по которому записывается новое значение в массив delay_buf
int16_t sound; // звук
volatile uint32_t overload; // перегрузка. накапливается квадрат 3-х старших битов значений звука
uint16_t analog_adc[2]; // массив со значениями АЦП

extern uint16_t control_mode; // высчитанное значение положения регулятора режима
extern uint_fast8_t overload_get; // указание, что значение считано и его необходимо обнулить.

extern void Delay_long(void);	// задержка от 50 мс до максимума, без повтора
extern void Delay_long_defuse(void);	// задержка от 50 мс до максимума, c примешиванием повторов
extern void Delay_short_defuse(void); // задержка от 0.26мс до 51.2мс, c примешиванием повторов
extern void NM3011(void); // эмуляция ревербератора на основе микросхемы mn3011
extern void Dattorro(void); // ревербератор по алгоритму Dattorro
extern void Gardner_small_room(void); // ревербератор по алгоритму Gardner Small Room Reverberator
extern void Gardner_medium_room(void); // ревербератор по алгоритму Gardner Medium Room Reverberator
extern void Gardner_large_room(void); // ревербератор по алгоритму Gardner Large Room Reverberator

void DMA2_Channel1_IRQHandler(void)
{
  // обработка звукового сигнала
	LED_FLASH=1;
	int16_t n=0;
	uint16_t add=0;

	if ((DMA2->ISR & DMA_ISR_TCIF1)!=0)
		{
			// Считаны оба канала
			sound=analog_adc[0];
			if (sound<0) n=65535-(uint16_t)sound; else n=sound;
			if (n>18259) {add=1;	if (n>20487) {add=4;	if (n>22987) {add=16; if (n>25792) {add=256; if (n>28939) add=65535;}}}};
			if (overload_get!=0)
				{
					overload_get=0;
					overload=0;
				}
			overload+=add;
		 switch (control_mode)
		  {
		  case 1:	Delay_long(); break;
		  case 2: Delay_long_defuse(); break;
		  case 3: Delay_short_defuse(); break;
		  case 4: NM3011(); break;
		  case 5: Dattorro(); break;
		  case 6: Gardner_small_room(); break;
		  case 7: Gardner_medium_room(); break;
		  case 8: Gardner_large_room();	break;
		  }
		  delay_in_index++;
		}
	DMA2->IFCR |= DMA_IFCR_CGIF1;
	LED_FLASH=0;
}
