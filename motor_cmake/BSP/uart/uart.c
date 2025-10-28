//
// Created by xwj on 10/16/25.
//

#include "uart.h"

#include "main.h"
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "usart.h"
#include "esp8266/esp8266.h"
#include "uart_dma.h"

extern uint8_t rx0;
uint8_t rxBuffer;
extern ESP8266_HandleTypeDef g_esp8266;
char str[1024];
UART_t uart_esp8266;
UART_t uart_printf;
char *sync_cmd = "SYNC_COMMAND";
int sync_flag = 0;
int RxCplt_flag = 0;

void UART_init(UART_t *uart)
{
    HAL_UART_Receive_IT(uart->huart, &uart->rxBuffer, 1);
}
void UART_PrintfInit(UART_t *uart)
{
    HAL_StatusTypeDef status;
    uart->huart = &UART_PRINTF;
    status = HAL_UART_Receive_IT(uart->huart, &uart->rxBuffer, 1);
    if (status != HAL_OK)
    {
        printf("[ERROR] Failed to start UART Receive IT\n");
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // if (huart == uart_printf.huart)
    if (huart->Instance == USART2)
    {
        // printf("%c", uart_printf.rxBuffer);
        if (strcpyFromUART(uart_printf.str, uart_printf.rxBuffer) == 0)
        {
            // printf("%s\n", uart_printf.str);
            if (strcmp(uart_printf.str, sync_cmd) == 0)
            {
                sync_flag = 1;
            }
        }
        HAL_UART_Receive_IT(uart_printf.huart, &uart_printf.rxBuffer, 1);
    }
}

// 2.
// void UART_PrintfInit(UART_t *uart)
// {
//     HAL_StatusTypeDef status;
//     status = HAL_UART_Receive_IT(&huart2, &rxBuffer, 1);
//     if (status != HAL_OK)
//     {
//         printf("[ERROR] Failed to start UART Receive IT\n");
//     }
//     else
//     {
//         printf("[OK] UART IT\n");
//     }
// }
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     if (huart == &huart2)
//     {
//         // printf("uart it ok, rx = %c\n", rxBuffer);
//         // printf("%c", rxBuffer);
//         if (strcpyFromUART(str, rxBuffer) == 0)
//         {
//             // RxCplt_flag = 1;
//             printf("[FROM UART] %s\n", str);
//         }
//         HAL_UART_Receive_IT(&huart2, &rxBuffer, 1);
//     }
// }
