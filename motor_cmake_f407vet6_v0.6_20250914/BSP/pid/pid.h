#ifndef __PID_H
#define __PID_H

#include "stm32_hal.h"
#include "stm32f4xx_hal_tim.h"
// #include "encoder.h"
#include "types.h"

extern TIM_HandleTypeDef htim4;
#define pid_update_htim htim4
#define PID_UPDATE_TIM TIM4

#define PID_UPDATE_RESOLUTION 1e6 // 单位Hz(1e6Hz --> 1us)
#define PID_UPDATE_TIME 0.01      // 单位s
#define PID_TIM_CLK 84e6          // 单位Hz

struct PIDController_t
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
    float *pdt;
    int frequency;
    int resolution;
    TIM_HandleTypeDef *tim;
    int tim_clk;
};

void PID_Init(PIDController_t *pid);
float PID_Update(PIDController_t *pid, float setpoint, float measurement);
float Incremental_PID_Update(PIDController_t *pid, float setpoint, float measurement);
int PID_dt_linkToEncoder(PIDController_t *pid, Encoder_t *encoder);
#endif // !__PID_H
