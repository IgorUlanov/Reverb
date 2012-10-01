#include "init.h"
#include "common.h"
//#include <misc.h>
#include <stdint.h>
#include <stm32f10x.h>

GPIO_InitTypeDef GPIO_InitStructure;
DMA_InitTypeDef DMA_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
ADC_InitTypeDef  ADC_InitStructure;
NVIC_InitTypeDef  NVIC_InitStructure;
EXTI_InitTypeDef   EXTI_InitStructure;
I2C_InitTypeDef   I2C_InitStructure;
I2S_InitTypeDef   I2S_InitStructure;
SPI_InitTypeDef  SPI_InitStructure;

extern uint16_t analog_adc[2]; // массив со значениями от АЦП
extern uint16_t analog_dac[2]; // массив со значениями для ЦАП
extern volatile uint16_t control_buf[ADC_CONTROL_COUNT*2]; // массив c результатами измерения управления.

void Begin_config(void)
{
  // начальный этап конфигурации
  RCC_PCLK2Config(RCC_HCLK_Div8); // APB2 = 9MHz
  RCC_ADCCLKConfig(RCC_PCLK2_Div8); // 9/8=1.125MHz
  NVIC_SetVectorTable(NVIC_VectTab_FLASH,0);
  SysTick_Config(SystemCoreClock/1000); // системный таймер = 1 мс.
  RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPDEN, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPEEN, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2ENR_ADC1EN, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1ENR_I2C2EN, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 | RCC_APB1Periph_SPI3, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void Final_config(void)
{
  // заключительный этап конфигурации
  // приоритеты
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void Audio_config(void)
{
  /*
   * интерфейс с декодером sc2441
   * через i2c - настройка
   * через pci - аудисигнал
   * i2s2 используется для передачи данных на АЦП - режим slave tx.
   * i2s3 используется для приема данных с ЦАПа - режим slave rx.
   * Данные выгружаются и загружаются с помощью дма. Обработка по прерыванию dma2_ch1, после того, как считаны данные
   * из АЦП правого и левого канала.
   * pa8 - xti, pb7 - rst, pe14 - i2c/spi, pe15 - ad0/cs
   */
  // pa4 - i2c/spi, pa6 - ad0/cs
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOA,GPIO_Pin_4 | GPIO_Pin_6);
  // reset
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  // pa5 - spi1 sck, pa7 - spi1 mosi
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_Init(SPI1, &SPI_InitStructure);
  /* Enable SPI_MASTER */
  SPI_Cmd(SPI1, ENABLE);
  SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_TXE,ENABLE);

  GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
  // на выход
  // I2S3 : CK, I2S2 : SD
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  // I2S3 : MCLK
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  // I2S3: WS
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  // на вход
  // I2S2 : CK, WS; I2S3 : SD
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  // I2S2 : MCLK
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  // на SDOUT для перевода в слайв режим cs4221 должна быть 1.
  GPIO_SetBits(GPIOB,GPIO_Pin_5);

  DMA_DeInit(DMA2_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(SPI3->DR));
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&analog_adc[0];
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 2;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA2_Channel1, &DMA_InitStructure);
  DMA_ITConfig(DMA2_Channel1,DMA_IT_TC,ENABLE);
  DMA_Cmd(DMA2_Channel1, ENABLE);

  DMA_DeInit(DMA1_Channel5);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(SPI2->DR));
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&analog_dac[0];
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 2;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel5, &DMA_InitStructure);
  DMA_Cmd(DMA1_Channel5, ENABLE);

  SPI_I2S_DeInit(SPI2);
  SPI_I2S_DeInit(SPI3);
	I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
	I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16bextended;
	I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
	I2S_InitStructure.I2S_AudioFreq = 35156;//I2S_AudioFreq_32k;
	I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
	I2S_InitStructure.I2S_Mode = I2S_Mode_MasterRx;
  I2S_Init(SPI3, &I2S_InitStructure);
	I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
	I2S_InitStructure.I2S_Mode = I2S_Mode_SlaveTx;
  I2S_Init(SPI2, &I2S_InitStructure);
  SPI_I2S_DMACmd(SPI3,SPI_I2S_DMAReq_Rx, ENABLE);
  SPI_I2S_DMACmd(SPI2,SPI_I2S_DMAReq_Tx, ENABLE);
  I2S_Cmd(SPI2, ENABLE);
  I2S_Cmd(SPI3, ENABLE);
  // exti2 config
  EXTI_InitStructure.EXTI_Line = EXTI_Line2;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}

void Control_config(void)
{
  /*
   * Измерение напряжения управляющих сигналов.
   *
   * Измерение производится АЦП 1, используя каналы 0 - 3 (PА0 - PА3).
   * каналы опрашиваются поочередно в 16 тактном цикле. АЦП работает в режиме самозапуска.
   * оцифрованные данные скидываются в память DMA1 channel1.
   *
   * Массив для измеренных значений двойного размера, чтобы успевать пересчитывать значения перед следующим измерением.
   */
  // порты PА1-РА3 используются для измерения напряжения управляющих сигналов..
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&control_buf[0];
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = ADC_CONTROL_COUNT*2;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
  DMA_ITConfig(DMA1_Channel1,DMA_IT_HT,ENABLE);
  DMA_Cmd(DMA1_Channel1, ENABLE);

  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = ADC_CONTROL_COUNT;
  ADC_Init(ADC1,&ADC_InitStructure);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_0,2,ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_0,3,ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_0,4,ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_1,5,ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_1,6,ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_1,7,ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_1,8,ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_2,9,ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_2,10,ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_2,11,ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_2,12,ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_3,13,ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_3,14,ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_3,15,ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_3,16,ADC_SampleTime_239Cycles5);
  ADC_DMACmd(ADC1, ENABLE);
  ADC_Cmd(ADC1, ENABLE);
  ADC_ResetCalibration(ADC1);
  while(ADC_GetResetCalibrationStatus(ADC1));
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));
}

void Led_config(void)
{
	// светодиод уровня сигнала подключен к таймеру 1 зеленый к каналу 1 (PE9), а красный к каналу 2 (PE11).
	// светодиод режим - PE14.
  // PD6 - флеш (не светится - считывание параметров из флеш,
  // светится - считывание параметров с регуляторов, мигает - запись параметров во флеш)
  // PD4 - режим (светится - эффект включен, мигает - эффект выключен, не светится - нет питания)
  // PD6 - отладка
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);

  /*
   * максимальное значение накопленой перегрузки 64 (максимальное значение за такт АЦП)
   * умноженое на количество накоплений за 10мс 35156Гц(Fs)/100Гц (за 10мс)=352*64= 22528
   * поэтому при прескалере = 8 при частоте таймера ~100Гц, максимальное значение каналов = 22528
   */
  TIM_TimeBaseStructure.TIM_Period = 22527;
  TIM_TimeBaseStructure.TIM_Prescaler = 7;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  /* зеленый */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OC1Init(TIM1, &TIM_OCInitStructure);
  /* красный */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC2Init(TIM1, &TIM_OCInitStructure);
  TIM_ARRPreloadConfig(TIM1, ENABLE);
  TIM_Cmd(TIM1, ENABLE);
  TIM_CtrlPWMOutputs(TIM1,ENABLE);
}
