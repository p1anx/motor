//
// Created by xwj on 10/15/25.
//

#ifndef MOTOR_CMAKE_COMMON_H
#define MOTOR_CMAKE_COMMON_H
#include "stm32_hal.h"
void delay_ms(int ms);
int strcpyFromUART(char *rxString, char rx_char);
HAL_StatusTypeDef UART_DMA_Send(UART_HandleTypeDef *uart, DMA_HandleTypeDef *dma, uint8_t *pdata,
                                uint16_t size);
#endif // MOTOR_CMAKE_COMMON_H