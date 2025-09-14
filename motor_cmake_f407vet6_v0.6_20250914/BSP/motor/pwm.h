
#ifndef __PWM_H
#define __PWM_H
#include "stm32_hal.h"
#include "types.h"
struct PWM_t
{
    TIM_HandleTypeDef *tim;
    int tim_psc;
    uint16_t channel;

    int resolution;
    int frequency;
    float duty;
};
int pwm_init(PWM_t *pwm);
#endif // !__MOTOR_H
