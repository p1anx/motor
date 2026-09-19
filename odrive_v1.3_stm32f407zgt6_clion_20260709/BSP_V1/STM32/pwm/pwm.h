//
// Created by xwj on 1/14/26.
//

#ifndef ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_PWM_H
#define ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_PWM_H


#include "stm32_hal.h"

typedef enum {
  _6PWM,
  _3PWM
}PWM_Type;

typedef struct PWM_t PWM_t;
struct PWM_t {
  TIM_HandleTypeDef* htim;
  int frequency;
  int resolution;
  int prescaler, autoreload; // prescaler, autoreload value
  PWM_Type pwm_type;

};
int PWM_Init(PWM_t* pwm);
int PWM3_Init(PWM_t* pwm);
int PWM6_Init(PWM_t* pwm);
int PWM_Example(void);

#endif // ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_PWM_H
