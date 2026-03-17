//
// Created by xwj on 3/11/26.
//

#include "my_uart.h"
/**
 * @file uart.c
 * @brief UART 中断接收：单字符 + 按行字符串，STM32 HAL
 */
#include "usart.h"

#include <stdio.h>
#include <string.h>


/* 当前绑定的 UART 句柄（只支持一个实例时用） */
static UART_HandleTypeDef *s_huart;

/* 单字节接收缓存，用于 HAL_UART_Receive_IT 单字节循环接收 */
uint8_t s_rx_byte;

/* 行缓冲：累积到 \r 或 \n 为一行 */
char s_line_buf[UART_RX_LINE_MAX];
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

    s_huart  = huart;
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

    s_line_ready = 0;
    s_line_len   = 0;
    memset(s_line_buf, 0, sizeof(s_line_buf));
    return (int)n;
}
int UART_IsLineReady(void)
{
    return s_line_ready;
}

int  UART_GetCMD_Once(char cmd[])
{
    static int already_matched = 0;
    char rx_buf[1024];
    int n = UART_GetLine(rx_buf, sizeof(rx_buf));
    // UART_ClearRxString();
    // printf("%s\n", rx_buf);
    if (n <= 0) {
        return 0;
    }
    // if (already_matched) {
    //     return 0;
    // }
    if (strcmp(rx_buf, cmd) == 0) {
        already_matched = 1;
        return 1;
    }
    return 0;
}
int  UART_GetCMD(char cmd[])
{
    char rx_buf[1024];
    UART_GetLine(rx_buf, sizeof(rx_buf));
    if (strcmp(rx_buf, cmd) == 0) {
        return 1;
    }
    return 0;
}
void UART_ClearRxString(void)
{
    strcpy(s_line_buf, "");
    s_line_len = 0;
}
char* UART_RxString(void)
{
    static char* Rx_buf;
    Rx_buf = s_line_buf;
    // UART_ClearRxString();
    return Rx_buf;
}

/**
 * @brief HAL 单字节接收完成回调：在这里拼成“行”或直接处理字符
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // if (huart != s_huart) return;
    if (huart == s_huart) {
        uint8_t c = s_rx_byte;

        /* 可选：每收到一字节都通知上层（字符模式） */
        UART_OnChar(c);

        /* 行结束符：当前行就绪，不再往缓冲区里塞 \r \n */
        if (c == UART_LINE_END_CR || c == UART_LINE_END_LF) {
            if (s_line_len > 0)
                s_line_ready = 1;
            /* 再次启动单字节接收 */
            // char* rx = UART_RxString();
            // printf("rx = %s\n", rx);

            // printf("%s\n", s_line_buf);
            // UART_ClearRxString();
            // memset(s_line_buf, 0, sizeof(s_line_buf));
            HAL_UART_Receive_IT(s_huart, &s_rx_byte, 1);
            return;
        }

        /* 未满则写入行缓冲 */
        if (s_line_len < UART_RX_LINE_MAX - 1) {
            s_line_buf[s_line_len++] = (char)c;
        }

        /* 继续接收下一字节 */
        HAL_UART_Receive_IT(s_huart, &s_rx_byte, 1);

    }

}
void uart_example0(void) {
    UART_RxIT_Start(&huart1);
    char buf[2];
    while (1) {
        UART_GetLine(buf, 2);
        if (strcmp(buf, "ab") == 0) {
            printf("rx  = %s\n", buf);
        }

    }

}
