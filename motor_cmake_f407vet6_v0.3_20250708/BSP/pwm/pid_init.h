#ifndef __PID_INIT_H
#define __PID_INIT_H
#include "mymain.h"
// #include "tb6612.h"
//it's one or the other
#define METHOD0_IsAvailable   1
#define METHOD1_IsAvailable   0


typedef struct {
    float Kp;           // 比例增益
    float Ki;           // 积分增益
    float Kd;           // 微分增益
    float integral;     // 积分项累积
    float prev_error;   // 上一次误差
    float output;       // PID输出
    float output_max;   // 输出限幅
    float output_min;
    float integral_max;   // 输出限幅
    float integral_min;
    float target_velocity;
    float target_position;
    float target_angle;
    float dt;
} PID_Controller;

typedef struct {
   TIM_HandleTypeDef* pwm_tim; 
   uint16_t           pwm_channel;

   TIM_HandleTypeDef* encoder_tim;
   TIM_HandleTypeDef* pid_update_tim;

   int                pulse_count;
   GPIO_TypeDef*      AIN1_GPIO_Port;
   uint16_t           AIN1_GPIO_Pin;
   GPIO_TypeDef*      AIN2_GPIO_Port;
   uint16_t           AIN2_GPIO_Pin;
//    int                direction;
   PID_Controller*    pid_pos_init;
   PID_Controller*    pid_vel_init;
   PID_Controller*    pid_angle_init;

} Motor_InitStruct;

void MG513_Init(Motor_InitStruct* motor);

float PID_Update(PID_Controller* pid, float setpoint, float measurement) ;

int encoder_overflow_cnt_fun(Motor_InitStruct* motor);
int read_motor_direction(Motor_InitStruct* motor);
float read_motor_speed(Motor_InitStruct* motor);
float read_motor_position(Motor_InitStruct* motor);
float read_motor_pulses(Motor_InitStruct* motor);


float positional_pid_position_loop(Motor_InitStruct* motor);
// void positional_pid_position_loop(void);
void tim_control_position(void);

void positional_pid_velocity_loop(Motor_InitStruct* motor);
// void positional_pid_velocity_loop(void);
void key_control_velocity(void);

void tb6612_direction(Motor_InitStruct* motor, int direction);
void tb6612_setDirection(Motor_InitStruct* motor, int direction);
void tb6612_setPWM(Motor_InitStruct* motor,float duty);
void tim_control_position_dual_direction(void);

void tim_control_position_qam_test(void);
void dual_pid_position(Motor_InitStruct *motor);

void key_control_position(Motor_InitStruct *motor);
void timer_control_position_qam(Motor_InitStruct* motor);
void timer_control_position_qam16(Motor_InitStruct* motor);

float anglePID_Update(PID_Controller* pid, float setpoint, float measurement);
float read_motor_angle(Motor_InitStruct *motor) ;
float positional_pid_angle_loop(Motor_InitStruct* motor) ;

#endif
