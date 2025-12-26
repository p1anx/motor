//
// Created by xwj on 11/17/25.
//
#include "stm32_hal.h"
#include <string.h>
#include <stdio.h>
#include "stm32h7_dma/stm32h7_dma.h"
#include "vofa.h"


extern int TxCompleteFlag;
char buffer[3];
extern char txDMA_buffer[256];
extern uint8_t tempData[8];
float data_float[1];
uint8_t tail[4] = {0, 0, 0x80, 0x7f};
uint8_t head[4] = {0, 0};
DMA_BUFFER uint8_t Data[8] = {0, 0, 0, 0, 0, 0,0x80, 0x7F};
DMA_BUFFER float DataFloat[1];
float data16[16];
void UART_DMASend2(float dataFloat1, float dataFloat2) {
    uint8_t justFloat_tail[4] = {0, 0, 0x80, 0x7f};
    uint8_t buffer[256];
    float dataFloat_list[2] = {dataFloat1, dataFloat2};
    int dataFloat_num = 2;
    int data_n = sizeof(dataFloat1) * dataFloat_num + sizeof(justFloat_tail);
    for (int i = 0; i < dataFloat_num; i++) {
        memcpy(buffer + i*sizeof(dataFloat1), &dataFloat_list[i], sizeof(dataFloat1));
    }
    memcpy(buffer + dataFloat_num*sizeof(dataFloat1) , &justFloat_tail, sizeof(justFloat_tail));
    memcpy(txDMA_buffer, buffer, sizeof(buffer));
    HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, data_n);
}
void UART_DMASend1(float dataFloat1) {
    uint8_t justFloat_tail[4] = {0, 0, 0x80, 0x7f};
    uint8_t buffer[256];
    int data_n = sizeof(dataFloat1) + sizeof(justFloat_tail);
    memcpy(buffer, &dataFloat1, sizeof(dataFloat1));
    memcpy(buffer + sizeof(dataFloat1), &justFloat_tail, sizeof(justFloat_tail));
    memcpy(txDMA_buffer, buffer, sizeof(buffer));
    HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, data_n);
}
void test_uart_0(void) {
    printf("start uart1\n");
    HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));
    // strcpy(txDMA_buffer, "hello world, uart1\n");
    float data_float[2];
    data_float[0] = 10.0f;
    data_float[1] = 10.0f;
    int last_t = 0;
    while (1) {
        int t1 = getUs();
        if (t1 -last_t > 200) {
            if (TxCompleteFlag == 1) {
                TxCompleteFlag = 0;
                data_float[0] += 1.0f;
                if (data_float[0] > 100.0f) {
                    data_float[0] = 10.0f;
                }
                // HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));
                // HAL_UART_Transmit(&print_uart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer), 1000);
                //3.
                // UART_DMASendVOFA_justFloat1(data_float[0]);
                // UART_DMASend1(data_float[0]);
                UART_DMASend2(data_float[0], data_float[1]);
                last_t = t1;
            }
        }


    }

}
// /* 示例：直接操作寄存器发送 */
// void UART_WriteByte(UART_HandleTypeDef *huart, uint8_t data) {
//     /* 1. 等待上一个数据已从数据寄存器转移到移位寄存器（数据寄存器为空） */
//     while(__HAL_UART_GET_FLAG(huart, UART_FLAG_TXE) == RESET) {
//         // 等待，直到TXE标志置位
//     }
//
//     /* 2. 将新数据写入数据寄存器(DR)，写入操作会自动清除TXE标志 */
//     huart->Instance->TDR = (data & (uint16_t)0x00FF);
// }
/* 直接操作寄存器发送多个字节数据 */
void UART_WriteBytes(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) {
    uint16_t i;

    for (i = 0; i < Size; i++) {
        /* 1. 等待上一个数据已从数据寄存器转移到移位寄存器（数据寄存器为空） */
        while(__HAL_UART_GET_FLAG(huart, UART_FLAG_TXE) == RESET) {
            // 等待，直到TXE标志置位
        }

        /* 2. 将新数据写入数据寄存器(DR)，写入操作会自动清除TXE标志 */
        huart->Instance->TDR = (pData[i] & (uint16_t)0x00FF);
    }

    /* 3. 等待最后一个字节发送完成（TC标志：传输完成） */
    while(__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET) {
        // 等待传输完成
    }
}
void test_uart(void) {
    // vofa_test_uart();
    vofa_test_uart_dma();

}
void test_uart2(void) {
    printf("start vofa test\n");
    // HAL_UART_Transmit_DMA(&huart1, tempData, 8);
    // HAL_UART_Transmit_DMA(&huart1, Data, 8);
    // strcpy(txDMA_buffer, "hello world\n");
    txDMA_buffer[0] = 0x11;
    txDMA_buffer[1] = 0x12;
    txDMA_buffer[2] = 0x13;
    txDMA_buffer[3] = 0x14;
    txDMA_buffer[4] = 0x0;
    txDMA_buffer[5] = 0x0;
    txDMA_buffer[6] = 0x80;
    txDMA_buffer[7] = 0x7f;
    // static uint8_t vofa_justFloat0[8] = {0, 0, 0, 0, 0, 0, 0x80, 0x7F};
    // memcpy(txDMA_buffer, (char*)vofa_justFloat0, 8);
    // txDMA_buffer[5] = '\n';

    HAL_UART_Transmit_DMA(&huart1, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));
    // HAL_UART_Transmit_DMA(&huart1, (uint8_t*)txDMA_buffer, 8);
    data_float[0] = 10.0f;
    DataFloat[0] = 10.0f;
    while (1) {
        data_float[0] += 1.0f;
        DataFloat[0] += 2.0f;
        if (data_float[0] > 100.0f) {
            data_float[0] = 10.0f;
        }
        if (DataFloat[0] > 1000.0f) {
            DataFloat[0] = 10.0f;

        }
        // 1.
         // memcpy(head, (uint8_t*)&data_float, 4);
         // HAL_UART_Transmit(&print_uart, head, 4, 1000);
         // HAL_UART_Transmit(&print_uart, tail, 4, 1000);
         // HAL_Delay(1);

        //2.
        //  memcpy(txDMA_buffer, (uint8_t*)&data_float, sizeof(data_float));
        // if (TxCompleteFlag == 1) {
        //     TxCompleteFlag = 0;
        //     HAL_UART_Transmit_DMA(&huart1, (uint8_t*)txDMA_buffer, 8);
        // }
        // HAL_Delay(1);
        //3.
        // UART_DMASend1(data_float[0]);
        UART_DMASend2(data_float[0], DataFloat[0]);
        //3.not ok
        // memcpy(vofa_justFloat, (uint8_t*)&data_float, sizeof(data_float));
        // HAL_UART_Transmit(&print_uart,vofa_justFloat, 8, 1000);

        // // memcpy(txDMA_buffer, (uint8_t*)&DataFloat, sizeof(DataFloat));
        // memcpy(Data, (uint8_t*)&DataFloat, sizeof(DataFloat));
        //  // memcpy(Data, (uint8_t*)&data_float, 4);
        // if (TxCompleteFlag == 1) {
        //     TxCompleteFlag = 0;
        //     // HAL_UART_Transmit_DMA(&huart1, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));
        //     // HAL_UART_Transmit_DMA(&huart1, (uint8_t*)txDMA_buffer, 8);
        //     HAL_UART_Transmit_DMA(&print_uart, Data, 8);
        // }
        HAL_Delay(1);


    }

}
void test_uart1(void)
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
