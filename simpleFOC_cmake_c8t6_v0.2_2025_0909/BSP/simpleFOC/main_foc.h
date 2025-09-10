#ifndef __MAIN_FOC_H
#define __MAIN_FOC_H
#include "stm32f1xx_hal.h"
#include "tim.h"

void _writeDutyCycle3PWM(float dc_a, float dc_b, float dc_c, int pinA, int pinB,
                         int pinC);

#endif
