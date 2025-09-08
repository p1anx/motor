#include "pwm.h"

void pwm_led_brth(int brth_time)
{
    HAL_TIM_PWM_Start(&htim_pwm, TIM_PWM_CHANNELx);
    __HAL_TIM_SetAutoreload(&htim_pwm, brth_time);
    int pwm_val = brth_time;
    for(int pwm =0; pwm <pwm_val; pwm++)
    {
        __HAL_TIM_SetCompare(&htim_pwm, TIM_PWM_CHANNELx, pwm);
        HAL_Delay(1);
    }
    for(int pwm =pwm_val; pwm > 0; pwm--)
    {
        __HAL_TIM_SetCompare(&htim_pwm, TIM_PWM_CHANNELx, pwm);
        
        HAL_Delay(1);
    }
}