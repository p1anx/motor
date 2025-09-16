
#include "motor.h"
#include "encoder.h"
#include "pid.h"
#include "pwm.h"
#include "stdio.h"
#include "stm32_hal.h"
#include "sys/types.h"
#include "tb6612_init.h"
#include "types.h"

int motor_init(Motor_t *motor)
{
    tb6612_init(motor->tb6612);
    return 0;
}

int motor_linkTB6612(Motor_t *motor, TB6612_t *tb6612)
{
    motor->tb6612 = tb6612;
    return 0;
}
float motor_getVelocity(Motor_t *motor)
{
    return encoder_getVelocity(motor->tb6612->encoder);
}
float _constrain(float value, float min, float max)
{
    value = value > max ? max : (value < min ? min : value);
    return value;
}
void motor_pid_velocity(Motor_t *motor, float target)
{
    float measued_target = motor_getVelocity(motor);
    motor->velocity = measued_target;
    motor->target_vel = target;
    PID_Update(motor->pid_velocity, target, measued_target);
    float output = motor->pid_velocity->output;
    output = _constrain(output, 0, 0.5);
    pwm_setDuty(motor->tb6612->pwm, output);
    DEBUG_PRINT("vel=%f\n", motor->velocity);
}
void motor_pid(Motor_t *motor)
{
    motor_pid_velocity(motor, 1);
}
