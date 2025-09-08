#ifndef __MOTOR_H
#define __MOTOR_H

#include "mymain.h"
#include "pid_init.h"

#define MOTOR1_ON 1
#define MOTOR2_ON 1

#define POSITION_INCREMENT (5000.0f*2 + 80)/4
#define POSITION_INCREMENT_60 6000.0f
#define ANGLE_INC           180

typedef struct {
    int time_flag;

}GlobalVar_Struct;
int motor_main(void);
void motor_run(void);
void qam_test(void);
void pid(void);

int time_test(void);
int calc_time_use_timer(void);
int calc_rotating_time(void);
// int calc_rotating_time(Motor_InitStruct* motor);
// int timer_increase_position(Motor_InitStruct* motor);
#endif
