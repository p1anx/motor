#ifndef __ENCODER1_H
#define __ENCODER1_H
#include "stm32_hal.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal_tim.h"
#include "types.h"

#define ENCODER1_TIM TIM3
#define ENCODER2_TIM TIM8

#define ENCODER_RESOLUTION 500
#define MOTOR_REDUCTION_RATIO 10
struct Encoder_t
{
    int overflow_count;
    int direction;
    TIM_HandleTypeDef *htim;
    int count_period;
    int pulses;
    float velocity;
};
int encoder_init(Encoder_t *encoder);
int encoder_getOverflowCount(Encoder_t *encoder);
int encoder_getPulses(Encoder_t *encoder);
float encoder_getVelocity(Encoder_t *encoder);
#endif // !__ENCODER1_H
