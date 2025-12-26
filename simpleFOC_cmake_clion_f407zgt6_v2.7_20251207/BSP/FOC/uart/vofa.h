//
// Created by xwj on 12/4/25.
//

#ifndef SIMPLEFOC_CMAKE_CLION_H750VBT6_V2_6_20251127_VOFA_H
#define SIMPLEFOC_CMAKE_CLION_H750VBT6_V2_6_20251127_VOFA_H

#include "stm32_hal.h"
#include "motor_config.h"

extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart1;

#define VOFA_UART USART3
#define vofa_huart CONFIG_VOFA_UART

void UART_SendJustFloat(float *pData, const int n);
void UART_SendJustFloat_DMA(float *pData, const int n);
void VOFA_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void VOFA_SendJustFloat(float *pData, const int n);
void VOFA_SendJustFloat_DMA(float *pData, const int n);
void VOFA_SendjustFloat4_DMA(float dataFloat1, float dataFloat2, float dataFloat3, float dataFloat4);

void vofa_test_uart(void);
void vofa_test_uart_dma(void);
#endif // SIMPLEFOC_CMAKE_CLION_H750VBT6_V2_6_20251127_VOFA_H
