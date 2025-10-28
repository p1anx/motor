//
// Created by xwj on 10/16/25.
//

#include "stash.h"
// UART句柄
extern UART_HandleTypeDef huart1;

// DMA传输缓冲区
uint8_t uart_rx_buffer[256];      // 接收缓冲区
uint8_t uart_tx_buffer[256];      // 发送缓冲区

// 接收状态标志
volatile uint8_t uart_rx_complete = 0;
volatile uint16_t uart_rx_len = 0;
// DMA发送数据
HAL_StatusTypeDef UART_DMA_Send(uint8_t *data, uint16_t len)
{
    HAL_StatusTypeDef status;

    // 等待上一次发送完成
    while (HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX)
    {
        HAL_Delay(1);
    }

    // 复制数据到发送缓冲区
    memcpy(uart_tx_buffer, data, len);

    // 启动DMA发送
    status = HAL_UART_Transmit_DMA(&huart1, uart_tx_buffer, len);

    return status;
}

// 发送字符串（自动计算长度）
HAL_StatusTypeDef UART_DMA_SendString(char *str)
{
    return UART_DMA_Send((uint8_t*)str, strlen(str));
}
void test_uart_dma(void) {
    //1.
    // char str[] = "Hello World!\n";
    // while (1) {
    //     UART_DMA_SendString(str);
    //     delay_ms(1000);
    //
    // }
    //2.
    // char str[] = "uart dma: Hello World!\n";
    // float f = 1.1f;
    // while (1) {
    //     // UART_DMA_SendString(str);
    //     // printf("printf: test dma\n");
    //     // printf_dma("printf: test dma\r\n");
    //     // printf_dma("printf: %s, %f\r\n", str, f);
    //     printf_dma_timeout(100,"printf timeout\n");
    //     printf_dma_timeout(100,"printf timeout1\n");
    //     // printf("printf dma: %s, %f\n", str, f);
    //     delay_ms(1000);
    // }




}
