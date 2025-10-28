#ifndef __UART_DMA_H
#define __UART_DMA_H

#include "main.h"

// UART句柄定义（根据实际使用的串口修改）
extern UART_HandleTypeDef huart3;
#define  huartx huart3

void UART_DMA_IRQHandler(UART_HandleTypeDef *huartx);
void ESP8266_UART_Init(void);
void UART_Idle_Callback(UART_HandleTypeDef *huart);

#endif
