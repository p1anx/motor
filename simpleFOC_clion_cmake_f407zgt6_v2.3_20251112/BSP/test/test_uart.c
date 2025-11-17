//
// Created by xwj on 11/17/25.
//
#include "stm32_hal.h"
#include <string.h>
#include <stdio.h>
void test_uart(void)
{

    // char buffer[100] = "hello dma\n";
    char buffer[100];
    float data_float = 3000.0f;
    int delta_t;
    printf("start dma\n");
    UART_HandleTypeDef test_uart = huart2;
    while (1)
    {
        int t0  = getUs();
        sprintf(buffer, "%.3f,%.3f\n", data_float, data_float);
        //=================================================================================
        //1. method 1

        if (HAL_UART_GetState(&huart2) == HAL_UART_STATE_READY)
        {
            if (HAL_UART_Transmit_DMA(&huart2, (uint8_t*)buffer, strlen(buffer)) != HAL_OK)
            {
                printf("dma error\n");
            }
        }
        while (HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY);
        int t1  = getUs();
        int t_dma = t1 - t0;

        //2. method 2
        t0 = getUs();
        sprintf(buffer, "%f\n", data_float);
        printf("%s",buffer);
        while (HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY);
        t1 = getUs();
        int t_printf = t1 - t0;

        //3. method 3
        t0 = getUs();
        sprintf(buffer, "%f\n", data_float);
        HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), 1000);
        while (HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY);
        t1  = getUs();
        int t_uart = t1 - t0;

        //4. method 4
        t0 = getUs();
        UART_SendFloat(1, data_float);
        while (HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY);
        t1  = getUs();
        int t_uart_float = t1 - t0;
        //5. method 5
        t0 = getUs();
        UART_SendFloat_DMA(1, data_float);
        // while (HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY);
        t1  = getUs();
        int t_uart_floatdma = t1 - t0;

        //=================================================================================
        while (HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY);
        {
            printf("t_dma = %d, t_printf=%d, t_uart = %d, t_uartf=%d, t_uarf_dma= %d\n", t_dma, t_printf, t_uart, t_uart_float, t_uart_floatdma);
        }
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
