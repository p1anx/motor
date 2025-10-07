#ifndef __PID_H
#define __PID_H

#include "stm32_hal.h"
typedef struct
{
    float Kp;           // 比例增益
    float Ki;           // 积分增益
    float Kd;           // 微分增益
    float integral;     // 积分项累积
    float prev_error;   // 上一次误差
    float prev_error_2; // 上上一次误差
    float output;       // PID输出
    float output_max;   // 输出限幅
    float output_min;
    float integral_max; // 输出限幅
    float integral_min;
    float target_velocity;
    float target_position;
    float target_angle;
    float dt;
} PIDController_t;

float PID_Update(PIDController_t *pid, float setpoint, float measurement);
float Incremental_PID_Update(PIDController_t *pid, float setpoint, float measurement);
#endif // !__PID_H
