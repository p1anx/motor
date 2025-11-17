#include "stm32_hal.h"
#include <string.h>
#include <stdio.h>

void delay_ms(int ms)
{
    HAL_Delay(ms);
}

uint32_t getMicros(void)
{
    // STM32实现示例
    return HAL_GetTick() * 1000 + (SysTick->LOAD - SysTick->VAL) / (SystemCoreClock / 1000000);
}
uint32_t getUs(void)
{
    // STM32实现示例
    return HAL_GetTick() * 1000 + (SysTick->LOAD - SysTick->VAL) / (SystemCoreClock / 1000000);
}
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
        int written = snprintf(buffer + length, sizeof(buffer) - length, i < count - 1 ? "%.3f," : "%.3f\n", (float)value);

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

    HAL_UART_Transmit(&print_uart, (uint8_t*)buffer, length, HAL_MAX_DELAY);
}
void UART_SendFloat_DMA(uint8_t count, ...)
{
    static char buffer[200];  // 使用静态缓冲区
    int length = 0;
    va_list args;

    memset(buffer, 0, sizeof(buffer));  // 清零缓冲区

    va_start(args, count);
    for(uint8_t i = 0; i < count; ++i)
    {
        double value = va_arg(args, double);

        // 检查剩余空间
        int remaining_space = sizeof(buffer) - length - 1;  // -1 为结束符预留
        if(remaining_space <= 0) break;

        int written;
        if(i < count - 1)
        {
            // 不是最后一个值，用逗号分隔
            written = snprintf(buffer + length, remaining_space, "%.3f,", (float)value);
        }
        else
        {
            // 最后一个值，用换行符结束
            written = snprintf(buffer + length, remaining_space, "%.3f\n", (float)value);
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
    buffer[sizeof(buffer)-1] = '\0';

    // 等待UART空闲
    while (HAL_UART_GetState(&print_uart) != HAL_UART_STATE_READY)
    {
        HAL_Delay(1);  // 避免死循环，给其他任务时间
    }

    // 发送数据
    if (HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)buffer, length) != HAL_OK)
    {
        printf("UART_SendFloat_DMA: HAL_UART_Transmit_DMA failed\n");
    }
    // else
    // {
    //     printf("UART_SendFloat_DMA: Sent %d bytes: %s", length, buffer);
    // }
}
// void UART_SendFloat_DMA(uint8_t count, ...)
// {
//     char buffer[200];
//     int length = 0;
//     va_list args;
//
//     memset(buffer, 0, sizeof(buffer));
//
//     va_start(args, count);
//     for(uint8_t i = 0; i < count; ++i)
//     {
//         double value = va_arg(args, double);
//         int written = snprintf(buffer + length, sizeof(buffer) - length, i < count - 1 ? "%.3f," : "%.3f\n", (float)value);
//
//         if(written < 0)
//         {
//             break;
//         }
//
//         length += written;
//         if(length >= (int)sizeof(buffer))
//         {
//             length = sizeof(buffer) - 1;
//             break;
//         }
//     }
//     va_end(args);
//
//     // if (HAL_UART_GetState(&print_uart) == HAL_UART_STATE_READY)
//     while (HAL_UART_GetState(&print_uart) != HAL_UART_STATE_READY);
//     {
//         if (HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)buffer, length) != HAL_OK)
//         {
//             printf("uart_send_float_DMA: HAL_UART_Transmit_DMA failed\n");
//         }
//     }
// }

// 使用示例
void Example(void)
{
    float sensor_data[4] = {3.14159f, 2.71828f, 1.41421f, 0.57721f};

    // UART_SendFloatData(&huart1, sensor_data, 4);
}