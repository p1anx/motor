/**
 * @file uart.c
 * @brief UART 中断接收：单字符 + 按行字符串，STM32 HAL
 */
#include "uart.h"
#include <stdio.h>
#include <string.h>

/* 当前绑定的 UART 句柄（只支持一个实例时用） */
static UART_HandleTypeDef *s_huart;

/* 单字节接收缓存，用于 HAL_UART_Receive_IT 单字节循环接收 */
static uint8_t s_rx_byte;

/* 行缓冲：累积到 \r 或 \n 为一行 */
static char s_line_buf[UART_RX_LINE_MAX];
static unsigned int s_line_len;
static volatile int s_line_ready;  /* 1 = 有一行可读 */

/* 防止未实现用户回调时链接报错（可改为空实现） */
__attribute__((weak)) void UART_OnChar(uint8_t c) { (void)c; }

/**
 * @brief 初始化并启动 UART 中断接收（单字节循环）
 */
int UART_RxIT_Start(UART_HandleTypeDef *huart)
{
    if (huart == NULL) return -1;

    s_huart   = huart;
    s_line_len = 0;
    s_line_ready = 0;
    memset(s_line_buf, 0, sizeof(s_line_buf));

    /* 启动单字节接收，收完一字节会在 HAL_UART_RxCpltCallback 里再次启动 */
    return (HAL_UART_Receive_IT(s_huart, &s_rx_byte, 1) == HAL_OK) ? 0 : -1;
}

void UART_RxIT_Stop(UART_HandleTypeDef *huart)
{
    if (huart != NULL)
        HAL_UART_AbortReceive_IT(huart);
}

/**
 * @brief 取出一行到用户缓冲区，并清除行就绪标志
 */
int UART_GetLine(char *buf, unsigned int size)
{
    if (buf == NULL || size == 0 || !s_line_ready)
        return 0;

    unsigned int n = s_line_len;
    if (n >= size)
        n = size - 1;
    memcpy(buf, s_line_buf, n);
    buf[n] = '\0';

    /* 消费并清空内部缓冲，等待新数据 */
    s_line_ready = 0;
    s_line_len   = 0;
    memset(s_line_buf, 0, sizeof(s_line_buf));
    return (int)n;
}

int UART_IsLineReady(void)
{
    return s_line_ready;
}

/**
 * @brief 返回当前一行的指针（先补 \\0，不清空；用完后须调用 UART_ClearRxString）
 */
char *UART_RxString(void)
{
    s_line_buf[s_line_len] = '\0';  /* 保证字符串结尾 */
    return s_line_buf;
}

/**
 * @brief 清空接收行缓冲（在用完 UART_RxString 返回的内容之后调用）
 */
void UART_ClearRxString(void)
{
    s_line_ready = 0;
    s_line_len   = 0;
    memset(s_line_buf, 0, sizeof(s_line_buf));
}

/**
 * @brief 取出一行并判断是否等于指定命令；无完整行时直接返回 0，不比较
 */
int UART_GetCMD_Once(char cmd[])
{
    char rx_buf[UART_RX_LINE_MAX];
    int  n = UART_GetLine(rx_buf, sizeof(rx_buf));
    if (n <= 0)
        return 0;   /* 暂无完整行，rx_buf 未写入，不能做 strcmp */
    if (cmd != NULL && strcmp(rx_buf, cmd) == 0)
        return 1;
    return 0;
}

/**
 * @brief HAL 单字节接收完成回调：在这里拼成“行”或直接处理字符
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart != s_huart) return;

    uint8_t c = s_rx_byte;

    UART_OnChar(c);

    if (c == UART_LINE_END_CR || c == UART_LINE_END_LF) {
        if (s_line_len > 0) {
            s_line_ready = 1;
            /* 先取指针再打印，最后再清空；顺序不能反 */
            char *rx = UART_RxString();
            printf("rx = %s\n", rx);
            UART_ClearRxString();
        }
        HAL_UART_Receive_IT(s_huart, &s_rx_byte, 1);
        return;
    }

    if (s_line_len < UART_RX_LINE_MAX - 1) {
        s_line_buf[s_line_len++] = (char)c;
    }

    HAL_UART_Receive_IT(s_huart, &s_rx_byte, 1);
}

/**
 * @brief 错误回调：可在此重新启动接收，避免卡死
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart == s_huart)
        HAL_UART_Receive_IT(s_huart, &s_rx_byte, 1);
}
