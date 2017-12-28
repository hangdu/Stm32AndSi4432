/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "Si4432Config.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/


/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/
  
/**
  * @brief  This function handles USARTx global interrupt request
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
    if ((USART1->SR & USART_FLAG_RXNE) != (u16)RESET)
    {
        int i = USART_ReceiveData(USART1);
        if(i == '1'){
            GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_SET);        // Set '1' on PA8
					  char test[] = "LED ON\r\n";
            UARTSend(test,sizeof(test));    // Send message to UART1
        }
        else if(i == '0'){
            GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_RESET);      // Set '0' on PA8
					  char test[] = "LED OFF\r\n";
            UARTSend(test,sizeof(test));			
        }
				
				else if(i == '2'){
            
						tx_data();	
						rx_data();
						while(GPIO_ReadInputDataBit(GPIOA, nIRQ));
						u8 ItStatus1 = SI4432_ReadReg(0x03);		//?????????
						u8 ItStatus2 = SI4432_ReadReg(0x04);		//?????????
						//burst read
						GPIO_ResetBits(GPIOA, nSEL);
						SPI1_ReadWriteByte(0x7F);
						char rx_buf1[10];
						for (int i = 0; i < 10; i++) 
						{
							rx_buf1[i] = SPI1_ReadWriteByte(0xFF);
						}
						GPIO_SetBits(GPIOA, nSEL);
						//enter ready mode
						SI4432_WriteReg(0x07, SI4432_PWRSTATE_READY);	
						GPIO_ResetBits(GPIOC, GPIO_Pin_13);
						delay_ms(500);
						GPIO_SetBits(GPIOC, GPIO_Pin_13);	
						delay_ms(3000);
					 
            UARTSend(rx_buf1,sizeof(rx_buf1));			
        }
				
    }
}


/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None

void SysTick_Handler(void)
{
}
  */


void EXTI0_IRQHandler(void)
{
	
}


void EXTI1_IRQHandler(void)
{
	
}



/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
