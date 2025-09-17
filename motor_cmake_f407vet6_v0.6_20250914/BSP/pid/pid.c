#include "pid.h"
#include "stm32_hal.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_tim.h"
#include "system_stm32f4xx.h"
#include "tb6612_init.h"
#include "types.h"
#include <stdio.h>
void PID_Init(PIDController_t *pid)
{
    if (HAL_TIM_Base_Start_IT(&pid_update_htim) != HAL_OK)
    {
        printf("[error] pid timer init\n");
    }
    else
    {
        printf("[ok] pid timer init\n");
    }
    pid->Kp = 0.01;
    pid->Ki = 0;
    pid->Kd = 0;
    pid->integral_max = 10;
    pid->integral_min = -10;
    pid->output_max = 1;
    pid->output_min = -1;
    pid->prev_error = 0;
    pid->prev_error_2 = 0;
    pid->dt = PID_UPDATE_TIME;
    pid->resolution = PID_UPDATE_RESOLUTION;
    pid->frequency = pid->resolution; // arr should be less than 65535, due to 16 bits
    pid->tim = &pid_update_htim;
    pid->tim_clk = PID_TIM_CLK; // 84MHz

    printf("[ok] pid paramter init\n");
    // __HAL_TIM_SET_PRESCALER(pid->tim, SystemCoreClock / pid->frequency - 1);
    __HAL_TIM_SET_PRESCALER(pid->tim, pid->tim_clk / pid->frequency - 1);
    __HAL_TIM_SetAutoreload(pid->tim, pid->dt * pid->frequency - 1);
    // __HAL_TIM_SET_PRESCALER(pid->tim, 84 - 1);
    // __HAL_TIM_SetAutoreload(pid->tim, PID_UPDATE_TIME * pid->frequency - 1);
}

int PID_dt_linkToEncoder(PIDController_t *pid, Encoder_t *encoder)
{
    printf("pid dt = %f\n", *pid->pdt);
    encoder->pdt = pid->pdt;
    printf("encoder dt = %f\n", *encoder->pdt);

    return 0;
}
// void PID_UpdateTime_Init(PIDController_t *pid)
// {
// }

// float PID_Update(PIDController_t *pid, float setpoint, float measurement)
// {
//     float error = setpoint - measurement;
//
//     // 比例项
//     float proportional = pid->Kp * error;
//
//     // 积分项(带抗饱和)
//     pid->integral += error;
//     if (pid->integral > pid->integral_max)
//         pid->integral = pid->integral_max;
//     else if (pid->integral < pid->integral_min)
//         pid->integral = pid->integral_min;
//     float integral = pid->Ki * pid->integral;
//
//     // 微分项
//     float derivative = pid->Kd * (error - pid->prev_error);
//     pid->prev_error = error;
//
//     // 计算总输出
//     pid->output = proportional + integral + derivative;
//
//     // 输出限幅
//     if (pid->output > pid->output_max)
//         pid->output = pid->output_max;
//     else if (pid->output < pid->output_min)
//         pid->output = pid->output_min;
//
//     // static int time_count = 0;
//     // if (time_count == 100)
//     // {
//     //     time_count = 0;
//     //     DEBUG_PRINT("%.2f, %.2f\n", setpoint, measurement);
//     //     printf("%.2f, %.2f\n", setpoint, measurement);
//     // }
//     // time_count++;
//
//     return pid->output;
// }

float PID_Update(PIDController_t *pid, float setpoint, float measurement)
{
    float error = setpoint - measurement;

    // 比例项
    float proportional = pid->Kp * error;

    // 积分项(带抗饱和)
    pid->integral += error * pid->dt;
    if (pid->integral > pid->integral_max)
        pid->integral = pid->integral_max;
    else if (pid->integral < pid->integral_min)
        pid->integral = pid->integral_min;
    float integral = pid->Ki * pid->integral;

    // 微分项
    float derivative = pid->Kd * (error - pid->prev_error) / pid->dt;
    pid->prev_error = error;

    // 计算总输出
    pid->output = proportional + integral + derivative;

    // 输出限幅
    if (pid->output > pid->output_max)
        pid->output = pid->output_max;
    else if (pid->output < pid->output_min)
        pid->output = pid->output_min;

    // static int time_count = 0;
    // if (time_count == 100)
    // {
    //     time_count = 0;
    //     DEBUG_PRINT("%.2f, %.2f\n", setpoint, measurement);
    //     printf("%.2f, %.2f\n", setpoint, measurement);
    // }
    // time_count++;

    return pid->output;
}
float Incremental_PID_Update(PIDController_t *pid, float setpoint, float measurement)
{
    float error = setpoint - measurement;

    // 比例项
    float proportional = pid->Kp * (error - pid->prev_error);

    // 积分项(带抗饱和)
    // pid->integral += error;
    // if(pid->integral > pid->integral_max)
    // pid->integral = pid->integral_max; else
    // if(pid->integral < pid->integral_min)
    // pid->integral = pid->integral_min;
    float integral = pid->Ki * error;

    // 微分项
    float derivative = pid->Kd * (error - 2 * pid->prev_error + pid->prev_error_2);

    // 计算总输出
    pid->output += (proportional + integral + derivative);

    // update error
    pid->prev_error_2 = pid->prev_error;
    pid->prev_error = error;

    // 输出限幅
    if (pid->output > pid->output_max)
        pid->output = pid->output_max;
    else if (pid->output < pid->output_min)
        pid->output = pid->output_min;

#if DEBUG_PID_INC * 1

    static int tim_count = 0;
    if (tim_count == 1)
    {
        tim_count = 0;
        printf("%.2f, %.2f\n", setpoint, measurement);
        printf("pid error = %.2f, previous error "
               "= %.2f, pid output = %.2f\n",
               pid->prev_error, pid->prev_error_2, pid->output);
        printf("p = %.2f, i = %.2f, d = %.2f\n", pid->Kp, pid->Ki, pid->Kd);
        printf("pro = %.2f, integral = %.2f, "
               "derivative = %.2f\n",
               proportional, integral, derivative);
        printf("\n");
    }
    tim_count++;

#endif

    return pid->output;
}
