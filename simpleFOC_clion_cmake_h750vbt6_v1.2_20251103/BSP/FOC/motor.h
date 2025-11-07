//
// Created by xwj on 11/3/25.
//

#ifndef STM32H750VBT6_CMAKE_MOTOR_H
#define STM32H750VBT6_CMAKE_MOTOR_H

#include "BLDCMotor.h"

void MotorFOC_Init(BLDCMotor_t *motor, PIDController pid, ControlType_t ctrl_type);

#endif // STM32H750VBT6_CMAKE_MOTOR_H
