//
// Created by xwj on 11/3/25.
//

#ifndef STM32H750VBT6_CMAKE_MOTOR_H
#define STM32H750VBT6_CMAKE_MOTOR_H

#include "BLDCMotor.h"

void MotorFOC_Init(BLDCMotor_t *motor, PIDController pid, ControlType_t ctrl_type);
void MotorFOC_Init_NotCurrent(BLDCMotor_t *motor, PIDController pid_param, ControlType_t ctrl_type);
void MotorFOC_InitADC(BLDCMotor_t *motor, PIDController pid_param, ControlType_t ctrl_type);
void MotorFOC_InitCurrentLoopPID(BLDCMotor_t *motor, PIDController pid_i_d,PIDController pid_i_q, ControlType_t ctrl_type);

#endif // STM32H750VBT6_CMAKE_MOTOR_H
