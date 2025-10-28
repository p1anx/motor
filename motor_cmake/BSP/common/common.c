//
// Created by xwj on 10/15/25.
//

#include "common.h"
#include "stm32f4xx_hal.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024

static char buffer[MAX_BUFFER_SIZE];
static int buffer_index = 0;
void delay_ms(int ms)
{
    HAL_Delay(ms);
}
// 逐字符接收函数
char *receiveStringFromUART(char rx_char)
{
    // 检查是否是换行符
    // printf("rx char = %c\n", rx_char);
    char *result;
    if (rx_char == '\n' || rx_char == '\r')
    {
        if (buffer_index > 0)
        {                                // 确保有数据
            buffer[buffer_index] = '\0'; // 添加字符串结束符

            // 分配新内存存储完整字符串
            char *result = (char *)malloc((buffer_index + 1) * sizeof(char));
            if (result != NULL)
            {
                strcpy(result, buffer);
            }

            // 重置缓冲区
            buffer_index = 0;
            memset(buffer, 0, sizeof(buffer));

            return result; // 返回完整字符串
        }
    }
    else
    {
        // 添加字符到缓冲区（防止溢出）
        if (buffer_index < MAX_BUFFER_SIZE - 1)
        {
            buffer[buffer_index++] = rx_char;
        }
    }

    return NULL; // 还没接收到完整字符串
}

int strcpyFromUART(char *rxString, char rx_char)
{
    char *result = receiveStringFromUART(rx_char);
    if (result != NULL)
    {
        strcpy(rxString, result);
        free(result); // 记得释放内存
        return 0;
    }
    return -1;
}

// UART_HandleTypeDef huart1;
// DMA_HandleTypeDef hdma_usart1_tx;

/**
 * @brief 最灵活的UART DMA发送函数
 * @param uart: UART句柄指针
 * @param dma: DMA句柄指针
 * @param pdata: 要发送的数据指针
 * @param size: 数据大小
 * @return HAL_StatusTypeDef: HAL_OK表示成功，其他表示失败
 */
HAL_StatusTypeDef UART_DMA_Send(UART_HandleTypeDef *uart, DMA_HandleTypeDef *dma, uint8_t *pdata,
                                uint16_t size)
{
    HAL_StatusTypeDef status = HAL_OK;

    // 检查参数有效性
    if (uart == NULL || dma == NULL || pdata == NULL || size == 0)
    {
        return HAL_ERROR;
    }

    // 检查当前UART发送状态
    if (uart->gState == HAL_UART_STATE_BUSY_TX)
    {
        // 等待当前传输完成，设置超时保护（1秒）
        uint32_t timeout_start = HAL_GetTick();
        while (uart->gState == HAL_UART_STATE_BUSY_TX)
        {
            if ((HAL_GetTick() - timeout_start) > 1000)
            {
                // 超时处理
                return HAL_TIMEOUT;
            }
            HAL_Delay(1);
        }
    }
    // 检查DMA状态（使用传入的DMA句柄）
    if (dma->State == HAL_DMA_STATE_BUSY)
    {
        // 等待DMA传输完成，设置超时保护（1秒）
        uint32_t timeout_start = HAL_GetTick();
        while (dma->State == HAL_DMA_STATE_BUSY)
        {
            if ((HAL_GetTick() - timeout_start) > 1000)
            {
                // 超时处理
                return HAL_TIMEOUT;
            }
            HAL_Delay(1);
        }
    }

    // 开始新的DMA传输
    status = HAL_UART_Transmit_DMA(uart, pdata, size);

    return status;
}
