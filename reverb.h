/*
 * reverb.h
 *
 *  Created on: 10.08.2012
 *      Author: igor
 */

#ifndef REVERB_H_
#define REVERB_H_
#include <stdint.h>
/*
 * MCLK I2S (9МГц) настроена на частоту выборки 35156 Гц. = 28.(4) мкс.
 * SCLK I2S 6750 KHz
 */
typedef struct
{
	uint8_t delay; // значение задержки
	uint8_t decay; // значение уровня задержки
	uint8_t defuse; // значение уровня примешивания
}SETTING;

#define FS 35156 // частота выборки cs4221 Fs = 35156hz (28,4446mks)
#define RATIO 16384
#define DELAY_COUNT_MAX 30000 // величина массива задержанного звука (макс. задержка ~ 853мс)

// константы для алгоритмa эмуляции микросхемы nm3011
#define NM_DELAY1 ((1980*FS)/100000) // 19.8ms
#define NM_DELAY2 ((3310*FS)/100000) // 33.1ms
#define NM_DELAY3 ((5970*FS)/100000) // 59.7ms
#define NM_DELAY4 ((8630*FS)/100000) // 86.3ms
#define NM_DELAY5 ((13950*FS)/100000) // 139.5ms
#define NM_DELAY6 ((16640*FS)/100000) // 166.4ms
#define NM_DECAY1 RATIO // 1
#define NM_DECAY2 ((909*RATIO)/1000) // 0.909
#define NM_DECAY3 ((833*RATIO)/1000) // 0.833
#define NM_DECAY4 ((769*RATIO)/1000) // 0.769
#define NM_DECAY5 ((667*RATIO)/1000) // 0.667
#define NM_DECAY6 ((625*RATIO)/1000) // 0.625

// константы для алгоритма Датторро
#define PREDELAY_MAX ((5000*FS)/100000) //1800 // максимальная задержка 50 мс
#define INPUT1_COUNT ((471*FS)/100000) // 4.71
#define INPUT2_COUNT ((360*FS)/100000) // 3.6
#define INPUT3_COUNT ((1270*FS)/100000) // 12.7
#define INPUT4_COUNT ((930*FS)/100000) // 9.3
#define DECAY1_COUNT (((2258*FS)/100000)+16) // 22.58
#define DECAY2_COUNT ((6048*FS)/100000) // 60.48
#define DECAY3_COUNT (((3050*FS)/100000)+16) // 30.5
#define DECAY4_COUNT ((8924*FS)/100000) // 89.24
#define DELAY1_COUNT ((14960*FS)/100000) // 149.6
#define DELAY2_COUNT ((12500*FS)/100000) // 125
#define DELAY3_COUNT ((14170*FS)/100000) // 141.7
#define DELAY4_COUNT ((10628*FS)/100000) // 106.28

// константы для алгоритма Гарднера SR - Small Room
#define SR_PREDELAY_COUNT ((2400*FS)/100000) // 24 ms
#define SR_DOUBLE1_DELAY ((470*FS)/100000) // 4.7 ms
#define SR_DOUBLE1_DECAY ((150*RATIO)/1000) // 0.15
#define SR_DOUBLE2_DELAY ((2200*FS)/100000) // 22 ms
#define SR_DOUBLE2_DECAY ((250*RATIO)/1000) // 0.25
#define SR_DOUBLE3_DELAY ((830*FS)/100000) //8.3 ms
#define SR_DOUBLE3_DECAY ((300*RATIO)/1000) // 0.3
#define SR_SINGLE1_DELAY ((3300*FS)/100000) // 33 ms
#define SR_SINGLE1_DECAY ((80*RATIO)/1000) // 0.08
#define SR_SINGLE2_DELAY ((3000*FS)/100000) // 30 ms
#define SR_SINGLE2_DECAY SR_DOUBLE3_DECAY // 0.3

// MR - Medium Room
#define MR_DOUBLE1_DELAY SR_DOUBLE1_DELAY // 4.7 ms
#define MR_DOUBLE1_DECAY SR_DOUBLE2_DECAY // 0.25
#define MR_DOUBLE2_DELAY SR_DOUBLE3_DELAY //8.3 ms
#define MR_DOUBLE2_DECAY ((350*RATIO)/1000) // 0.35
#define MR_DOUBLE3_DELAY SR_DOUBLE2_DELAY // 22 ms
#define MR_DOUBLE3_DECAY ((450*RATIO)/1000) // 0.45
#define MR_DELAY_5ms ((500*FS)/100000)
#define MR_ALLPASS_DELAY SR_SINGLE2_DELAY // 30 ms
#define MR_ALLPASS_DECAY MR_DOUBLE3_DECAY // 0.45
#define MR_DELAY_67ms ((6700*FS)/100000)
#define MR_DELAY_15ms ((1500*FS)/100000)
#define MR_SINGLE1_DELAY ((2920*FS)/100000) // 29.2 ms
#define MR_SINGLE1_DECAY SR_DOUBLE2_DECAY // 0.25
#define MR_SINGLE2_DELAY ((980*FS)/100000) // 9.8 ms
#define MR_SINGLE2_DECAY MR_DOUBLE2_DECAY // 0.35
#define MR_DELAY_108ms ((10800*FS)/100000)

// LR - Large Room
#define LR_ALLPASS1_DELAY ((800*FS)/100000) // 8 ms
#define LR_ALLPASS1_DECAY SR_DOUBLE3_DECAY // 0.3
#define LR_ALLPASS2_DELAY ((1200*FS)/100000) // 12 ms
#define LR_ALLPASS2_DECAY SR_DOUBLE3_DECAY // 0.3
#define LR_DELAY_4ms ((400*FS)/100000)
#define LR_DELAY_17ms ((1700*FS)/100000)
#define LR_SINGLE1_DELAY ((2500*FS)/100000) // 25 ms
#define LR_SINGLE1_DECAY ((500*RATIO)/1000) // 0.5
#define LR_SINGLE2_DELAY ((6200*FS)/100000) // 62 ms
#define LR_SINGLE2_DECAY SR_DOUBLE2_DECAY // 0.25
#define LR_DELAY_31ms ((3100*FS)/100000)
#define LR_DELAY_3ms ((300*FS)/100000)
#define LR_DOUBLE1_DELAY ((1400*FS)/100000) // 14 ms
#define LR_DOUBLE1_DECAY LR_SINGLE1_DECAY // 0.5
#define LR_DOUBLE2_DELAY ((7600*FS)/100000) //76 ms
#define LR_DOUBLE2_DECAY SR_DOUBLE2_DECAY // 0.25
#define LR_DOUBLE3_DELAY ((3000*FS)/100000) // 30 ms
#define LR_DOUBLE3_DECAY SR_DOUBLE2_DECAY // 0.25

typedef struct {
	// структура данных для эффекта ревербератор по алгоритму Датторро
//	int32_t left; // выходной сигнал левого канала
	int32_t right; // выходной сигнал правого канала
	int32_t bandwidth; // входной фильтр
	int32_t damping_right; // фильтр правого канала
	int32_t damping_left; // фильтр левого канала
	uint16_t predelay_step; // счетчик положения в массиве
	uint16_t input1_step; // счетчик положения в массиве
	uint16_t input2_step; // счетчик положения в массиве
	uint16_t input3_step; // счетчик положения в массиве
	uint16_t input4_step; // счетчик положения в массиве
	uint16_t decay1_step; // счетчик положения в массиве
	uint16_t decay2_step; // счетчик положения в массиве
	uint16_t decay3_step; // счетчик положения в массиве
	uint16_t decay4_step; // счетчик положения в массиве
	uint16_t delay1_step; // счетчик положения в массиве
	uint16_t delay2_step; // счетчик положения в массиве
	uint16_t delay3_step; // счетчик положения в массиве
	uint16_t delay4_step; // счетчик положения в массиве
	int16_t predelay_buf[PREDELAY_MAX]; // массив для предзадержки в ревербераторе
	int16_t input1_buf[INPUT1_COUNT]; // массив для input diffusion
  int16_t input2_buf[INPUT2_COUNT]; // массив для input diffusion
  int16_t input3_buf[INPUT3_COUNT]; // массив для input diffusion
  int16_t input4_buf[INPUT4_COUNT]; // массив для input diffusion
  int16_t decay1_buf[DECAY1_COUNT]; // массив для decay diffusion
  int16_t decay2_buf[DECAY2_COUNT]; // массив для decay diffusion
  int16_t decay3_buf[DECAY3_COUNT]; // массив для decay diffusion
  int16_t decay4_buf[DECAY4_COUNT]; // массив для decay diffusion
  int16_t delay1_buf[DELAY1_COUNT]; // массив для простой задержки
  int16_t delay2_buf[DELAY2_COUNT]; // массив для простой задержки
  int16_t delay3_buf[DELAY3_COUNT]; // массив для простой задержки
  int16_t delay4_buf[DELAY4_COUNT]; // массив для простой задержки
}DATTORRO;

typedef struct
{
  //  структура данных для эффекта ревербератор по алгоритму Mikelson'a Small Room
  uint16_t predelay_step;
  uint16_t double1_step;
  uint16_t double2_step;
  uint16_t double3_step;
  uint16_t single1_step;
  uint16_t single2_step;
  int16_t predelay_buf[SR_PREDELAY_COUNT];
  int16_t double1_buf[SR_DOUBLE1_DELAY];
  int16_t double2_buf[SR_DOUBLE2_DELAY];
  int16_t double3_buf[SR_DOUBLE3_DELAY];
  int16_t single1_buf[SR_SINGLE1_DELAY];
  int16_t single2_buf[SR_SINGLE2_DELAY];
}SMALL_ROOM;

typedef struct
{
  //  структура данных для эффекта ревербератор по алгоритму Mikelson'a Medium Room
  uint16_t double1_step;
  uint16_t double2_step;
  uint16_t double3_step;
  uint16_t delay5ms_step;
  uint16_t allpass_step;
  uint16_t delay67ms_step;
  uint16_t delay15ms_step;
  uint16_t single1_step;
  uint16_t single2_step;
  uint16_t delay108ms_step;
  int16_t double1_buf[MR_DOUBLE1_DELAY];
  int16_t double2_buf[MR_DOUBLE2_DELAY];
  int16_t double3_buf[MR_DOUBLE3_DELAY];
  int16_t delay5ms_buf[MR_DELAY_5ms];
  int16_t allpass_buf[MR_ALLPASS_DELAY];
  int16_t delay67ms_buf[MR_DELAY_67ms];
  int16_t delay15ms_buf[MR_DELAY_15ms];
  int16_t single1_buf[MR_SINGLE1_DELAY];
  int16_t single2_buf[MR_SINGLE2_DELAY];
  int16_t delay108ms_buf[MR_DELAY_108ms];
}MEDIUM_ROOM;

typedef struct
{
  //  структура данных для эффекта ревербератор по алгоритму Mikelson'a Medium Room
  uint16_t allpass1_step;
  uint16_t allpass2_step;
  uint16_t delay4ms_step;
  uint16_t delay17ms_step;
  uint16_t single1_step;
  uint16_t single2_step;
  uint16_t delay31ms_step;
  uint16_t delay3ms_step;
  uint16_t double1_step;
  uint16_t double2_step;
  uint16_t double3_step;
  int16_t allpass1_buf[LR_ALLPASS1_DELAY];
  int16_t allpass2_buf[LR_ALLPASS2_DELAY];
  int16_t delay4ms_buf[LR_DELAY_4ms];
  int16_t delay17ms_buf[LR_DELAY_17ms];
  int16_t single1_buf[LR_SINGLE1_DELAY];
  int16_t single2_buf[LR_SINGLE2_DELAY];
  int16_t delay31ms_buf[LR_DELAY_31ms];
  int16_t delay3ms_buf[LR_DELAY_3ms];
  int16_t double1_buf[LR_DOUBLE1_DELAY];
  int16_t double2_buf[LR_DOUBLE2_DELAY];
  int16_t double3_buf[LR_DOUBLE3_DELAY];
}LARGE_ROOM;


typedef union {
	int16_t delay_buf[DELAY_COUNT_MAX]; // массив задержанного звука для эффекта delay
	DATTORRO d;
	SMALL_ROOM s;
	MEDIUM_ROOM m;
	LARGE_ROOM l;
}ALGORITM;

#endif /* REVERB_H_ */
