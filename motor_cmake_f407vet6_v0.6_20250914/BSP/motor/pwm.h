
#ifndef __PWM_H
#define __PWM_H
#include "stm32_hal.h"
#include "types.h"

extern TIM_HandleTypeDef htim1;
#define pwm_htim htim1
#define PWM_CHANNEL TIM_CHANNEL_4
#define PWM_RESOLUTION 4096 // 12 bits
#define PWM_FREQUENCY 10e3  // Hz

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
int pwm_setCompare(PWM_t *pwm, float compare);
#endif // !__MOTOR_H
