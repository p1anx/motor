
#include "motor.h"
#include "encoder.h"
#include "pid.h"
#include "pwm.h"
#include "stdio.h"
#include "stm32_hal.h"
#include "sys/types.h"
#include "tb6612_init.h"
#include "types.h"

float _constrain(float value, float min, float max)
{
    value = value > max ? max : (value < min ? min : value);
    return value;
}

int motor_init(Motor_t *motor)
{
    tb6612_init(motor->tb6612);
    PID_Init(motor->pid);
    // PID_dt_linkToEncoder(motor->pid, motor->tb6612->encoder);
    motor->tb6612->encoder->pid = motor->pid;
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

float motor_getPosition(Motor_t *motor)
{
    return encoder_getPosition(motor->tb6612->encoder);
}
float motor_getAngle(Motor_t *motor)
{
    return Encoder_GetAngle(motor->tb6612->encoder);
}

void motor_setDirection(Motor_t *motor, int direction)
{
    tb6612_setDirection(motor->tb6612, direction);
}

int motor_enable(Motor_t *motor)
{
    return tb6612_enable(motor->tb6612);
}
int motor_disable(Motor_t *motor)
{
    return tb6612_disable(motor->tb6612);
}
void motor_pid_velocity(Motor_t *motor, float target)
{
    int direction;
    float measued_target = motor_getVelocity(motor);
    motor->velocity = measued_target;
    motor->target_vel = target;
    PID_Update(motor->pid_velocity, target, measued_target);
    float output = motor->pid_velocity->output;
    if (output < 0)
    {
        output = -output;
        motor_setDirection(motor, 0);
    }
    else
    {
        motor_setDirection(motor, 1);
    }

    output = _constrain(output, 0, 1);
    pwm_setDuty(motor->tb6612->pwm, output);
    DEBUG_PRINT("vel=%f\n", motor->velocity);
}

void motor_pid_velocityLoop(Motor_t *motor, float target)
{
    int direction;
    float measued_target = motor_getVelocity(motor);
    motor->velocity = measued_target;
    motor->target_vel = target;
    PID_Update(motor->pid, target, measued_target);
    float output = motor->pid->output;
    if (output < 0)
    {
        output = -output;
        motor_setDirection(motor, 0);
    }
    else
    {
        motor_setDirection(motor, 1);
    }

    output = _constrain(output, 0, 1);
    pwm_setDuty(motor->tb6612->pwm, output);
    DEBUG_PRINT("vel=%f\n", motor->velocity);
}
void motor_pid_velocity_compare(Motor_t *motor, float target)
{
    int direction;
    float measued_target = motor_getVelocity(motor);
    motor->velocity = measued_target;
    motor->target_vel = target;
    PID_Update(motor->pid_velocity, target, measued_target);
    float output = motor->pid_velocity->output;
    if (output < 0)
    {
        output = -output;
        motor_setDirection(motor, 0);
    }
    else
    {
        motor_setDirection(motor, 1);
    }

    // output = _constrain(output, 0, 1);
    pwm_setCompare(motor->tb6612->pwm, output);
    DEBUG_PRINT("vel = %f\n", motor->velocity);
}

void motor_pid_positionLoop(Motor_t *motor, float target)
{
    int direction;
    float measued_target = motor_getPosition(motor);
    motor->position = measued_target;
    motor->target = target;
    PID_Update(motor->pid, target, measued_target);
    float output = motor->pid->output;
    if (output < 0)
    {
        output = -output;
        motor_setDirection(motor, 0);
    }
    else
    {
        motor_setDirection(motor, 1);
    }

    output = _constrain(output, 0, 1);
    pwm_setDuty(motor->tb6612->pwm, output);
    DEBUG_PRINT("pos = %f\n", motor->position);
}
void motor_pid_position(Motor_t *motor, float target)
{
    int direction;
    float measued_target = motor_getPosition(motor);
    motor->position = measued_target;
    motor->target_postion = target;
    PIDController_t *pid;
    pid = motor->pid_position;

    PID_Update(pid, target, measued_target);
    float output = pid->output;
    if (output < 0)
    {
        output = -output;
        motor_setDirection(motor, 0);
    }
    else
    {
        motor_setDirection(motor, 1);
    }

    output = _constrain(output, 0, 1);
    pwm_setDuty(motor->tb6612->pwm, output);
    DEBUG_PRINT("pos = %f\n", motor->position);
}

void motor_pid_angleLoop(Motor_t *motor, float target)
{
    int direction;
    float measued_target = motor_getAngle(motor);
    motor->angle = measued_target;
    motor->target = target;
    PID_Update(motor->pid, target, measued_target);
    float output = motor->pid->output;
    if (output < 0)
    {
        output = -output;
        motor_setDirection(motor, 0);
    }
    else
    {
        motor_setDirection(motor, 1);
    }

    output = _constrain(output, 0, 1);
    pwm_setDuty(motor->tb6612->pwm, output);
    DEBUG_PRINT("angle = %f\n", motor->angle);
}
void motor_pid(Motor_t *motor)
{
    motor_pid_velocity(motor, 1);
    // motor_pid_velocity_compare(motor, 1);
}
int motor_move(Motor_t *motor, float target)
{
    switch (motor->controllerType)
    {
    case PID_VelocityLoop:
        // motor_pid_velocity(motor, target);
        motor_pid_velocityLoop(motor, target);
        break;
    case PID_PositionLoop:
        // motor_pid_position(motor, target);
        motor_pid_positionLoop(motor, target);
        break;
    case PID_AngleLoop:
        motor_pid_angleLoop(motor, target);
        break;
    default:
        motor_pid_velocity(motor, target);
        break;
    }
    return 0;
}
