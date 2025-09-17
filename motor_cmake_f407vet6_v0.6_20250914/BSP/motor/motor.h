#ifndef __MOTOR1_H
#define __MOTOR1_H

#include "pid.h"
#include "sys/types.h"
#include "types.h"
#include "encoder.h"
#include "tb6612_init.h"
#include "pwm.h"

#define RX_SIZE 10
typedef enum
{
    PID_VelocityLoop,
    PID_PositionLoop,
    PID_AngleLoop,
    OpenLoop_Velociy
} ControllerType_t;

struct Motor_t
{
    TB6612_t *tb6612;
    PIDController_t *pid_velocity;
    PIDController_t *pid_position;
    PIDController_t *pid;
    float velocity;
    float position;
    float angle;
    float target;
    float target_vel;
    float target_angle;
    float target_postion;
    int direction;
    char rxCommand[RX_SIZE];
    ControllerType_t controllerType;
};

int motor_linkTB6612(Motor_t *motor, TB6612_t *tb6612);
int motor_init(Motor_t *motor);
int motor_enable(Motor_t *motor);
int motor_disable(Motor_t *motor);
void motor_setDirection(Motor_t *motor, int direction);

float motor_getVelocity(Motor_t *motor);
float motor_getPosition(Motor_t *motor);
float motor_getAngle(Motor_t *motor);

void motor_pid_velocity(Motor_t *motor, float target);
void motor_pid_velocityLoop(Motor_t *motor, float target);

void motor_pid_velocity_compare(Motor_t *motor, float target);

void motor_pid_position(Motor_t *motor, float target);
void motor_pid_positionLoop(Motor_t *motor, float target);
void motor_pid(Motor_t *motor);
int motor_move(Motor_t *motor, float target);
#endif // !__MOTOR1_H
