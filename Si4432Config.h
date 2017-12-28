/**
  ******************************************************************************
  * @file		delay.h
  * @author	Yohanes Erwin Setiawan
  * @date		10 January 2016
  ******************************************************************************
  */
	
#ifndef __SI4432CONFIG_H
#define __SI4432CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif

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
extern u8 dt;
extern unsigned char tx_test_data[10];

	 
typedef struct 
{
	unsigned char reach_1s				: 1;
	unsigned char rf_reach_timeout			: 1;
	unsigned char is_tx				: 1;
}	FlagType;
extern FlagType Flag;

u8 SPI1_ReadWriteByte(u8 TxData);
u8 SI4432_ReadReg(u8 addr);
void SI4432_WriteReg(u8 addr, u8 value);
void tx_data(void);
void rx_data(void);
void SI4432_init(void);




#ifdef __cplusplus
}
#endif

#endif

/********************************* END OF FILE ********************************/
/******************************************************************************/
