#ifndef __PWM_H
#define __PWM_H
#include "tim.h"

extern TIM_HandleTypeDef htim1;
#define pwm_tim htim1

// void pwm_init(void);
void pwm_init(int frequency, int resolution);
int PWM6_Init(int frequency, int resolution); //6 pwm
int test_PWM(int frequency, int resolution);
int PWM3_Init(int frequency, int resolution);
int test_PWM3_Init(int frequency, int resolution);
#endif // !__PWM_H
