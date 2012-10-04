/*
 * reverb.c
 *
 *  Created on: 27.09.2012
 *      Author: igor
 */
#include <stdint.h>
#include "reverb.h"

extern int16_t sound; // звук
extern uint16_t delay_in_index; // индекс по которому записывается новое значение в массив DELAY_buf
extern const uint16_t adjust[]; // массив с таблицей децибел от значения 65536 в диапазоне 0-99дб c шагом 0.1dB
extern uint16_t control_delay; // высчитанное значение регулировки задержки
extern uint16_t control_decay; // высчитанное значение регулировки усиления
extern uint16_t control_defuse; // высчитанное значение регулировки примешивания
const SETTING setting_eeprom[8] __attribute__ ((section (".setting"))); // значения регулировок во флеш

uint16_t analog_dac[2]; // массив со значениями для ЦАП
int16_t step;
int32_t in,all,x,y;
ALGORITM ar;
/*
// константы для алгоритмa эмуляции микросхемы nm3011
const uint16_t NM_DECAY1=RATIO; // 1
const uint16_t NM_DECAY2=(uint16_t)(0.909*RATIO);
const uint16_t NM_DECAY3=(uint16_t)(0.833*RATIO);
const uint16_t NM_DECAY4=(uint16_t)(0.769*RATIO);
const uint16_t NM_DECAY5=(uint16_t)(0.667*RATIO);
const uint16_t NM_DECAY6=(uint16_t)(0.625*RATIO);
const uint16_t NM_DELAY1=(uint16_t)(19.8/SAMPLING);
const uint16_t NM_DELAY2=(uint16_t)(33.1/SAMPLING);
const uint16_t NM_DELAY3=(uint16_t)(59.7/SAMPLING);
const uint16_t NM_DELAY4=(uint16_t)(86.3/SAMPLING);
const uint16_t NM_DELAY5=(uint16_t)(139.5/SAMPLING);
const uint16_t NM_DELAY6=(uint16_t)(166.4/SAMPLING);

// константы для алгоритма Датторро
const uint16_t preDELAY_max=(uint16_t)(50.0/SAMPLING); // максимальная задержка 50 мс
const uint16_t INPUT1_COUNT=(uint16_t)(4.71/SAMPLING); // 4.71
const uint16_t INPUT2_COUNT=(uint16_t)(3.6/SAMPLING); // 3.6
const uint16_t INPUT3_COUNT=(uint16_t)(12.7/SAMPLING); // 12.7
const uint16_t INPUT4_COUNT=(uint16_t)(9.3/SAMPLING); // 9.3
const uint16_t DECAY1_COUNT=(uint16_t)(22.58/SAMPLING)+16; // 22.58
const uint16_t DECAY2_COUNT=(uint16_t)(60.48/SAMPLING); // 60.48
const uint16_t DECAY3_COUNT=(uint16_t)(30.5/SAMPLING)+16; // 30.5
const uint16_t DECAY4_COUNT=(uint16_t)(89.24/SAMPLING); // 89.24
const uint16_t DELAY1_COUNT=(uint16_t)(149.6/SAMPLING); // 149.6
const uint16_t DELAY2_COUNT=(uint16_t)(125.0/SAMPLING); // 125
const uint16_t DELAY3_COUNT=(uint16_t)(141.7/SAMPLING); // 141.7
const uint16_t DELAY4_COUNT=(uint16_t)(106.28/SAMPLING); // 106.28

// константы для алгоритма Гарднера SR - Small Room
const uint16_t SR_preDELAY_COUNT=(uint16_t)(24.0/SAMPLING); // 24 ms
const uint16_t SR_DOUBLE1_DELAY=(uint16_t)(4.7/SAMPLING); // 4.7 ms
const uint16_t SR_DOUBLE1_DECAY=(uint16_t)(0.15*RATIO); // 0.15
const uint16_t SR_DOUBLE2_DELAY=(uint16_t)(22.0/SAMPLING); // 22 ms
const uint16_t SR_DOUBLE2_DECAY=(uint16_t)(0.25*RATIO); // 0.25
const uint16_t SR_DOUBLE3_DELAY=(uint16_t)(8.3/SAMPLING); //8.3 ms
const uint16_t SR_DOUBLE3_DECAY=(uint16_t)(0.3*RATIO); // 0.3
const uint16_t SR_SINGLE1_DELAY=(uint16_t)(33.0/SAMPLING); // 33 ms
const uint16_t SR_SINGLE1_DECAY=(uint16_t)(0.08*RATIO); // 0.08
const uint16_t SR_SINGLE2_DELAY=(uint16_t)(30.0/SAMPLING); // 30 ms
const uint16_t SR_SINGLE2_DECAY=(uint16_t)(0.3*RATIO); // 0.3

// MR - Medium Room
const uint16_t MR_DOUBLE1_DELAY=(uint16_t)(4.7/SAMPLING); // 4.7 ms
const uint16_t MR_DOUBLE1_DECAY=(uint16_t)(0.25*RATIO); // 0.25
const uint16_t MR_DOUBLE2_DELAY=(uint16_t)(8.3/SAMPLING); //8.3 ms
const uint16_t MR_DOUBLE2_DECAY=(uint16_t)(0.35*RATIO); // 0.35
const uint16_t MR_DOUBLE3_DELAY=(uint16_t)(22.0/SAMPLING); // 22 ms
const uint16_t MR_DOUBLE3_DECAY=(uint16_t)(0.45*RATIO); // 0.45
const uint16_t MR_DELAY_5ms=(uint16_t)(5.0/SAMPLING);
const uint16_t MR_ALLPASS_DELAY=(uint16_t)(30.0/SAMPLING); // 30 ms
const uint16_t MR_ALLPASS_DECAY=(uint16_t)(0.45*RATIO); // 0.45
const uint16_t MR_DELAY_67ms=(uint16_t)(67.0/SAMPLING);
const uint16_t MR_DELAY_15ms=(uint16_t)(15.0/SAMPLING);
const uint16_t MR_SINGLE1_DELAY=(uint16_t)(29.2/SAMPLING); // 29.2 ms
const uint16_t MR_SINGLE1_DECAY=(uint16_t)(0.25*RATIO); // 0.25
const uint16_t MR_SINGLE2_DELAY=(uint16_t)(9.8/SAMPLING); // 9.8 ms
const uint16_t MR_SINGLE2_DECAY=(uint16_t)(0.35*RATIO); // 0.35
const uint16_t MR_DELAY_108ms=(uint16_t)(108.0/SAMPLING);

// LR - Large Room
const uint16_t LR_ALLPASS1_DELAY=(uint16_t)(8.0/SAMPLING); // 8 ms
const uint16_t LR_ALLPASS1_DECAY=(uint16_t)(0.3*RATIO); // 0.3
const uint16_t LR_ALLPASS2_DELAY=(uint16_t)(12.0/SAMPLING); // 12 ms
const uint16_t LR_ALLPASS2_DECAY=(uint16_t)(0.3*RATIO); // 0.3
const uint16_t LR_DELAY_4ms=(uint16_t)(4.0/SAMPLING);
const uint16_t LR_DELAY_17ms=(uint16_t)(17.0/SAMPLING);
const uint16_t LR_SINGLE1_DELAY=(uint16_t)(25.0/SAMPLING); // 25 ms
const uint16_t LR_SINGLE1_DECAY=(uint16_t)(0.5*RATIO); // 0.5
const uint16_t LR_SINGLE2_DELAY=(uint16_t)(62.0/SAMPLING); // 62 ms
const uint16_t LR_SINGLE2_DECAY=(uint16_t)(0.25*RATIO); // 0.25
const uint16_t LR_DELAY_31ms=(uint16_t)(31.0/SAMPLING);
const uint16_t LR_DELAY_3ms=(uint16_t)(3.0/SAMPLING);
const uint16_t LR_DOUBLE1_DELAY=(uint16_t)(14.0/SAMPLING); // 14 ms
const uint16_t LR_DOUBLE1_DECAY=(uint16_t)(0.5*RATIO); // 0.5
const uint16_t LR_DOUBLE2_DELAY=(uint16_t)(76/SAMPLING); //76 ms
const uint16_t LR_DOUBLE2_DECAY=(uint16_t)(0.25*RATIO); // 0.25
const uint16_t LR_DOUBLE3_DELAY=(uint16_t)(30/SAMPLING); // 30 ms
const uint16_t LR_DOUBLE3_DECAY=(uint16_t)(0.25*RATIO); // 0.25
*/
void Delay_long(void)
{
	// задержка от 47 мс до максимума, без повтора
  if (delay_in_index > DELAY_COUNT_MAX-1) delay_in_index=0;
  ar.delay_buf[delay_in_index]=sound;
  step=(int16_t)delay_in_index-((uint32_t)adjust[control_delay*6]*30000)/65536;
  if (step<0) step+=(int16_t)DELAY_COUNT_MAX-1;
  in=ar.delay_buf[(uint16_t)step];
  analog_dac[0]=0; // сигнал для примешивания
  analog_dac[1]=(int16_t)in; // задержанный аудиосигнал
}

void Delay_long_defuse(void)
{
	// задержка от 47 мс до максимума, c примешиванием повторов
  if (delay_in_index > DELAY_COUNT_MAX-1) delay_in_index=0;
  ar.delay_buf[delay_in_index]=sound;
  step=(int16_t)delay_in_index-((uint32_t)adjust[control_delay*6]*30000)/65536;
  if (step<0) step+=(int16_t)DELAY_COUNT_MAX-1;
  in=ar.delay_buf[(uint16_t)step];
  analog_dac[0]=(int16_t)in; // сигнал для примешивания
  analog_dac[1]=(int16_t)in; // аудио сигнал готовый для вывода на ЦАП
}

void Delay_short_defuse(void)
{
  // задержка от 0.41мс до 60мс, c примешиванием повторов
  if (delay_in_index > DELAY_COUNT_MAX-1) delay_in_index=0;
  ar.delay_buf[delay_in_index]=sound;
  step=(int16_t)delay_in_index-((uint32_t)adjust[control_delay*12]*2100)/65536;
  if (step<0) step+=(int16_t)DELAY_COUNT_MAX-1;
  in=ar.delay_buf[(uint16_t)step];
  analog_dac[0]=(int16_t)in; // сигнал для примешивания
  analog_dac[1]=(int16_t)in; // аудио сигнал готовый для вывода на ЦАП
}

void NM3011(void)
{
	// эмуляция ревербератора на основе микросхемы mn3011
if (delay_in_index > DELAY_COUNT_MAX-1) delay_in_index=0;
ar.delay_buf[delay_in_index]=sound;
step=(int16_t)delay_in_index-((int32_t)NM_DELAY1*adjust[control_delay*6])/65536;
if (step<0) step+=(int16_t)DELAY_COUNT_MAX;
in=ar.delay_buf[(uint16_t)step--];
if (step<0) step+=(int16_t)DELAY_COUNT_MAX;
all=((in+ar.delay_buf[(uint16_t)step])*NM_DECAY1)/RATIO;

step=(int16_t)delay_in_index-((int32_t)NM_DELAY2*adjust[control_delay*6])/65536;
if (step<0) step+=(int16_t)DELAY_COUNT_MAX;
in=ar.delay_buf[(uint16_t)step--];
if (step<0) step+=(int16_t)DELAY_COUNT_MAX;
all+=((in+ar.delay_buf[(uint16_t)step])*NM_DECAY2)/RATIO;

step=(int16_t)delay_in_index-((int32_t)NM_DELAY3*adjust[control_delay*6])/65536;
if (step<0) step+=(int16_t)DELAY_COUNT_MAX;
in=ar.delay_buf[(uint16_t)step--];
if (step<0) step+=(int16_t)DELAY_COUNT_MAX;
all+=((in+ar.delay_buf[(uint16_t)step])*NM_DECAY3)/RATIO;

step=(int16_t)delay_in_index-((int32_t)NM_DELAY4*adjust[control_delay*6])/65536;
if (step<0) step+=(int16_t)DELAY_COUNT_MAX;
in=ar.delay_buf[(uint16_t)step--];
if (step<0) step+=(int16_t)DELAY_COUNT_MAX;
all+=((in+ar.delay_buf[(uint16_t)step])*NM_DECAY4)/RATIO;

step=(int16_t)delay_in_index-((int32_t)NM_DELAY5*adjust[control_delay*6])/65536;
if (step<0) step+=(int16_t)DELAY_COUNT_MAX;
in=ar.delay_buf[(uint16_t)step--];
if (step<0) step+=(int16_t)DELAY_COUNT_MAX;
all+=((in+ar.delay_buf[(uint16_t)step])*NM_DECAY5)/RATIO;

step=(int16_t)delay_in_index-((int32_t)NM_DELAY6*adjust[control_delay*6])/65536;
if (step<0) step+=(int16_t)DELAY_COUNT_MAX;
in=ar.delay_buf[(uint16_t)step--];
if (step<0) step+=(int16_t)DELAY_COUNT_MAX;
all+=((in+ar.delay_buf[(uint16_t)step])*NM_DECAY6)/RATIO;
y=all/5;
if (y>32700) y=32700; else if (y<-32700) y=-32700;
analog_dac[0]=(int16_t)y/2; // сигнал для примешивания
analog_dac[1]=(int16_t)y; // аудио сигнал готовый для вывода на ЦАП
}

void Dattorro(void)
{
	// ревербератор по алгоритму Dattorro
  if (ar.d.input1_step>INPUT1_COUNT-1) ar.d.input1_step=0;
  if (ar.d.input2_step>INPUT2_COUNT-1) ar.d.input2_step=0;
  if (ar.d.input3_step>INPUT3_COUNT-1) ar.d.input3_step=0;
  if (ar.d.input4_step>INPUT4_COUNT-1) ar.d.input4_step=0;
  if (ar.d.decay1_step>DECAY1_COUNT-1) ar.d.decay1_step=0;
  if (ar.d.delay1_step>DELAY1_COUNT-1) ar.d.delay1_step=0;
  if (ar.d.decay2_step>DECAY2_COUNT-1) ar.d.decay2_step=0;
  if (ar.d.delay2_step>DELAY2_COUNT-1) ar.d.delay2_step=0;
  if (ar.d.decay3_step>DECAY3_COUNT-1) ar.d.decay3_step=0;
  if (ar.d.delay3_step>DELAY3_COUNT-1) ar.d.delay3_step=0;
  if (ar.d.decay4_step>DECAY4_COUNT-1) ar.d.decay4_step=0;
  if (ar.d.delay4_step>DELAY4_COUNT-1) ar.d.delay4_step=0;
  if (ar.d.predelay_step>PREDELAY_MAX-1) ar.d.predelay_step=0;
  step=(int16_t)ar.d.predelay_step-((uint32_t)adjust[control_delay*10]*(uint16_t)PREDELAY_MAX)/65536;
  if (step<0) step+=(uint16_t)PREDELAY_MAX;
  x=ar.d.predelay_buf[step];
  ar.d.predelay_buf[ar.d.predelay_step++]=sound;

  // здесь фильтр
  // bandwidth
  y=adjust[(control_decay*8)+1];
  x=(ar.d.bandwidth*(65535-y))/65536+(x*y)/65536;
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
  ar.d.bandwidth=(int16_t)x;
  // первый рассеиватель
  in=ar.d.input1_buf[ar.d.input1_step];
  y=in+(x*192)/256;
  x=x-(y*192)/256;
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
  ar.d.input1_buf[ar.d.input1_step++]=(int16_t)x;
  // второй рассеиватель
  in=ar.d.input2_buf[ar.d.input2_step];
  x=in+(y*192)/256;
  y=y-(x*192)/256;
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
  ar.d.input2_buf[ar.d.input2_step++]=(int16_t)y;
  // третий рассеиватель
  in=ar.d.input3_buf[ar.d.input3_step];
  y=in+(x*160)/256;
  x=x-(y*160)/256;
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
  ar.d.input3_buf[ar.d.input3_step++]=(int16_t)x;
  // четвертый рассеиватель
  in=ar.d.input4_buf[ar.d.input4_step];
  x=in+(y*160)/256;
  y=y-(x*160)/256;
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
  ar.d.input4_buf[ar.d.input4_step++]=(int16_t)y;
  all=x;

  // левая часть алгоритма
  x=x+(ar.d.right*128)/256;
  // первый рассеиватель DECAY
  in=ar.d.decay1_buf[ar.d.decay1_step];
  y=in-(x*179)/256;
  x=x+(y*179)/256;
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
  ar.d.decay1_buf[ar.d.decay1_step++]=(int16_t)x;
  // задержка на 4453
  in=ar.d.delay1_buf[ar.d.delay1_step];
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
  ar.d.delay1_buf[ar.d.delay1_step++]=(int16_t)y;
  // damping
  x=adjust[(control_defuse*8)+1];
  y=(ar.d.damping_left*(65535-x))/65536+(in*x)/65536;
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
  ar.d.damping_left=(int16_t)y;
  // второй рассеиватель DECAY
  in=ar.d.decay2_buf[ar.d.decay2_step];
  x=in+y/2;
  y=y-x/2;
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
  ar.d.decay2_buf[ar.d.decay2_step++]=(int16_t)y;
  // задержка на 3720
  in=ar.d.delay2_buf[ar.d.delay2_step];
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
  ar.d.delay2_buf[ar.d.delay2_step++]=(int16_t)x;

  // правая часть алгоритма
  x=all+(in*128)/256;
  // третий рассеиватель DECAY
  in=ar.d.decay3_buf[ar.d.decay3_step];
  y=in-(x*179)/256;
  x=x+(y*179)/256;
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
  ar.d.decay3_buf[ar.d.decay3_step++]=(int16_t)x;
  // задержка на 4217
  in=ar.d.delay3_buf[ar.d.delay3_step];
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
  ar.d.delay3_buf[ar.d.delay3_step++]=(int16_t)y;
  // damping
  x=adjust[(control_defuse*8)+1];
  y=(ar.d.damping_right*(65535-x))/65536+(in*x)/65536;
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
  ar.d.damping_right=(int16_t)y;
  // четвертый рассеиватель DECAY
  in=ar.d.decay4_buf[ar.d.decay4_step];
  x=in+y/2;
  y=y-x/2;
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
  ar.d.decay4_buf[ar.d.decay4_step++]=(int16_t)y;
  // задержка на 3163
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
  ar.d.delay4_buf[ar.d.delay4_step++]=(int16_t)x;
  ar.d.right=(int16_t)x;

  // выход для левого канала
  step=ar.d.delay3_step+266;
  if (step>DELAY3_COUNT-1) step=step-DELAY3_COUNT;
  x=ar.d.delay3_buf[step];
  // 2
  step=ar.d.delay3_step+2974;
  if (step>DELAY3_COUNT-1) step=step-DELAY3_COUNT;
  x+=ar.d.delay3_buf[step];
  // 3
  step=ar.d.decay4_step+1913;
  if (step>DECAY4_COUNT-1) step=step-DECAY4_COUNT;
  x-=ar.d.decay4_buf[step];
  // 4
  step=ar.d.delay4_step+1996;
  if (step>DELAY4_COUNT-1) step=step-DELAY4_COUNT;
  x+=ar.d.delay4_buf[step];
  // 5
  step=ar.d.delay1_step+1990;
  if (step>DELAY1_COUNT-1) step=step-DELAY1_COUNT;
  x-=ar.d.delay1_buf[step];
  // 6
  step=ar.d.decay2_step+187;
  if (step>DECAY2_COUNT-1) step=step-DECAY2_COUNT;
  x-=ar.d.decay2_buf[step];
  // 7
  step=ar.d.delay2_step+1066;
  if (step>DELAY2_COUNT-1) step=step-DELAY2_COUNT;
  x-=ar.d.delay2_buf[step];

  // выход для правого канала
  step=ar.d.delay1_step+353;
  if (step>DELAY1_COUNT-1) step=step-DELAY1_COUNT;
  y=ar.d.delay1_buf[step];
  // 2
  step=ar.d.delay1_step+3627;
  if (step>DELAY1_COUNT-1) step=step-DELAY1_COUNT;
  y+=ar.d.delay1_buf[step];
  // 3
  step=ar.d.decay2_step+1228;
  if (step>DECAY2_COUNT-1) step=step-DECAY2_COUNT;
  y-=ar.d.decay2_buf[step];
  // 4
  step=ar.d.delay2_step+2673;
  if (step>DELAY2_COUNT-1) step=step-DELAY2_COUNT;
  y+=ar.d.delay2_buf[step];
  // 5
  step=ar.d.delay3_step+2111;
  if (step>DELAY3_COUNT-1) step=step-DELAY3_COUNT;
  y-=ar.d.delay3_buf[step];
  // 6
  step=ar.d.decay4_step+335;
  if (step>DECAY4_COUNT-1) step=step-DECAY4_COUNT;
  y-=ar.d.decay4_buf[step];
  // 7
  step=ar.d.delay4_step+121;
  if (step>DELAY4_COUNT-1) step=step-DELAY4_COUNT;
  y-=ar.d.delay4_buf[step];
  x=(y+x)/6;
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
  analog_dac[0]=0;
  analog_dac[1]=(int16_t)x; // аудио сигнал готовый для вывода на ЦАП
}

void Mikelson_small_room(void)
{
	// ревербератор по алгоритму Hans Mikelson Small Room Reverberator
	// http://fc.mbs.net/~pfisher/FOV2-0010016C/FOV2-0010016E/FOV2-001001A3/book/chapters/24mikelson/index.html
	if (ar.s.predelay_step>SR_PREDELAY_COUNT-1) ar.s.predelay_step=0;
	if (ar.s.double1_step>SR_DOUBLE1_DELAY-1) ar.s.double1_step=0;
	if (ar.s.double2_step>SR_DOUBLE2_DELAY-1) ar.s.double2_step=0;
	if (ar.s.double3_step>SR_DOUBLE3_DELAY-1) ar.s.double3_step=0;
	if (ar.s.single1_step>SR_SINGLE1_DELAY-1) ar.s.single1_step=0;
	if (ar.s.single2_step>SR_SINGLE2_DELAY-1) ar.s.single2_step=0;

	// predelay
	in=ar.s.predelay_buf[ar.s.predelay_step];
	ar.s.predelay_buf[ar.s.predelay_step++]=sound;

	// double allpass
	// DELAY2
	x=ar.s.double1_buf[ar.s.double1_step];
	y=ar.s.double2_buf[ar.s.double2_step];
	y=y-(x*SR_DOUBLE2_DECAY)/RATIO;
	x=x+(y*SR_DOUBLE2_DECAY)/RATIO;
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
	ar.s.double2_buf[ar.s.double2_step++]=(int16_t)x;
	// DELAY3
	x=ar.s.double3_buf[ar.s.double3_step];
	x=x-(y*SR_DOUBLE3_DECAY)/RATIO;
	y=y+(x*SR_DOUBLE3_DECAY)/RATIO;
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
	ar.s.double3_buf[ar.s.double3_step++]=(int16_t)y;
	// DELAY1
	x=x-(in*SR_DOUBLE1_DECAY)/RATIO;
	y=in+(x*SR_DOUBLE1_DECAY)/RATIO;
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
	ar.s.double1_buf[ar.s.double1_step++]=(int16_t)y;
	in=x;

	// single allpass
	// DELAY2
	x=ar.s.single1_buf[ar.s.single1_step];
	y=ar.s.single2_buf[ar.s.single2_step];
	y=y-(x*SR_SINGLE2_DECAY)/RATIO;
	x=x+(y*SR_SINGLE2_DECAY)/RATIO;
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
	ar.s.single2_buf[ar.s.single2_step++]=(int16_t)x;
	// DELAY1
	y=y-(in*SR_SINGLE1_DECAY)/RATIO;
	x=in+(y*SR_SINGLE1_DECAY)/RATIO;
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
	ar.s.single1_buf[ar.s.single1_step++]=(int16_t)x;
	// выход на ЦАП
	analog_dac[0]=(int16_t)(y/2);
	y=(y+in)/2;
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
	analog_dac[1]=(int16_t)y; // аудио сигнал готовый для вывода на ЦАП
}

void Mikelson_medium_room(void)
{
  // ревербератор по алгоритму Gardner Medium Room Reverberator
  if (ar.m.double1_step>MR_DOUBLE1_DELAY-1) ar.m.double1_step=0;
  if (ar.m.double2_step>MR_DOUBLE2_DELAY-1) ar.m.double2_step=0;
  if (ar.m.double3_step>MR_DOUBLE3_DELAY-1) ar.m.double3_step=0;
  if (ar.m.delay5ms_step>MR_DELAY_5ms-1) ar.m.delay5ms_step=0;
  if (ar.m.allpass_step>MR_ALLPASS_DELAY-1) ar.m.allpass_step=0;
  if (ar.m.delay67ms_step>MR_DELAY_67ms-1) ar.m.delay67ms_step=0;
  if (ar.m.delay15ms_step>MR_DELAY_15ms-1) ar.m.delay15ms_step=0;
  if (ar.m.single1_step>MR_SINGLE1_DELAY-1) ar.m.single1_step=0;
  if (ar.m.single2_step>MR_SINGLE2_DELAY-1) ar.m.single2_step=0;
  if (ar.m.delay108ms_step>MR_DELAY_108ms-1) ar.m.delay108ms_step=0;

  // double allpass
  // DELAY2
  x=ar.m.double1_buf[ar.m.double1_step];
  y=ar.m.double2_buf[ar.m.double2_step];
  y=y-(x*MR_DOUBLE2_DECAY)/RATIO;
  x=x+(y*MR_DOUBLE2_DECAY)/RATIO;
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
  ar.m.double2_buf[ar.m.double2_step++]=(int16_t)x;
  // DELAY3
  x=ar.m.double3_buf[ar.m.double3_step];
  x=x-(y*MR_DOUBLE3_DECAY)/RATIO;
  y=y+(x*MR_DOUBLE3_DECAY)/RATIO;
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
  ar.m.double3_buf[ar.m.double3_step++]=(int16_t)y;
  // DELAY1
  x=x-(sound*MR_DOUBLE1_DECAY)/RATIO;
  y=sound+(x*MR_DOUBLE1_DECAY)/RATIO;
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
  ar.m.double1_buf[ar.m.double1_step++]=(int16_t)y;
  in=x;

  // DELAY 5 ms
  x=ar.m.delay5ms_buf[ar.m.delay5ms_step];
  if (in>32700) in=32700; else if (in<-32700) in=-32700;
  ar.m.delay5ms_buf[ar.m.delay5ms_step++]=(int16_t)in;

  // allpass
  y=ar.m.allpass_buf[ar.m.allpass_step];
  y=y-(x*MR_ALLPASS_DECAY)/RATIO;
  x=x+(y*MR_ALLPASS_DECAY)/RATIO;
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
  ar.m.allpass_buf[ar.m.allpass_step++]=(int16_t)x;

  // DELAY 67 ms
  x=ar.m.delay67ms_buf[ar.m.delay67ms_step];
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
  ar.m.delay67ms_buf[ar.m.delay67ms_step++]=(int16_t)y;
  all=in+x;

  // DELAY 15 ms
  y=ar.m.delay15ms_buf[ar.m.delay15ms_step];
  ar.m.delay15ms_buf[ar.m.delay15ms_step++]=(int16_t)x;
  in=sound/2+y;

  // single allpass
  // DELAY2
  x=ar.m.single1_buf[ar.m.single1_step];
  y=ar.m.single2_buf[ar.m.single2_step];
  y=y-(x*MR_SINGLE2_DECAY)/RATIO;
  x=x+(y*MR_SINGLE2_DECAY)/RATIO;
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
  ar.m.single2_buf[ar.m.single2_step++]=(int16_t)x;
  // DELAY1
  y=y-(in*MR_SINGLE1_DECAY)/RATIO;
  x=in+(y*MR_SINGLE1_DECAY)/RATIO;
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
  ar.m.single1_buf[ar.m.single1_step++]=(int16_t)x;
  all+=y; // gain 0.3

  // DELAY 108 ms
  x=ar.m.delay108ms_buf[ar.m.delay108ms_step];
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
  ar.m.delay108ms_buf[ar.m.delay108ms_step++]=(int16_t)y;
  y=all/2;
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
  analog_dac[0]=(int16_t)(x/2);
  analog_dac[1]=(int16_t)y; // аудио сигнал готовый для вывода на ЦАП
}

void Mikelson_large_room(void)
{
  // ревербератор по алгоритму Gardner Large Room Reverberator
  if (ar.l.allpass1_step>LR_ALLPASS1_DELAY-1) ar.l.allpass1_step=0;
  if (ar.l.allpass2_step>LR_ALLPASS2_DELAY-1) ar.l.allpass2_step=0;
  if (ar.l.delay4ms_step>LR_DELAY_4ms-1) ar.l.delay4ms_step=0;
  if (ar.l.delay17ms_step>LR_DELAY_17ms-1) ar.l.delay17ms_step=0;
  if (ar.l.single1_step>LR_SINGLE1_DELAY-1) ar.l.single1_step=0;
  if (ar.l.single2_step>LR_SINGLE2_DELAY-1) ar.l.single2_step=0;
  if (ar.l.delay31ms_step>LR_DELAY_31ms-1) ar.l.delay31ms_step=0;
  if (ar.l.delay3ms_step>LR_DELAY_3ms-1) ar.l.delay3ms_step=0;
  if (ar.l.double1_step>LR_DOUBLE1_DELAY-1) ar.l.double1_step=0;
  if (ar.l.double2_step>LR_DOUBLE2_DELAY-1) ar.l.double2_step=0;
  if (ar.l.double3_step>LR_DOUBLE3_DELAY-1) ar.l.double3_step=0;

  // allpass 1
  y=ar.l.allpass1_buf[ar.l.allpass1_step];
  y=y-(sound*LR_ALLPASS1_DECAY)/RATIO;
  x=sound+(y*LR_ALLPASS1_DECAY)/RATIO;
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
  ar.l.allpass1_buf[ar.l.allpass1_step++]=(int16_t)x;

  // allpass 2
  x=ar.l.allpass2_buf[ar.l.allpass2_step];
  x=x-(y*LR_ALLPASS2_DECAY)/RATIO;
  y=y+(x*LR_ALLPASS2_DECAY)/RATIO;
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
  ar.l.allpass2_buf[ar.l.allpass2_step++]=(int16_t)y;

  // DELAY 4 ms
  y=ar.l.delay4ms_buf[ar.l.delay4ms_step];
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
  ar.l.delay4ms_buf[ar.l.delay4ms_step++]=(int16_t)x;
  all=y*15;

  // DELAY 17 ms
  in=ar.l.delay17ms_buf[ar.l.delay17ms_step];
  ar.l.delay17ms_buf[ar.l.delay17ms_step++]=(int16_t)y;

  // single allpass
  // DELAY2
  x=ar.l.single1_buf[ar.l.single1_step];
  y=ar.l.single2_buf[ar.l.single2_step];
  y=y-(x*LR_SINGLE2_DECAY)/RATIO;
  x=x+(y*LR_SINGLE2_DECAY)/RATIO;
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
  ar.l.single2_buf[ar.l.single2_step++]=(int16_t)x;
  // DELAY1
  y=y-(in*LR_SINGLE1_DECAY)/RATIO;
  x=in+(y*LR_SINGLE1_DECAY)/RATIO;
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
  ar.l.single1_buf[ar.l.single1_step++]=(int16_t)x;

  // DELAY 31 ms
  x=ar.l.delay31ms_buf[ar.l.delay31ms_step];
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
  ar.l.delay31ms_buf[ar.l.delay31ms_step++]=(int16_t)y;
  all+=x*8;

  // DELAY 3 ms
  in=ar.l.delay3ms_buf[ar.l.delay3ms_step];
  ar.l.delay3ms_buf[ar.l.delay3ms_step++]=(int16_t)x;

  // double allpass
  // DELAY2
  x=ar.l.double1_buf[ar.l.double1_step];
  y=ar.l.double2_buf[ar.l.double2_step];
  y=y-(x*LR_DOUBLE2_DECAY)/RATIO;
  x=x+(y*LR_DOUBLE2_DECAY)/RATIO;
  if (x>32700) x=32700; else if (x<-32700) x=-32700;
  ar.l.double2_buf[ar.l.double2_step++]=(int16_t)x;
  // DELAY3
  x=ar.l.double3_buf[ar.l.double3_step];
  x=x-(y*LR_DOUBLE3_DECAY)/RATIO;
  y=y+(x*LR_DOUBLE3_DECAY)/RATIO;
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
  ar.l.double3_buf[ar.l.double3_step++]=(int16_t)y;
  // DELAY1
  x=x-(in*LR_DOUBLE1_DECAY)/RATIO;
  y=in+(x*LR_DOUBLE1_DECAY)/RATIO;
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
  ar.l.double1_buf[ar.l.double1_step++]=(int16_t)y;
  all+=x*8;
  y=all/20;
  if (y>32700) y=32700; else if (y<-32700) y=-32700;
  analog_dac[0]=(int16_t)(x/2);
  analog_dac[1]=(int16_t)y; // аудио сигнал готовый для вывода на ЦАП
}
