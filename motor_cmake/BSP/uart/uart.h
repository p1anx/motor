//
// Created by xwj on 10/16/25.
//

#ifndef __UART_PRINTF_H
#define __UART_PRINTF_H

// #include "main.h"
// #include <stdio.h>
// #include <string.h>
//
// #define PRINT_BUFFER_SIZE 256
//
// // 函数声明
// int fputc(int ch, FILE *f);
// void uart_flush(void);

#include "main.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define REDIRECT_PRINTF_TO_DMA
// UART句柄
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
#define MY_UART_1 huart1
#define UART_ESP8266 huart3
#define UART_PRINTF huart2

typedef struct UART_t UART_t;

struct UART_t
{
    UART_HandleTypeDef *huart;
    uint8_t rxBuffer;
    char str[1024];
};

void UART_init(UART_t *uart);

// DMA发送缓冲区
#define PRINTF_BUFFER_SIZE 256
static uint8_t printf_buffer[PRINTF_BUFFER_SIZE];

// 发送状态标志
static volatile uint8_t dma_tx_busy = 0;
static volatile uint8_t dma_tx_complete = 1;

int printf_dma(const char *format, ...);
int printf_dma_timeout(uint32_t timeout, const char *format, ...);
void UART_PrintfInit(UART_t *uart);
#endif