#include "stm32_hal.h"
#include <string.h>
#include <stdio.h>

extern char txDMA_buffer[256];
extern int TxCompleteFlag;
#define TIM_TIME_BASE TIM2

void delay_ms(int ms)
{
    HAL_Delay(ms);
}

uint32_t micros(void) {
    register uint32_t ms, cycle_cnt;
    do {
        ms = HAL_GetTick();
        cycle_cnt = TIM_TIME_BASE->CNT;
    } while (ms != HAL_GetTick());

    return (ms * 1000) + cycle_cnt;
}

// @brief: Busy wait delay for given amount of microseconds (us)
void delay_us(uint32_t us)
{
    uint32_t start = micros();
    while (micros() - start < (uint32_t) us) {
        __ASM("nop");
    }
}
uint32_t getMicros(void)
{
    // STM32实现示例
    return HAL_GetTick() * 1000 + (SysTick->LOAD - SysTick->VAL) / (SystemCoreClock / 1000000);
}
// uint32_t getUs(void)
// {
//     // STM32实现示例
//     return HAL_GetTick() * 1000 + (SysTick->LOAD - SysTick->VAL) / (SystemCoreClock / 1000000);
// }

void UsTimer_init(void)
{
    int arr = APB1_MAX_FREQ * 2 / 1e6 - 1;
    __HAL_TIM_SET_PRESCALER(&us_htim, arr);
    __HAL_TIM_SET_AUTORELOAD(&us_htim, 0xFFFFFFFF);
    if (HAL_TIM_Base_Start_IT(&us_htim) != HAL_OK)
    {
        printf("failed to start pid update timer\n");
    }
}
// uint32_t getUs(void)
// {
//     return HAL_GetTickUs();
// }
// uint32_t getUs(void) {
//     static uint32_t freq = 168000000;  // 你的系统频率
//     static uint32_t base_us = 0;
//     static uint32_t last_cyccnt = 0;
//
//     if (!DWT->CTRL) {
//         // DWT 未启用，启用它
//         CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
//         DWT->CYCCNT = 0;
//         DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
//         last_cyccnt = 0;
//     }
//
//     uint32_t current = DWT->CYCCNT;
//     uint32_t diff = current - last_cyccnt;
//     uint32_t us_diff = diff * 1000000UL / freq;
//     base_us += us_diff;
//     last_cyccnt = current;
//
//     return base_us;
// }
#include <stdint.h>

static uint32_t us_counter_base = 0;
static uint32_t last_cyccnt = 0;
static uint32_t cycles_per_us = 0;  // 系统时钟(MHz)

uint32_t getUs(void) {
    // 初始化 DWT
    if (!(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
        last_cyccnt = 0;
    }

    if (cycles_per_us == 0) {
        // 获取系统时钟频率，转换为每微秒的周期数
        cycles_per_us = HAL_RCC_GetSysClockFreq() / 1000000UL;
    }

    uint32_t current_cyccnt = DWT->CYCCNT;
    uint32_t diff = current_cyccnt - last_cyccnt;

    // 计算微秒差值
    uint32_t us_diff = diff / cycles_per_us;

    // 更新基准
    us_counter_base += us_diff;
    last_cyccnt = current_cyccnt;

    return us_counter_base;
}

// 重置计数器（可选）
void resetUsCounter(void) {
    us_counter_base = 0;
    last_cyccnt = DWT->CYCCNT;
}
// 2. 获取微秒级时间戳 (类似 HAL_GetTick)
uint32_t HAL_GetTickUs(void)
{
    // 直接读取寄存器比调用 HAL_TIM_ReadCapturedValue 更快
    return __HAL_TIM_GET_COUNTER(&us_htim);
}
// 需要引用 HAL 库全局变量 uwTick
// 或者使用 HAL_GetTick()

// void UART_DMASendVOFA_justFloat(float floatData) {
//     static uint8_t vofa_justFloat[8] = {0, 0, 0, 0, 0, 0, 0x80, 0x7F};
//     memcpy(vofa_justFloat, &vofa_justFloat, sizeof(vofa_justFloat));
//
// }

void UART_DMASendVOFA_justFloat4(float dataFloat1, float dataFloat2, float dataFloat3, float dataFloat4) {
    // void UART_DMASend2(float dataFloat1, float dataFloat2) {
    uint8_t justFloat_tail[4] = {0, 0, 0x80, 0x7f};
    uint8_t buffer[256];
    const int dataFloat_num = 4;
    float dataFloat_list[4] = {dataFloat1, dataFloat2, dataFloat3, dataFloat4};
    int data_n = sizeof(dataFloat1) * dataFloat_num + sizeof(justFloat_tail);
    for (int i = 0; i < dataFloat_num; i++) {
        memcpy(buffer + i*sizeof(dataFloat1), &dataFloat_list[i], sizeof(dataFloat1));
    }
    memcpy(buffer + dataFloat_num*sizeof(dataFloat1) , &justFloat_tail, sizeof(justFloat_tail));
    memcpy(txDMA_buffer, buffer, sizeof(buffer));
    if (TxCompleteFlag == 1) {
        TxCompleteFlag = 0;
        HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, data_n);
    }
}
void UART_DMASendVOFA_justFloat2(float dataFloat1, float dataFloat2) {
// void UART_DMASend2(float dataFloat1, float dataFloat2) {
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
    if (TxCompleteFlag == 1) {
        TxCompleteFlag = 0;
        HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, data_n);
    }
}
void UART_DMASendVOFA_justFloat1(float dataFloat1) {
// void UART_DMASend1(float dataFloat1) {
    uint8_t justFloat_tail[4] = {0, 0, 0x80, 0x7f};
    uint8_t buffer[256];
    int data_n = sizeof(dataFloat1) + sizeof(justFloat_tail);
    memcpy(buffer, &dataFloat1, sizeof(dataFloat1));
    memcpy(buffer + sizeof(dataFloat1), &justFloat_tail, sizeof(justFloat_tail));
    memcpy(txDMA_buffer, buffer, sizeof(buffer));
    // HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, data_n);
    if (TxCompleteFlag == 1) {
        TxCompleteFlag = 0;
        HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, data_n);
    }
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
void UART_SendFloat_DMA0(uint8_t count, ...)
{
    static char buffer[256];  // 使用静态缓冲区
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
    strcpy(txDMA_buffer, buffer);

    if (TxCompleteFlag == 1) {
        if (HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, length) != HAL_OK)
        {
            printf("UART_SendFloat_DMA: HAL_UART_Transmit_DMA failed\n");
        }
    }
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