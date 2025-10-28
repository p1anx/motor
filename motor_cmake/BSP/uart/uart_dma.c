#include "uart_dma.h"
#include  <stdio.h>
#include  "esp8266.h"

extern ESP8266_HandleTypeDef g_esp8266;
extern UART_HandleTypeDef huart3;
// 定义全局变量和缓冲区
#define UART_RX_BUFFER_SIZE  2048
uint8_t rx_buffer[UART_RX_BUFFER_SIZE]; // 接收缓冲区

void UART_DMA_IRQHandler(UART_HandleTypeDef *huartx)
{
    if (__HAL_UART_GET_FLAG(huartx, UART_FLAG_IDLE) != RESET)
    {
        // 必须清除 IDLE 标志位
        __HAL_UART_CLEAR_IDLEFLAG(huartx);

        // 触发自定义的空闲中断处理函数
        // 这里可以直接调用一个自定义的函数，例如：
        UART_Idle_Callback(huartx);
    }

}
void ESP8266_UART_Init(void)
{
    // 1. 启用 UART 空闲中断 (IDLE Line Detection)
    __HAL_UART_ENABLE_IT(&huartx, UART_IT_IDLE);

    // 2. 启动 DMA 环形接收模式 (一次性启动，DMA会循环写入缓冲区)
    HAL_UART_DMAStop(&huartx);
    if (huartx.RxState != HAL_UART_STATE_READY) {
        HAL_UART_DMAStop(&huartx);
        // 重置状态为READY
        huartx.RxState = HAL_UART_STATE_READY;
    }
    HAL_StatusTypeDef status = HAL_UART_Receive_DMA(&huartx, rx_buffer, UART_RX_BUFFER_SIZE);
    if (status != HAL_OK)
    {
        printf("[ERROR] failed to start DMA, status = %d\n", status);
    }
}
// 在你的驱动文件中 (例如 esp8266.c)

// 上次接收到的数据长度 (初始为0)
static uint16_t old_pos = 0;

void UART_Idle_Callback(UART_HandleTypeDef *huart)
{
    if (huart == &huartx)
    {
        // 1. 停止 DMA (确保数据不再被写入)
        HAL_UART_DMAStop(huart);

        // 2. 计算本次接收到的数据长度
        // 重点：计算 DMA 计数器中还剩下多少未接收的数据
        // N = 总长度 - 剩余未填充的长度

        uint16_t pos = huart->RxXferSize - __HAL_DMA_GET_COUNTER(huart->hdmarx);
        // uint16_t pos = UART_RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);
        // printf("rx size = %d\n", huart->RxXferSize);

        uint16_t data_len = 0;

        // 由于是环形缓冲区，我们需要考虑两次 IDLE 之间的数据可能跨越了缓冲区的末尾
        if (pos >= old_pos)
        {
            // 正常情况：数据在缓冲区内连续
            data_len = pos - old_pos;
        }
        else
        {
            // 跨越缓冲区末尾的情况：数据从 old_pos 到末尾 + 从开头到 pos
            data_len = huart->RxXferSize - old_pos + pos;
        }

        // 3. 处理接收到的数据 (从 rx_buffer[old_pos] 开始，长度为 data_len)
        // printf("data len = %d\n", data_len);
        if (data_len > 0)
        {
            // 将接收到的数据复制到一个临时的处理缓冲区，
            // 或直接在这里对 rx_buffer [old_pos] 到 [old_pos + data_len - 1] 的数据进行解析
            // printf("dma: %s\n", rx_buffer[old_pos]);
            // g_esp8266.rxString = rx_buffer;
        }

        // 4. 更新下次的起始位置 (下次接收从 pos 开始)
        old_pos = pos;
        strcpy(g_esp8266.rxString, rx_buffer);

        // 5. 重新启动 DMA 环形接收
        HAL_UART_Receive_DMA(huart, rx_buffer, UART_RX_BUFFER_SIZE);
    }
}