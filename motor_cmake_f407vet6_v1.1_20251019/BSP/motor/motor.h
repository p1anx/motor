#ifndef __MOTOR1_H
#define __MOTOR1_H

#include "pid.h"
#include "types.h"
#include "encoder.h"
#include "tb6612_init.h"
#include "lis3mdl.h"
#include "pwm.h"
#include <stdbool.h>

#define RX_SIZE 10
typedef enum
{
    PID_VelocityLoop,
    PID_PositionLoop,
    PID_AngleLoop,
    PID_MagneticLoop,
    OpenLoop_Velociy
} ControllerType_t;

struct Motor_t
{
    TB6612_t *tb6612;
    LIS3MDL_t *lis3mdl;
    PIDController_t *pid_velocity;
    PIDController_t *pid_position;
    PIDController_t *pid_magnetic;
    PIDController_t *pid;
    float velocity;
    float position;
    float magnetic;
    float angle;
    float target;
    float target_vel;
    float target_angle;
    float target_postion;
    bool isEnable;
    bool isDisable;
    int direction;
    char rxCommand[RX_SIZE];
    ControllerType_t controllerType;
    float setVelocity;
    float setAngle;
    float setPostion;
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
float motor_getMagnetic(void);
LIS3MDL_t motor_getMagneticGauss(void);

void motor_pid_update(Motor_t *motor);
#endif // !__MOTOR1_H
