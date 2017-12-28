#include "UartConfiguration.h"

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports
*******************************************************************************/
void USART1_Init(void)
{
	/* Enable USART1 and GPIOA clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	/* Enable the USARTx Interrupt */
	NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Configure (PA.8) as output */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure); // Save
  
  /* Configure USART1 Tx (PA.09) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Configure USART1 Rx (PA.10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* USART1 configuration ------------------------------------------------------*/	
	USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 9600;        // Baud Rate
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  USART_Init(USART1, &USART_InitStructure); 
  /* Enable USART1 */
  USART_Cmd(USART1, ENABLE);
	
	/* Enable the USART1 Receive interrupt: this interrupt is generated when the
         USART1 receive data register is not empty */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}
  

/*******************************************************************************
* Function Name  : UARTSend
* Description    : Send a string to the UART.
* Input          : - pucBuffer: buffers to be printed.
*                : - ulCount  : buffer's length
*******************************************************************************/
void UARTSend(char *pucBuffer, unsigned long ulCount)
{
    //
    // Loop while there are more characters to send.
    //
    while(ulCount != 1)
    {
			  char c = (uint16_t) *pucBuffer;
        USART_SendData(USART1, (uint16_t) *pucBuffer++);
        /* Loop until the end of transmission */
        //USART_FLAG_TC: ???????
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
        {
        }
				ulCount--;
    }
}
