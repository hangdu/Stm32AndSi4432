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

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup SPI_FullDuplex_SoftNSS
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BufferSize 32

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SPI_InitTypeDef   SPI_InitStructure;
__IO uint8_t TxIdx = 0, RxIdx = 0, k = 0;
																			
u8 test = 0x20;

/* Private functions ---------------------------------------------------------*/
void SI4432_init(void);
void RCC_Configuration(void);
void GPIO_Configuration(uint16_t SPIy_Mode);
u8 SPI1_ReadWriteByte(u8 TxData);
u8 SI4432_ReadReg(u8 addr);
void SI4432_WriteReg(u8 addr, u8 value);
																																																							
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


  while (1)
  {
		test = SI4432_ReadReg(0x00);
	}
}

void SI4432_init(void)
{		
	u8 ItStatus1 = SI4432_ReadReg(0x03);
	u8 ItStatus2 = SI4432_ReadReg(0x04);
	
	SI4432_WriteReg(0x06, 0x00);
	test = SI4432_ReadReg(0x06);
	
	SI4432_WriteReg(0x06, 0x01);
	test = SI4432_ReadReg(0x06);
	
	

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
	
	/* Configure MISO 
	GPIO_InitStructure.GPIO_Pin = SPIy_PIN_MISO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	*/
	
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

u8 SPI1_ReadWriteByte(u8 TxData)
{		
		u8 retry = 0;				 	
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //wait
		{
			retry++;
			if(retry > 200) return 0;
		}			  
		SPI_I2S_SendData(SPI1, TxData);
		
		retry=0; 
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //wait
		{
			retry++;
			if(retry > 200)	return 0xFF;
		}	  						    
		return SPI_I2S_ReceiveData(SPI1); //????SPIx???????			    
}


u8 SI4432_ReadReg(u8 addr)     //?????? 8???????
{
    uint8_t temp = 0;
		GPIO_ResetBits(GPIOA, nSEL);
		SPI1_ReadWriteByte(addr);    //?????????
		temp = SPI1_ReadWriteByte(0Xff);
		GPIO_SetBits(GPIOA, nSEL);
	  return temp;
}
	
void SI4432_WriteReg(u8 addr, u8 value) 
{
		uint8_t temp = 0;
		GPIO_ResetBits(GPIOA, nSEL);
		
		SPI1_ReadWriteByte(addr | 0x80);    //?????????
		SPI1_ReadWriteByte(value);
		GPIO_SetBits(GPIOA, nSEL);
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
