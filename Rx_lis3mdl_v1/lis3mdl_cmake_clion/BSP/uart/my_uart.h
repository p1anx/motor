//
// Created by xwj on 3/11/26.
//

/**
 * @file uart.h
 * @brief UART interrupt receive (char/string) - STM32 HAL
 */
#ifndef __UART_H
#define __UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"   /* 确保有 UART_HandleTypeDef 等 */

// #define s_huart (&huart1)
  /* 接收行缓冲长度（按需修改） */
#define UART_RX_LINE_MAX  256

  /* 行结束符：收到其一即认为一行结束 */
#define UART_LINE_END_CR  0x0D  /* '\r' */
#define UART_LINE_END_LF  0x0A  /* '\n' */

  /**
   * @brief 初始化 UART 并启动中断接收（单字节循环接收）
   * @param huart  HAL UART 句柄（如 &huart1）
   * @return 0 成功，非 0 失败
   */
  int UART_RxIT_Start(UART_HandleTypeDef *huart);

  /**
   * @brief 停止 UART 中断接收（可选，用于关闭或切换模式）
   */
  void UART_RxIT_Stop(UART_HandleTypeDef *huart);

  /**
   * @brief 获取当前已接收的一行字符串（以 \\0 结尾），并清除“行就绪”状态
   * @param buf    用户缓冲区
   * @param size   buf 大小
   * @return 拷贝的字节数（不含 \\0），0 表示暂无完整行
   */
  int UART_GetLine(char *buf, unsigned int size);

  /**
   * @brief 是否有完整的一行可读（收到 \\r 或 \\n 后为 1，GetLine 后清零）
   */
  int UART_IsLineReady(void);

  /**
   * @brief 单字符接收回调（可选）：每收到一字节会调用一次
   *        在 user 代码里实现，用于只关心“字符”的场景
   */
  void UART_OnChar(uint8_t c);

  void UART_ClearRxString(void);
  void uart_example0(void);
  int  UART_GetCMD(char cmd[]);
  int  UART_GetCMD_Once(char cmd[]);

#ifdef __cplusplus
}
#endif

#endif /* __UART_H */
