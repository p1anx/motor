//
// Created by xwj on 11/18/25.
//

#ifndef SIMPLEFOC_CMAKE_CLION_H750VBT6_V2_4_20251117_BLDCDRIVER_3PWM_H
#define SIMPLEFOC_CMAKE_CLION_H750VBT6_V2_4_20251117_BLDCDRIVER_3PWM_H
#include "stm32_hal.h"

extern TIM_HandleTypeDef htim2;
#define _3pwm_tim htim2

int BLDCDriver_3PWM_Init(int frequency, int resolution);
#endif // SIMPLEFOC_CMAKE_CLION_H750VBT6_V2_4_20251117_BLDCDRIVER_3PWM_H
