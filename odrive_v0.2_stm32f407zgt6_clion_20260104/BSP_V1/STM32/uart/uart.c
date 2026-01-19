//
// Created by xwj on 1/13/26.
//

#include "uart.h"
#include <stdio.h>
#include <string.h>

extern char txDMA_buffer[256];
extern int TxCompleteFlag;

extern  UART_HandleTypeDef  huart2;
#define print_uart  huart2

// 发送浮点数数组的函数
void UART_SendDataFloat(float *data, uint8_t count)
{
    char buffer[200]; // 缓冲区大小根据实际需要调整
    char temp[20];    // 临时存储单个浮点数

    // 清空缓冲区
    memset(buffer, 0, sizeof(buffer));

    // 构建格式化字符串
    for(uint8_t i = 0; i < count; i++)
    {
        sprintf(temp, "%.3f", data[i]); // 保留2位小数
        strcat(buffer, temp);

        if(i < count - 1) // 不是最后一个数据时添加逗号
        {
            strcat(buffer, ",");
        }
    }

    // 添加换行符
    strcat(buffer, "\n");

    // 发送数据
    HAL_UART_Transmit(&print_uart, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
}
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void UART_SendFloat(uint8_t count, ...)
{
    char buffer[200];
    int length = 0;
    va_list args;

    memset(buffer, 0, sizeof(buffer));

    va_start(args, count);
    for(uint8_t i = 0; i < count; ++i)
    {
        double value = va_arg(args, double);
        int written = snprintf(buffer + length, sizeof(buffer) - length, i < count - 1 ? "%.4f," : "%.4f\n", (float)value);

        if(written < 0)
        {
            break;
        }

        length += written;
        if(length >= (int)sizeof(buffer))
        {
            length = sizeof(buffer) - 1;
            break;
        }
    }
    va_end(args);

    HAL_UART_Transmit(&print_uart, (uint8_t*)buffer, length, 1000);
}
void UART_SendFloat_DMA(uint8_t count, ...)
{
    // static char txDMA_buffer[256];  // 使用静态缓冲区
    int length = 0;
    va_list args;

    memset(txDMA_buffer, 0, sizeof(txDMA_buffer));  // 清零缓冲区

    va_start(args, count);
    for(uint8_t i = 0; i < count; ++i)
    {
        double value = va_arg(args, double);

        // 检查剩余空间
        int remaining_space = sizeof(txDMA_buffer) - length - 1;  // -1 为结束符预留
        if(remaining_space <= 0) break;

        int written;
        if(i < count - 1)
        {
            // 不是最后一个值，用逗号分隔
            written = snprintf(txDMA_buffer + length, remaining_space, "%.3f,", (float)value);
        }
        else
        {
            // 最后一个值，用换行符结束
            written = snprintf(txDMA_buffer + length, remaining_space, "%.3f\n", (float)value);
        }

        if(written < 0 || written >= remaining_space)
        {
            // 格式化失败或缓冲区不够
            break;
        }

        length += written;
    }
    va_end(args);

    // 确保字符串结束
    txDMA_buffer[sizeof(txDMA_buffer)-1] = '\0';
    // strcpy(txDMA_buffer, buffer);

    if (TxCompleteFlag == 1) {
        TxCompleteFlag = 0;
        if (HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, length) != HAL_OK)
        {
            printf("UART_SendFloat_DMA: HAL_UART_Transmit_DMA failed\n");
        }
    }
}

#include "usart.h"
#include <stdio.h>
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE {
    HAL_UART_Transmit(&print_uart, (uint8_t *)&ch, 1, 0xFFFF); // 阻塞方式打印,串口1
    return ch;
}
