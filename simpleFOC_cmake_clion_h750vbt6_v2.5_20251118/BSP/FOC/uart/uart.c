//
// Created by xwj on 11/22/25.
//

#include "uart.h"

int TxCompleteFlag = 0;

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART1)
  {
    // 设置标志位，通知主循环发送已完成
    TxCompleteFlag = 1;
  }
}
