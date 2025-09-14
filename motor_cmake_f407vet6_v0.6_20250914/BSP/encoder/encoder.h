#ifndef __ENCODER1_H
#define __ENCODER1_H
#include "stm32_hal.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal_tim.h"
#include "types.h"
struct Encoder_t
{
    int overflow_count;
    int direction;
    TIM_HandleTypeDef *htim;
    int count_period;
};
int encoder_init(Encoder_t *encoder);
#endif // !__ENCODER1_H
