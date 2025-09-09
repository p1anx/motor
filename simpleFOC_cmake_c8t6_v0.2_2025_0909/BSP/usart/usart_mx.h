
#ifndef __USART_MX_H
#define __USART_MX_H
#include <stdio.h>
#include "mymain.h"


extern UART_HandleTypeDef huart1;

#define mx_huart1 huart1
void UART_SendStr(uint8_t *str);
#endif