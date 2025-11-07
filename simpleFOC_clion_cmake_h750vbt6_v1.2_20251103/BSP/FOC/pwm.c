#include "stm32_hal.h"
#include "pwm.h"
#include "tim.h"


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
void PWM6_Init(int frequency, int resolution)
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
