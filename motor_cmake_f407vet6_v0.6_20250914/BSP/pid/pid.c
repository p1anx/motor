#include "pid.h"
float PID_Update(PIDController_t *pid, float setpoint, float measurement)
{
    float error = setpoint - measurement;

    // 比例项
    float proportional = pid->Kp * error;

    // 积分项(带抗饱和)
    pid->integral += error;
    if (pid->integral > pid->integral_max)
        pid->integral = pid->integral_max;
    else if (pid->integral < pid->integral_min)
        pid->integral = pid->integral_min;
    float integral = pid->Ki * pid->integral;

    // 微分项
    float derivative = pid->Kd * (error - pid->prev_error);
    pid->prev_error = error;

    // 计算总输出
    pid->output = proportional + integral + derivative;

    // 输出限幅
    if (pid->output > pid->output_max)
        pid->output = pid->output_max;
    else if (pid->output < pid->output_min)
        pid->output = pid->output_min;

    static int time_count = 0;
    if (time_count == 100)
    {
        time_count = 0;
        printf("%.2f, %.2f\n", setpoint, measurement);
    }
    time_count++;

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
