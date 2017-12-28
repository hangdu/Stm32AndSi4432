#ifndef __UARTCONFIGURATION_H
#define __UARTCONFIGURATION_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f10x.h"
#include "stm32f10x_usart.h"

void USART1_Init(void);
void UARTSend(char *pucBuffer, unsigned long ulCount);

#ifdef __cplusplus
}
#endif

#endif

/********************************* END OF FILE ********************************/
/******************************************************************************/
