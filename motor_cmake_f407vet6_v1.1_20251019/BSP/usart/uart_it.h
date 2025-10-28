#ifndef _UART_IT_H
#define _UART_IT_H

#include "mymain.h"
#include "stm32f4xx_hal_uart.h"

#define MY_UART_1 huart1

#define RXBUFFER_SIZE 1
#define RXBUFFER 1024
typedef struct UART_t UART_t;

struct UART_t
{
    UART_HandleTypeDef *huart;
    char str[RXBUFFER_SIZE];
    char *rxBuffer;
    int rxSize;
    int i;
};
#endif
