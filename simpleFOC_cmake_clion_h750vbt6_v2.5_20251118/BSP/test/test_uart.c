//
// Created by xwj on 11/17/25.
//
#include "stm32_hal.h"
#include <string.h>
#include <stdio.h>
extern int TxCompleteFlag;
char buffer[3];
extern char txDMA_buffer[256];

void test_uart(void)
{
    float data_float = 3000.0f;
    int delta_t;
    strcpy(txDMA_buffer, "hello world\n");

    printf("start dma\n");
    printf("start dma2\n");
    int t0, t1;

    HAL_UART_Transmit_DMA(&huart1, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));
    int last_t = getUs();
    int now_t;
    while (1)
    {
        //=================================================================================
        //1. method 1

        now_t = getUs();
        if (now_t - last_t > 1000) {
            if (TxCompleteFlag){
                TxCompleteFlag = 0;
                if (HAL_UART_Transmit_DMA(&huart1, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer)) != HAL_OK)
                {
                    printf("dma error\n");
                }
            }
            last_t = now_t;
        }

        //2. method2
        // now_t = getUs();
        // if (now_t - last_t > 100) {
        //     if (HAL_UART_GetState(&huart1) == HAL_UART_STATE_READY) {
        //         if (HAL_UART_Transmit_DMA(&huart1, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer)) != HAL_OK)
        //         {
        //             printf("dma error\n");
        //         }
        //     }
        //     last_t = now_t;
        // }


        // int t_dma = 10;
        // sprintf(buffer, "t_dma = %d\n", t_dma);
        // printf("buffer = %s\n", buffer);
        // HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer),1000);

        //2. method 2
        // t0 = getUs();
        // sprintf(buffer, "%f\n", data_float);
        // printf("%s",buffer);
        // while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);
        // t1 = getUs();
        // int t_printf = t1 - t0;
        //
        // printf("printf_t = %d\n", t_printf);

        // //3. method 3
        // t0 = getUs();
        // sprintf(buffer, "%f\n", data_float);
        // HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), 1000);
        // while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);
        // t1  = getUs();
        // int t_uart = t1 - t0;
        //
        // //4. method 4
        // t0 = getUs();
        // UART_SendFloat(1, data_float);
        // while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);
        // t1  = getUs();
        // int t_uart_float = t1 - t0;
        // //5. method 5
        // t0 = getUs();
        // UART_SendFloat_DMA(1, data_float);
        // // while (HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY);
        // t1  = getUs();
        // int t_uart_floatdma = t1 - t0;
        //
        // //=================================================================================
        // while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);
        // {
        //     printf("t_dma = %d, t_printf=%d, t_uart = %d, t_uartf=%d, t_uarf_dma= %d\n", t_dma, t_printf, t_uart, t_uart_float, t_uart_floatdma);
        // }
        // HAL_Delay(1000);


        // if (HAL_UART_GetState(&huart2) == HAL_UART_STATE_READY)
        // {
        //     if (HAL_UART_Transmit_DMA(&huart2, (uint8_t*)&delta_t, 4) != HAL_OK)
        //     {
        //         printf("dma error\n");
        //     }
        // }

        // HAL_Delay(1000);
    }
}
void test_uart3(void)
{

    float data_float = 3000.0f;
    int delta_t;
    strcpy(buffer, "hello world\n");

    // HAL_UART_Transmit_DMA(&huart1, (uint8_t*)buffer, strlen(buffer));
    printf("start uart3 1\n");
    printf("start uart3 2\n");
    // HAL_UART_Transmit_IT(&huart1, );
    delay_ms(1000);
    int t0, t1;

    HAL_UART_Transmit_DMA(&huart3, (uint8_t*)buffer, strlen(buffer));
    while (1)
    {
        //=================================================================================
        //1. method 1
        // HAL_UART_Transmit(&huart3, (uint8_t*)"hello\n", strlen("hello\n"), 1000);
        if (HAL_UART_GetState(&huart3) == HAL_UART_STATE_READY) {
            HAL_UART_Transmit_DMA(&huart3, (uint8_t*)"hello\n", strlen("hello\n"));
        }
        //
        // if (TxCompleteFlag){
        //     TxCompleteFlag = 0;
        //     if (HAL_UART_Transmit_DMA(&huart1, (uint8_t*)buffer, strlen(buffer)) != HAL_OK)
        //     {
        //         printf("dma error\n");
        //     }
        // }

        int t_dma = 10;
        sprintf(buffer, "t_dma = %d\n", t_dma);
        printf("buffer = %s\n", buffer);
        HAL_UART_Transmit(&huart3, (uint8_t*)buffer, strlen(buffer),1000);

        //2. method 2
        // t0 = getUs();
        // sprintf(buffer, "%f\n", data_float);
        // printf("%s",buffer);
        // while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);
        // t1 = getUs();
        // int t_printf = t1 - t0;
        //
        // printf("printf_t = %d\n", t_printf);

        // //3. method 3
        // t0 = getUs();
        // sprintf(buffer, "%f\n", data_float);
        // HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), 1000);
        // while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);
        // t1  = getUs();
        // int t_uart = t1 - t0;
        //
        // //4. method 4
        // t0 = getUs();
        // UART_SendFloat(1, data_float);
        // while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);
        // t1  = getUs();
        // int t_uart_float = t1 - t0;
        // //5. method 5
        // t0 = getUs();
        // UART_SendFloat_DMA(1, data_float);
        // // while (HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY);
        // t1  = getUs();
        // int t_uart_floatdma = t1 - t0;
        //
        // //=================================================================================
        // while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);
        // {
        //     printf("t_dma = %d, t_printf=%d, t_uart = %d, t_uartf=%d, t_uarf_dma= %d\n", t_dma, t_printf, t_uart, t_uart_float, t_uart_floatdma);
        // }
        // HAL_Delay(1000);


        // if (HAL_UART_GetState(&huart2) == HAL_UART_STATE_READY)
        // {
        //     if (HAL_UART_Transmit_DMA(&huart2, (uint8_t*)&delta_t, 4) != HAL_OK)
        //     {
        //         printf("dma error\n");
        //     }
        // }

        HAL_Delay(1000);
    }
}
