#ifndef __PWM_H
#define __PWM_H
#include "tim.h"

extern TIM_HandleTypeDef htim1;
#define pwm_tim htim1

// void pwm_init(void);
void pwm_init(int frequency, int resolution);
void PWM6_Init(int frequency, int resolution); //6 pwm
#endif // !__PWM_H
