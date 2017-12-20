#include "Si4432Config.h"

const unsigned char tx_test_data[10] = {0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x6d};
FlagType Flag;


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
		GPIO_ResetBits(GPIOA, nSEL);
		
		SPI1_ReadWriteByte(addr | 0x80);    //?????????
		SPI1_ReadWriteByte(value);
		GPIO_SetBits(GPIOA, nSEL);
}


void tx_data(void)
{
	unsigned char i;
	Flag.is_tx = 1;
	SI4432_WriteReg(0x07, SI4432_PWRSTATE_READY);	// rf Ready mode
	TX1_RX0;		//TX status
	delay_ms(5);
	
	
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
	u8 ItStatus1 = SI4432_ReadReg(0x03);		
	u8 ItStatus2 = SI4432_ReadReg(0x04);
	
	u8 test = GPIO_ReadInputDataBit(GPIOA, nIRQ);
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
//	unsigned char i, chksum;
	Flag.is_tx = 0;
	
	SI4432_WriteReg(0x07, SI4432_PWRSTATE_READY);	//Ready Mode
	delay_ms(5);

	TX0_RX1;		
	
	//clear the contents of the RX FIFO
  //clear the contents of the TX FIFO.
	SI4432_WriteReg(0x08, 0x03);  
	SI4432_WriteReg(0x08, 0x00);  

	SI4432_WriteReg(0x05, SI4432_Rx_packet_received_interrupt);  // Valid Packet Received Interrupt is enabled		
	u8 ItStatus1 = SI4432_ReadReg(0x03);		
	u8 ItStatus2 = SI4432_ReadReg(0x04);	
		
	SI4432_WriteReg(0x07,SI4432_PWRSTATE_RX);  // RF enter receive mode
}

void SI4432_init(void)
{		
	u8 ItStatus1 = SI4432_ReadReg(0x03);
	u8 ItStatus2 = SI4432_ReadReg(0x04);
	
	//unable all the interrupts that are not needed	
	SI4432_WriteReg(0x06, 0x00);
	u8 test = SI4432_ReadReg(0x06);
	
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
	
	SI4432_WriteReg(0x70, 0x24);
	SI4432_WriteReg(0x71, 0x2A);
	SI4432_WriteReg(0x72, 0x30);
	SI4432_WriteReg(0x75, 0x75);
	SI4432_WriteReg(0x76, 0xBB);
	SI4432_WriteReg(0x77, 0x80);
}


