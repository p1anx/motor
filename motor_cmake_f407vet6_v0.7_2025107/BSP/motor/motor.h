#ifndef __MOTOR1_H
#define __MOTOR1_H

#include "pid.h"
#include "types.h"
#include "encoder.h"
#include "tb6612_init.h"
#include "pwm.h"

struct Motor_t
{
    TB6612_t *tb6612;
    PIDController_t *pid_velocity;
    PIDController_t *pid_position;
    float velocity;
    float position;
    float angle;
};

#endif // !__MOTOR1_H
