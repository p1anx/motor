//
// Created by xwj on 10/22/25.
//

#ifndef MOTOR_CMAKE_UART_POLLING_H
#define MOTOR_CMAKE_UART_POLLING_H
#include "stm32_hal.h"

void UART_Polling_Receive(void);
void UART_Polling_Receive_Update(UART_HandleTypeDef *huart, char *rxData, int timeout);
// uint8_t UART_Polling_Receive_Data(UART_HandleTypeDef *huart, char *expected, int timeout);
#endif // MOTOR_CMAKE_UART_POLLING_H
