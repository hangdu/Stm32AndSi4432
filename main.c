/**
  ******************************************************************************
  * @file    SPI/FullDuplex_SoftNSS/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
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
#include "stm32f10x.h"
#include "platform_config.h"
#include "delay.h"
#include "Si4432Config.h"

unsigned char rx_buf[15];
/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
u8 ItStatus1;
u8 ItStatus2;

/* Private define ------------------------------------------------------------*/
#define BufferSize 32
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SPI_InitTypeDef   SPI_InitStructure;								
u8 test = 0x20;
/* Private functions ---------------------------------------------------------*/

void RCC_Configuration(void);
void GPIO_Configuration(uint16_t SPIy_Mode);
																																															
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* System clocks configuration ---------------------------------------------*/
  RCC_Configuration();
  /* GPIO configuration ------------------------------------------------------*/
  GPIO_Configuration(SPI_Mode_Master);
	
  DelayInit();
	
  /* SPIy Config -------------------------------------------------------------*/
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPIy, &SPI_InitStructure);
	
	/* Enable SPIy */
  SPI_Cmd(SPIy, ENABLE);
	SI4432_init();
	TX0_RX0;
	
	/* code for tx data 
	while (1) 
	{
		tx_data();
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		DelayMs(500);
		GPIO_SetBits(GPIOC, GPIO_Pin_13);	
		DelayMs(5000);
	}
	*/
	
	rx_data();
	unsigned char chksum;
	unsigned char i;
	
	while (1)
	{
		if(!GPIO_ReadInputDataBit(GPIOA, nIRQ)) 
		{
			//nIRQ is low.
			ItStatus1 = SI4432_ReadReg(0x03);		
			ItStatus2 = SI4432_ReadReg(0x04);		
			
			//burst read
			GPIO_ResetBits(GPIOA, nSEL);
			SPI1_ReadWriteByte(0x7F);
			for (int i = 0; i < 10; i++) 
			{
				rx_buf[i] = SPI1_ReadWriteByte(0xFF);
			}
			GPIO_SetBits(GPIOA, nSEL);
			//enter ready mode
			SI4432_WriteReg(0x07, SI4432_PWRSTATE_READY);	
       //If you want to enter receive mode. You need to do all the configuration again
			
			chksum = 0;
			for(i=0;i<9;i++)
			{
				chksum += rx_buf[i];  
			}	
			if((chksum == rx_buf[9] )&&( rx_buf[0] == 0x41 ))    					
     	{    
        //data is right.				
     		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
				DelayMs(500);
				GPIO_SetBits(GPIOC, GPIO_Pin_13);	
				rx_data();
      } 
			else 
			{
				GPIO_ResetBits(GPIOC, GPIO_Pin_13);
				break;
			}						        	        				    				
		}		
	}
	while(1);
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
  /* PCLK2 = HCLK/2 */
  RCC_PCLK2Config(RCC_HCLK_Div2); 
	//all the connetions between stm32 and Si4432 are in GPIOA.
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
	
	/* GPIOC clock enable (For testing LED use) */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

/**
  * @brief  Configures the different SPIy and SPIz GPIO ports.
  * @param  SPIy_Mode: Specifies the SPIy operating mode. 
  *            This parameter can be:
  *              -  SPIy_Mode_Master
  *              -  SPIy_Mode_Slave                 
  * @param  SPIz_Mode: Specifies the SPIz operating mode. 
  *            This parameter can be:
  *              -  SPIz_Mode_Master
  *              -  SPIz_Mode_Slave 
  * @retval None
  */
void GPIO_Configuration(uint16_t SPIy_Mode)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	/* Configure SCK and MOSI */
  GPIO_InitStructure.GPIO_Pin = SPIy_PIN_SCK | SPIy_PIN_MOSI | SPIy_PIN_MISO;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//Configure and nSEL
	GPIO_InitStructure.GPIO_Pin = nSEL; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//Configure NIRQ
	GPIO_InitStructure.GPIO_Pin = nIRQ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//configure for testing LED
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}

#endif
/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
