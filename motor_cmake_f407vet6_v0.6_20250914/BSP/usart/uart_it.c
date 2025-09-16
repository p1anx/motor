#include "uart_it.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_uart.h"
#include "usart.h"
#include "usart_mx.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"

// extern char rxBuffer[RXBUFFER];
extern uint8_t rxBuffer;
// extern char *rxBuffer;

extern GlobalVar_t g_var;
char str[RXBUFFER];
// extern uint8_t str[RXBUFFER];
// char rx[RXBUFFER_SIZE];
uint8_t rx;
extern UART_HandleTypeDef huart1;
UART_t uart1 = {.huart = &huart1, .i = 0, .rxBuffer = &rxBuffer, .rxSize = RXBUFFER_SIZE};
char *getString(UART_t *uart)
{
    printf("rx =  %c\n", rx);
    uart->str[uart->i++] = *uart->rxBuffer;
    if (uart->rxBuffer == '\n')
    {
        printf("str = %s\n", uart->str);
        memset(uart->str, 0, uart->rxSize);
        uart->i = 0;
    }
    HAL_UART_Receive_IT(uart->huart, uart->rxBuffer, uart->rxSize);

    return uart->str;
}
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     if (huart->Instance == USART1)
//     {
//         getString(&uart1);
//         // printf("str = %s\n", uart1.str);
//     }
// }
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        static int i = 0;
        // printf("rx i= %c\n", rx);
        // HAL_UART_Transmit(&MY_UART_1, &rx, 1, HAL_MAX_DELAY);
        int result;
        float f_result;
        str[i++] = rx;
        if (rx == '\n')
        {
            printf("str: %s\n", str);
            // result = getInt_from_str(str, 2);
            // if (str[0] == 'p')
            // {
            //     printf("p = %d\n", result);
            // }
            // else if (str[0] == 'i')
            // {
            //     printf("i = %d\n", result);
            // }
            f_result = getFloat_from_str(str, 2);
            if (str[0] == 'p')
            {
                printf("p = %f\n", f_result);
            }
            else if (str[0] == 'i')
            {
                printf("i = %f\n", f_result);
            }
            g_var.pwm_duty = f_result;
            // printf("result = %d\n", result);
            i = 0;
            memset(str, 0, RXBUFFER);
        }
        HAL_UART_Receive_IT(&MY_UART_1, &rx, RXBUFFER_SIZE);
    }
}

// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     if (huart->Instance == USART1)
//     {
//         static int i = 0;
//         printf("uart rx\n");
//         // PID_Controller* pid_init = motor1.inc_pid_vel_init;
//         str[i++] = rxBuffer;
//         printf("rx = %c\n", rxBuffer);
//         if (rxBuffer == '\n')
//         {
//
//             printf("str = %s", str);
//             memset(str, 0x00, RXBUFFER);
//             i = 0;
//         }
//
//         HAL_UART_Receive_IT(&MY_UART_1, &rxBuffer, 1);
//     }
// }
