#include "pwm_server.h"

void pwm_setFreq(int brth_time){
    HAL_GPIO_WritePin(PWM_SERVER_DIRECTION_GPIO_PORT, PWM_SERVER_DIRECTION_GPIO_PIN, 1);

    int pwm = 0.5*brth_time;
    HAL_TIM_PWM_Start(&htim_pwm_server, TIM_PWM_SERVER_CHANNELx);
    __HAL_TIM_SetAutoreload(&htim_pwm_server, brth_time);
    __HAL_TIM_SetCompare(&htim_pwm_server, TIM_PWM_SERVER_CHANNELx, pwm);

}
void pwm_server(int brth_time){
    int pwm = 0.5*brth_time;
    HAL_TIM_PWM_Start(&htim_pwm_server, TIM_PWM_SERVER_CHANNELx);
    __HAL_TIM_SetAutoreload(&htim_pwm_server, brth_time);
    __HAL_TIM_SetCompare(&htim_pwm_server, TIM_PWM_SERVER_CHANNELx, pwm);

}
void pwm_server_test(int brth_time)
{
    int pwm = 400;
    HAL_TIM_PWM_Start(&htim_pwm_server, TIM_PWM_SERVER_CHANNELx);
    __HAL_TIM_SetAutoreload(&htim_pwm_server, brth_time);
    __HAL_TIM_SetCompare(&htim_pwm_server, TIM_PWM_SERVER_CHANNELx, pwm);
    
    
}