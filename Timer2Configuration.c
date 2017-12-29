#include "Timer2Configuration.h"

void Timer2Init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	NVIC_InitTypeDef nvicStructure;
  nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
  nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
  nvicStructure.NVIC_IRQChannelSubPriority = 1;
  nvicStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvicStructure);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Period = 9000;
  TIM_TimeBaseStructure.TIM_Prescaler = 8000;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  
  /* Enables the TIM2 counter */
  //TIM_Cmd(TIM2, ENABLE);
  
  /* Enables the TIM2 Interrupt source */
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}