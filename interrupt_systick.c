#include <stdint.h>
#include <stm32f10x.h>
#include "common.h"

//  pa6 - ad0/cs
extern uint8_t tx_spi[7]; // буфер для передачи управления на cs4221
extern uint_fast8_t step_spi; // счетчик переданных байт по spi1
extern uint_fast8_t count_spi; // количество передаваемых байт
extern uint16_t control_decay; // высчитанное значение регулировки усиления
extern uint16_t control_defuse; // высчитанное значение регулировки примешивания
extern volatile uint32_t overload; // перегрузка. накапливается квадрат 3-х старших битов значений звука
extern uint16_t control_mode; // высчитанное значение положения регулятора режима

uint_fast8_t dec;
uint_fast8_t step_mode;
uint_fast8_t mode_led;
uint_fast8_t overload_get; // указание, что значение считано и его необходимо обнулить.

void SysTick_Handler(void)
{

	switch(dec)
	{
	case 4:
		// каждые 10 мс
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);
    tx_spi[0]=0x20; // adr
    tx_spi[1]=0x83; // num registr
		tx_spi[2]=control_defuse;
    if (control_mode!=5) tx_spi[3]=control_decay+6; // выход как минимум приглушен вдвое, так как диф. каскад имеет ус. = 2
    else tx_spi[3]=6; // выход приглушен вдвое, так как диф. каскад имеет ус. = 2
    count_spi=4;
    step_spi=0;
    SPI_I2S_SendData(SPI1,tx_spi[step_spi++]);
    SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_TXE,ENABLE);
		break;
	case 5:
		GPIO_SetBits(GPIOA,GPIO_Pin_6);
		break;
	case 6:
		if (step_mode==36)
			{
				if(mode_led>0) LED_REGIM=0;
				mode_led++;
			}
		if (step_mode>39)
			{
				step_mode=0;
				if (mode_led>control_mode) mode_led=0;
				LED_REGIM=1;
			} else step_mode++;
		break;
	case 7:
		TIM_SetCompare1(TIM1,overload/128);
		TIM_SetCompare2(TIM1,overload/128);
		overload_get=0x5A;
		break;
	case 10:
		dec=0;
		break;
	}
	dec++;
}
