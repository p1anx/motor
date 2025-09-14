#ifndef _CAR_MOTOR_PWM_H
#define _CAR_MOTOR_PWM_H

#include "mymain.h"
#include "pid_init.h"
// #include "stm32f407xx.h"

#define CLOCKWISE 1
#define NOT_CLOCKWISE 0
#define PWM_TIM_FREQUENCY 1e6

#define ENCODER_A_GPIO_PIN GPIO_PIN_7
#define ENCODER_A_GPIO_PORT GPIOC

#define ENCODER_B_GPIO_PIN GPIO_PIN_6
#define ENCODER_B_GPIO_PORT GPIOC

#define ENCODER_COUNT_PERIOD 0xffff // 65535
#define ENCODER_RESOLUTION 500
#define MOTOR_REDUCTION_RATIO 10
#define PID_UPDATE_TIME 0.005 // 单位s
#define PID_TIM_CLK_DIV 16800
#define PID_TIM_PERIOD 84e6 / PID_TIM_CLK_DIV *PID_UPDATE_TIME

// extern TIM_HandleTypeDef htim3;
// #define encoder_htim htim3

// extern TIM_HandleTypeDef htim4;
// #define pid_htim     htim4

// #define TB6612_PWM1_GPIO_PIN   GPIO_PIN_11
// #define TB6612_PWM1_GPIO_PORT  GPIOA
// extern TIM_HandleTypeDef       htim1;
// #define tb6612_pwm1_htim       htim1
// #define TB6612_PWM1_CHANNEL_x  TIM_CHANNEL_4
// #define TB6612_PWM1_PERIOD      1000

// #define TB6612_AIN1_GPIO_PIN   GPIO_PIN_4
// #define TB6612_AIN1_GPIO_PORT  GPIOB

// #define TB6612_AIN2_GPIO_PIN   GPIO_PIN_5
// #define TB6612_AIN2_GPIO_PORT  GPIOB

// #define TB6612_STBY_GPIO_PIN   GPIO_PIN_11
// #define TB6612_STBY_GPIO_PORT  GPIOB
// PID timer
extern TIM_HandleTypeDef htim4;
#define pid_htim htim4
#define PID_UPDATE_TIM TIM4

// encoder timer
extern TIM_HandleTypeDef htim8;
#define encoder_htim htim8
#define ENCODER1_TIM TIM8

#define TB6612_PWM1_GPIO_PIN GPIO_PIN_9
#define TB6612_PWM1_GPIO_PORT GPIOE
extern TIM_HandleTypeDef htim1;
#define tb6612_pwm1_htim htim1
#define TB6612_PWM1_CHANNEL_x TIM_CHANNEL_1
#define TB6612_PWM1_PERIOD 1000

#define TB6612_AIN1_GPIO_PIN GPIO_PIN_8
#define TB6612_AIN1_GPIO_PORT GPIOB

#define TB6612_AIN2_GPIO_PIN GPIO_PIN_9
#define TB6612_AIN2_GPIO_PORT GPIOB

#define TB6612_STBY_GPIO_PIN GPIO_PIN_11
#define TB6612_STBY_GPIO_PORT GPIOB

#define TB6612_ENABLE() HAL_GPIO_WritePin(TB6612_STBY_GPIO_PORT, TB6612_STBY_GPIO_PIN, SET)
#define TB6612_DISABLE() HAL_GPIO_WritePin(TB6612_STBY_GPIO_PORT, TB6612_STBY_GPIO_PIN, RESET)

// void tb6612_setPWM(float duty);
// void tb6612_setDirection(Motor_InitStruct* motor);
// void tb6612_direction(Motor_InitStruct* motor);

/*=============================motor2===================================*/
extern TIM_HandleTypeDef htim3;
#define encoder2_htim htim3 // PA6, PA7; PA6--->E2A, PA7--->E2B;
#define ENCODER2_TIM TIM3

#define TB6612_PWM2_GPIO_PIN GPIO_PIN_11
#define TB6612_PWM2_GPIO_PORT GPIOA
extern TIM_HandleTypeDef htim1;
#define tb6612_pwm2_htim htim1
#define TB6612_PWM2_CHANNEL_x TIM_CHANNEL_4
#define TB6612_PWM2_PERIOD 1000

#define TB6612_MOTOR2_AIN1_GPIO_PIN GPIO_PIN_4
#define TB6612_MOTOR2_AIN1_GPIO_PORT GPIOA

#define TB6612_MOTOR2_AIN2_GPIO_PIN GPIO_PIN_5
#define TB6612_MOTOR2_AIN2_GPIO_PORT GPIOA

#define TB6612_STBY_GPIO_PIN GPIO_PIN_11
#define TB6612_STBY_GPIO_PORT GPIOB

#define TB6612_ENABLE() HAL_GPIO_WritePin(TB6612_STBY_GPIO_PORT, TB6612_STBY_GPIO_PIN, SET)
#define TB6612_DISABLE() HAL_GPIO_WritePin(TB6612_STBY_GPIO_PORT, TB6612_STBY_GPIO_PIN, RESET)

#endif
