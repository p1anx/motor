#ifndef __PWM_SERVER_H
#define __PWM_SERVER_H
#include "stm32f1xx_hal.h"
#include "tim.h"

#define PWM_SERVER_DIRECTION_GPIO_PIN  GPIO_PIN_15
#define PWM_SERVER_DIRECTION_GPIO_PORT GPIOB


#define  SERVER_TIM2_CH3_STATE 0
#define  SERVER_TIM1_CH1_STATE 1


#if SERVER_TIM2_CH3_STATE
#define PWM1_GPIO_PIN  GPIO_PIN_2
#define PWM1_GPIO_PORT GPIOA
extern TIM_HandleTypeDef htim2;
#define htim_pwm            htim2
#define TIM_PWM_CHANNELx    TIM_CHANNEL_3
#endif


#if SERVER_TIM1_CH1_STATE
#define PWM_SERVER_PLUSE_GPIO_PIN  GPIO_PIN_8
#define PWM_SERVER_PLUSE_GPIO_PORT GPIOA

extern TIM_HandleTypeDef htim1;
#define htim_pwm_server            htim1
#define TIM_PWM_SERVER_CHANNELx    TIM_CHANNEL_1
#endif

void pwm_server(int brth_time);
void pwm_server_test(int brth_time);
void pwm_setFreq(int brth_time);

#endif // ! __PWM_H