#ifndef __PWM_H
#define __PWM_H
#include "stm32f1xx_hal.h"
#include "tim.h"

#define ON 1
#define OFF 0

#define  TIM2_CH3_STATE 1
#define  TIM1_CH1_STATE 0


#if TIM2_CH3_STATE
#define PWM1_GPIO_PIN  GPIO_PIN_2
#define PWM1_GPIO_PORT GPIOA
extern TIM_HandleTypeDef htim2;
#define htim_pwm            htim2
#define TIM_PWM_CHANNELx    TIM_CHANNEL_3
#endif


#if TIM1_CH1_STATE
#define PWM1_GPIO_PIN  GPIO_PIN_8
#define PWM1_GPIO_PORT GPIOA
extern TIM_HandleTypeDef htim1;
#define htim_pwm            htim1
#define TIM_PWM_CHANNELx    TIM_CHANNEL_1
#endif


void pwm_led_brth(int brth_time);
// #define PWM1_TOGGLE() HAL_GPIO_TogglePin(PWM1_GPIO_PORT, PWM1_GPIO_PIN)




#endif // ! __PWM_H