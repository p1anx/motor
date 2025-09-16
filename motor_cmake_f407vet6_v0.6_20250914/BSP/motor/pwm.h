
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
    int (*get_psc)(PWM_t *pwm);
    float duty;
    int arr;
};
int pwm_init(PWM_t *pwm);
int pwm_calculatePSC(PWM_t *pwm);
int pwm_setPWM(PWM_t *pwm);
int pwm_setDuty(PWM_t *pwm, float duty);
#endif // !__MOTOR_H
