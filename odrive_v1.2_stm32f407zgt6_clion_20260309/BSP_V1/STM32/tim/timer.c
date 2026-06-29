//
// Created by xwj on 4/9/26.
//

#include "timer.h"
/* 用户代码区域 */
#include "stm32_hal.h"

#include <stdio.h>

//int qam_type = 64;

//int qam_time[16] = {0,0,0,125, 0, 250,0, 375, 0, 500,0, 625, 0, 750, 0, 875};
int flag_phase_64qam[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

int qam_type = 16;
//4个相位
//int qam_time[8] = {0, 125, 0, 51, 0, 198,0, 125};
//int flag_phase_16qam[8] = {1, 2, 3, 4, 5, 6, 7, 8};

int qam_time[32] = {0, 125,0, 51,0, 198,0, 125,0, 301,0, 375,0, 448,0, 375,0, 551,0, 625,0, 698,0, 625,0, 801,0, 875,0, 948,0, 875};
int flag_phase_16qam[32] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};

int tim_count = 0;
int t_period = 2000;
int qam_index = 0;
int flag_phase[8] = {1, 2, 3, 4, 5, 6, 7, 8};

int qam_phase0 = -1;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* 判断中断是由哪个定时器产生的 */
  if (htim->Instance == TIM3)
  {
      if (tim_count % t_period == qam_time[qam_index]) {
        // printf("hello, tim3, %d\n", qam_time[qam_index]);
        if (qam_type == 16) {
          qam_phase0 = flag_phase_16qam[qam_index];
          qam_index++;
          if (qam_index == 32) {
            qam_index = 0;
          }

        }
        else if (qam_type == 64) {
          qam_phase0 = flag_phase_64qam[qam_index];
          qam_index++;
          if (qam_index == 16) {
            qam_index = 0;
          }

        }
        else {
          qam_phase0 = flag_phase[qam_index];
          qam_index++;
          if (qam_index == Q_SIZE) {
            qam_index = 0;
          }

        }

      }


    tim_count++;
    if (tim_count == t_period) {
      tim_count = 0;
    }
    /* 在此处添加您的中断处理代码，例如翻转LED */
    // HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  }
}