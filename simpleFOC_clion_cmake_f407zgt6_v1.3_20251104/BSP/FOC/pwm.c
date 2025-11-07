#include "stm32_hal.h"
#include "pwm.h"
#include "tim.h"

#include <stdio.h>

void pwm_init(int frequency, int resolution)
{
    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&pwm_tim, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&pwm_tim, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&pwm_tim, TIM_CHANNEL_3);
    __HAL_TIM_SET_PRESCALER(&pwm_tim, SystemCoreClock / (frequency * resolution) - 1);
    __HAL_TIM_SET_AUTORELOAD(&pwm_tim, resolution - 1);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_1, 500);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_2, 400);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_3, 300);
}
// for 6 pwm
int PWM6_Init(int frequency, int resolution)
{
    if (HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_1) != HAL_OK)
    {
        printf("PWM6_Init: TIM_CHANNEL_1 failed\n");
        return -1;
    }
    if (HAL_TIMEx_PWMN_Start(&pwm_tim, TIM_CHANNEL_1) != HAL_OK)
    {
        printf("PWM6_Init: TIM_CHANNEL_2 failed\n");
        return -1;
    }
    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&pwm_tim, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&pwm_tim, TIM_CHANNEL_3);
    __HAL_TIM_SET_PRESCALER(&pwm_tim, SystemCoreClock / (frequency*2 * resolution) - 1);
    __HAL_TIM_SET_AUTORELOAD(&pwm_tim, resolution - 1);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_1, resolution - 00);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_2, resolution - 00);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_3, resolution - 00);

    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_4);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_4, 1);
    printf("[OK] PWM Init\n");
}
