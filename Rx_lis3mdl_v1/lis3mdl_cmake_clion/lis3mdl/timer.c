//
// Created by xwj on 5/11/26.
//

#include "timer.h"

#include "tim.h"
#include <stdio.h>
// #include "my_uart.h"
// #include "usart.h"
// #include "stm32_hal.h"

extern UART_HandleTypeDef huart1;

int flag_send = 0;
int counter = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* 判断中断是由哪个定时器产生的 */
  if (htim->Instance == TIM1)
  {
    // printf("hello timer1\n");
    counter++;
    if (counter == 2) {
      counter = 0;
      flag_send = 1;

    }
    // HAL_UART_Transmit(&huart1, (uint8_t*)"hello\n", 10, 1000);
    // HAL_TIM_Base_Start_IT(&HTIM_SAMPLE);
    /* 在此处添加您的中断处理代码，例如翻转LED */
    // HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  }
}
