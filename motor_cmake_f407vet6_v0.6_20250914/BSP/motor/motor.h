#ifndef __MOTOR1_H
#define __MOTOR1_H

#include "pid.h"
#include "sys/types.h"
#include "types.h"
#include "encoder.h"
#include "tb6612_init.h"
#include "pwm.h"

#define RX_SIZE 10

struct Motor_t
{
    TB6612_t *tb6612;
    PIDController_t *pid_velocity;
    PIDController_t *pid_position;
    float velocity;
    float position;
    float angle;
    float target_vel;
    char rxCommand[RX_SIZE];
};

int motor_linkTB6612(Motor_t *motor, TB6612_t *tb6612);
int motor_init(Motor_t *motor);
float motor_getVelocity(Motor_t *motor);
void motor_pid_velocity(Motor_t *motor, float target);

void motor_pid(Motor_t *motor);
#endif // !__MOTOR1_H
