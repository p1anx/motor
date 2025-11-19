//
// Created by xwj on 11/18/25.
//

#include "BLDCDriver_3PWM.h"
#include <stdio.h>

int BLDCDriver_3PWM_Init(int frequency, int resolution)
{
  // if (HAL_TIM_PWM_Start(&_3pwm_tim, TIM_CHANNEL_1) != HAL_OK)
  // {
  //   printf("PWM6_Init: TIM_CHANNEL_1 failed\n");
  //   return -1;
  // }
  // HAL_TIM_PWM_Start(&_3pwm_tim, TIM_CHANNEL_2);
  // HAL_TIM_PWM_Start(&_3pwm_tim, TIM_CHANNEL_3);
  // __HAL_TIM_SET_PRESCALER(&_3pwm_tim, SystemCoreClock / (frequency*2 * resolution) - 1);
  // __HAL_TIM_SET_AUTORELOAD(&_3pwm_tim, resolution - 1);
  // __HAL_TIM_SET_COMPARE(&_3pwm_tim, TIM_CHANNEL_1, resolution * 0.2);
  // __HAL_TIM_SET_COMPARE(&_3pwm_tim, TIM_CHANNEL_2, resolution * 0.4);
  // __HAL_TIM_SET_COMPARE(&_3pwm_tim, TIM_CHANNEL_3, resolution * 0.6);
  //
  // // HAL_TIM_PWM_Start(&_3pwm_tim, TIM_CHANNEL_4);
  // // __HAL_TIM_SET_COMPARE(&_3pwm_tim, TIM_CHANNEL_4, 1);
  // printf("[OK] 3PWM Init\n");
}
