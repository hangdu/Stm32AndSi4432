/**
  ******************************************************************************
  * @file		delay.c
  * @author	Yohanes Erwin Setiawan
  * @date		10 January 2016
  ******************************************************************************
  */
	
#include "delay.h"

void delay_ms(u32 nus)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	SysTick_SetReload(9000*nus);
	SysTick_ITConfig(DISABLE);
	SysTick_CounterCmd(SysTick_Counter_Enable);
	
	while(RESET == SysTick_GetFlagStatus(SysTick_FLAG_COUNT));
	SysTick_CounterCmd(SysTick_Counter_Disable);
}






/********************************* END OF FILE ********************************/
/******************************************************************************/
