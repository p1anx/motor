#ifndef __PWM_H
#define __PWM_H
#include "tim.h"

extern TIM_HandleTypeDef htim8;
#define pwm_tim htim8

// void pwm_init(void);
void pwm_init(int Hz, int resolution);
#endif // !__PWM_H
