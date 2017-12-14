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

#define  SI4432_PWRSTATE_READY		01
#define  TX1_RX0	SI4432_WriteReg(0x0e, 0x01)		// TX status
#define  TX0_RX1	SI4432_WriteReg(0x0e, 0x02)		// RX status
#define  TX0_RX0	SI4432_WriteReg(0x0e, 0x00)         // not TX status and not RX status

#define  SI4432_PWRSTATE_RX		05
#define  SI4432_PWRSTATE_TX		0x09	
#define  SI4432_Rx_packet_received_interrupt   0x02
#define  SI4432_PACKET_SENT_INTERRUPT	04
const unsigned char tx_test_data[10] = {0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x6d}; 
unsigned char rx_buf[15];

typedef struct 
{
	unsigned char reach_1s				: 1;
	unsigned char rf_reach_timeout			: 1;
	unsigned char is_tx				: 1;
}	FlagType;
FlagType Flag;

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

u8 ItStatus1;
u8 ItStatus2;

/* Private define ------------------------------------------------------------*/
#define BufferSize 32

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SPI_InitTypeDef   SPI_InitStructure;
__IO uint8_t TxIdx = 0, RxIdx = 0, k = 0;
																			
u8 test = 0x20;
u8 RSSI;

/* Private functions ---------------------------------------------------------*/
void SI4432_init(void);
void RCC_Configuration(void);
void GPIO_Configuration(uint16_t SPIy_Mode);
u8 SPI1_ReadWriteByte(u8 TxData);
u8 SI4432_ReadReg(u8 addr);
void SI4432_WriteReg(u8 addr, u8 value);
void tx_data(void);
void rx_data(void);
																																																							
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
	
	unsigned char chksum;
	unsigned char i;
	
	rx_data();
	SI4432_WriteReg(0x06, 0x40);	// Register 06h. Interrupt Enable 2 :   Enable Valid Preamble Detected.
	
	while (1)
	{
		if(!GPIO_ReadInputDataBit(GPIOA, nIRQ)) 
		{
			//read RSSI first
			RSSI = SI4432_ReadReg(0x26);
			//nIRQ is low.
			ItStatus1 = SI4432_ReadReg(0x03);		
			ItStatus2 = SI4432_ReadReg(0x04);	

			SI4432_WriteReg(0x06, 0x40);
			rx_data();
		}		
	}
}

void SI4432_init(void)
{		
	ItStatus1 = SI4432_ReadReg(0x03);
	ItStatus2 = SI4432_ReadReg(0x04);
	
	//unable all the interrupts that are not needed	
	SI4432_WriteReg(0x06, 0x00);
	test = SI4432_ReadReg(0x06);
	
	//Set ready mode
	SI4432_WriteReg(0x07, 0x01);
	test = SI4432_ReadReg(0x07);
	
	
	//load capacitance = 12P  Why we need this???
	SI4432_WriteReg(0x09, 0x7f);
	test = SI4432_ReadReg(0x09);
	
	//close low frequency output. 0x0a: Microcontroller output clock.
	SI4432_WriteReg(0x0a, 0x05);
	test = SI4432_ReadReg(0x0a);
	
	SI4432_WriteReg(0x0b, 0xea);
	test = SI4432_ReadReg(0x0b);
	
	SI4432_WriteReg(0x0c, 0xea);
	test = SI4432_ReadReg(0x0c);
	
	SI4432_WriteReg(0x0d, 0xf4);
	test = SI4432_ReadReg(0x0d);
	
	SI4432_WriteReg(0x70, 0x2c);    //0x70: Modulation Mode Control 1
	test = SI4432_ReadReg(0x70);
	SI4432_WriteReg(0x1d, 0x40);  // ?? afc: automatic frequency control
	test = SI4432_ReadReg(0x1d);
		
	// 1.2K bps setting
	SI4432_WriteReg(0x1c, 0x16);	// ???????Silabs ?Excel  0x1c:IF Filter Bandwidth
	test = SI4432_ReadReg(0x1c);
	
	
	SI4432_WriteReg(0x20, 0x83);   //0x20 Clock recovery oversampling rate
	test = SI4432_ReadReg(0x20);
	 
	SI4432_WriteReg(0x21, 0xc0); // 0x21  Clock recovery offset 2
	test = SI4432_ReadReg(0x21);
	SI4432_WriteReg(0x22, 0x13);//  0x22 Clock recovery offset 1
	test = SI4432_ReadReg(0x22);
	SI4432_WriteReg(0x23, 0xa9); //  0x23 Clock recovery offset 0
	test = SI4432_ReadReg(0x23);
	SI4432_WriteReg(0x24, 0x00); //  Clock recovery timing loop gain 1
	test = SI4432_ReadReg(0x24);
	SI4432_WriteReg(0x25, 0x04); //  Clock recovery timing loop gain 0
	test = SI4432_ReadReg(0x25);
	
	SI4432_WriteReg(0x2a, 0x14);
	test = SI4432_ReadReg(0x2a);
	SI4432_WriteReg(0x6e, 0x09);   //TX data rate 1
	test = SI4432_ReadReg(0x6e);
	SI4432_WriteReg(0x6f, 0xd5);   //TX data rate 0
	test = SI4432_ReadReg(0x6f);
	
	//1.2K bps setting end		
	SI4432_WriteReg(0x30, 0x8c);   // Enable Packet RX Handling + FIFO??,?????,??CRC??   0x30 Data Access Control	
	test = SI4432_ReadReg(0x30);	
	SI4432_WriteReg(0x32, 0xff);  // byte0, 1,2,3 ????  0x32: Header Control 1
	test = SI4432_ReadReg(0x32);
	SI4432_WriteReg(0x33, 0x42);//  byte 0,1,2,3 ???,Synchronization Word 3 and 2   ????   0x33: Header Control 2
                                       // packet length is included in the header
	test = SI4432_ReadReg(0x33);
	SI4432_WriteReg(0x34, 16);  // ??16?Nibble?Preamble   0x34: Preamble length
	test = SI4432_ReadReg(0x34);
	SI4432_WriteReg(0x35, 0x20);  // ????4?nibble?Preamble    0x35: Preamble Detection Control 1
	test = SI4432_ReadReg(0x35);
	SI4432_WriteReg(0x36, 0x2d);  // ???? 0x2dd4   0x36: Synchronization Word 3
	test = SI4432_ReadReg(0x36);
	SI4432_WriteReg(0x37, 0xd4);  //0x37 : Synchronization Word 2
	test = SI4432_ReadReg(0x37);
	SI4432_WriteReg(0x38, 0x00);   //0x38 : Synchronization Word 1
	test = SI4432_ReadReg(0x38);
	SI4432_WriteReg(0x39, 0x00);   //0x39 : Synchronization Word 0
	test = SI4432_ReadReg(0x39);
	SI4432_WriteReg(0x3a, 's');  // ??????: “swwx"  0x3a:Transmit Header 3
	test = SI4432_ReadReg(0x3a);
	SI4432_WriteReg(0x3b, 'w');   //0x3b:Transmit Header 2
	test = SI4432_ReadReg(0x3b);
	SI4432_WriteReg(0x3c, 'w');   // 0x3c:Transmit Header 1
	test = SI4432_ReadReg(0x3c);
	SI4432_WriteReg(0x3d, 'x');    //0x3d:Transmit Header 0
	test = SI4432_ReadReg(0x3d);
	SI4432_WriteReg(0x3e, 10);  // ????10??????   0x3e: Packet Length
	test = SI4432_ReadReg(0x3e);
	SI4432_WriteReg(0x3f, 's'); // ????????:”swwx"  0x3f: Check Header 3
	test = SI4432_ReadReg(0x3f);
	SI4432_WriteReg(0x40, 'w');  //0x40: Check Header 2
	test = SI4432_ReadReg(0x40);
	SI4432_WriteReg(0x41, 'w');   //0x41: Check Header 1
	test = SI4432_ReadReg(0x41);
	SI4432_WriteReg(0x42, 'x');   //0x42: Check Header 0
	test = SI4432_ReadReg(0x42);
	SI4432_WriteReg(0x43, 0xff);  // ??1,2,3,4 ?????????  0x43: Header Enable 3
	test = SI4432_ReadReg(0x43);
	SI4432_WriteReg(0x44, 0xff);  //   0x44: Header Enable 2
	test = SI4432_ReadReg(0x44);
	SI4432_WriteReg(0x45, 0xff);  //   0x45: Header Enable 1
	test = SI4432_ReadReg(0x45);
	SI4432_WriteReg(0x46, 0xff);  //   0x46: Header Enable 0
	test = SI4432_ReadReg(0x46);
	SI4432_WriteReg(0x6d, 0x07);  // ?????????  0x6d: TX power
	test = SI4432_ReadReg(0x6d);

	SI4432_WriteReg(0x79, 0x0);  // ?????  0x79: Frequency Hopping Channel Select
	test = SI4432_ReadReg(0x79);
	SI4432_WriteReg(0x7a, 0x0);  // ?????  0x7a: Frequency Hopping Step Size
	test = SI4432_ReadReg(0x7a);
	
	
	SI4432_WriteReg(0x71, 0x22); // ????? CLK,FiFo , FSK??  0x71: Modulation Mode Control 2
                                  //No TX Data CLK is available (asynchronous mode – Can only work with modulations FSK or OOK)		
	test = SI4432_ReadReg(0x71);
	SI4432_WriteReg(0x72, 0x30);  // ??? 30KHz  0x72: Frequency Deviation
	test = SI4432_ReadReg(0x72);
	
	SI4432_WriteReg(0x73, 0x0);  // ??????  0x73:Frequency Offset 1
	test = SI4432_ReadReg(0x73);
	SI4432_WriteReg(0x74, 0x0);  // ??????  0x74:Frequency Offset 2
	test = SI4432_ReadReg(0x74);
	
	SI4432_WriteReg(0x75, 0x53);  // ???? 434  0x75: Frequency Band Select
	test = SI4432_ReadReg(0x75);
	SI4432_WriteReg(0x76, 0x64);  // 0x76: Nominal Carrier Frequency
	test = SI4432_ReadReg(0x76);
	SI4432_WriteReg(0x77, 0x00);   // 0x77: Nominal Carrier Frequency
	test = SI4432_ReadReg(0x77);
}



void tx_data(void)
{
	unsigned char i;
	Flag.is_tx = 1;
	SI4432_WriteReg(0x07, SI4432_PWRSTATE_READY);	// rf Ready mode
	TX1_RX0;		//TX status
	DelayMs(5);		// ?? 5ms, ?????
	
	
  //clear the contents of the RX FIFO
  //clear the contents of the TX FIFO.
	SI4432_WriteReg(0x08, 0x03);  // 
	SI4432_WriteReg(0x08, 0x00);  // ???,?????
	
	SI4432_WriteReg(0x34, 40);  // 0x34: set Preamble Length=20bytes
	SI4432_WriteReg(0x3e, 10);  // 0x3e: Packet Length=10bytes
  for (i = 0; i<10; i++)
	{
		SI4432_WriteReg(0x7f, tx_test_data[i]); 	// ????????????
	}
	SI4432_WriteReg(0x05, SI4432_PACKET_SENT_INTERRUPT);	// Enable Packet Sent Interrupt
	ItStatus1 = SI4432_ReadReg(0x03);		
	ItStatus2 = SI4432_ReadReg(0x04);
	
	test = GPIO_ReadInputDataBit(GPIOA, nIRQ);
	SI4432_WriteReg(0x07, SI4432_PWRSTATE_TX);  // ??????
	/*
	rf_timeout = 0;
	Flag.rf_reach_timeout = 0;
	while(nIRQ)		// ????
	{
		
		if(Flag.rf_reach_timeout)
		{
			
			SDN  = 1;		//??0.5??????,?RF???????,??????????
			delay_1ms(10);
			SDN = 0;
			delay_1ms(200);
			
			SI4432_init();
			break;		// ?????
		}
			
	}	
  	rx_data();		//rf ????,??????
		*/
		//waiting for interrupt
		while(GPIO_ReadInputDataBit(GPIOA, nIRQ));
		//tx finished, nIRQ is low.
		ItStatus1 = SI4432_ReadReg(0x03);		//?????????
		ItStatus2 = SI4432_ReadReg(0x04);		//?????????
		test = 0x01;
		
}



void rx_data(void)
{	
	unsigned char i, chksum;
	Flag.is_tx = 0;
	
	SI4432_WriteReg(0x07, SI4432_PWRSTATE_READY);	//Ready Mode
	DelayMs(5);		

	TX0_RX1;		
	
	//clear the contents of the RX FIFO
  //clear the contents of the TX FIFO.
	SI4432_WriteReg(0x08, 0x03);  
	SI4432_WriteReg(0x08, 0x00);  

	//SI4432_WriteReg(0x05, SI4432_Rx_packet_received_interrupt);  // Valid Packet Received Interrupt is enabled		
	ItStatus1 = SI4432_ReadReg(0x03);		
	ItStatus2 = SI4432_ReadReg(0x04);	
		
	SI4432_WriteReg(0x07,SI4432_PWRSTATE_RX);  // RF enter receive mode
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
