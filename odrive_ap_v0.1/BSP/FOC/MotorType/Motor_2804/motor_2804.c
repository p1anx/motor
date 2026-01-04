//
// Created by xwj on 11/27/25.
//

#include "motor_2804.h"

#include "LoopController.h"
#include "as5600.h"
#include "currentSense.h"
#include "key.h"
#include "lowpass_filter.h"
#include "motor.h"
#include "pwm.h"
#include "usart.h"
#include <stdio.h>

#include <string.h>

extern BLDCMotor_t motor;
extern CurrentSense_t currentSense;
extern AS5600_t as5600;
extern Encoder_t encoder;

extern char txDMA_buffer[256];
extern int TxCompleteFlag;

void motor_2804_currentOpenLoopBandwith(void)
{
    HAL_TIM_Base_Start(&htim2);
    PIDController pid_velocity;
    PIDController pid_degree;

    PIDController pid_id;
    //1. ok
    // pid_id.P = 60.0f;
    // pid_id.I = 20.0f;
    // pid_id.D = 0.0f;
    pid_id.P = 20.0f;
    pid_id.I = 00.0f;
    pid_id.D = 0.0f;
    PIDController pid_iq;
    //2. ok
    // pid_iq.P = 60.0f;
    // pid_iq.I = 30.0f;
    // pid_iq.D = 0.0f;
    pid_iq.P = 20.0f;
    pid_iq.I = 00.0f;
    pid_iq.D = 0.0f;

    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_AS5600, ControlType_currentClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);
    // PWM3_Init(CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION);
    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7,EncoderType_MT6835, ControlType_currentClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);
    BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7,EncoderType_MT6835, ControlType_currentVelocityOpenLoop, pid_id, pid_iq, pid_velocity, pid_degree);

    delay_ms(1000);
    printf("waiting to start motor....\n");
    BLDCMotor_disable(&motor);
    HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));


    motor.target = 2.f;

    int i = 0;
    int last_t = getUs();
    while (1)
    {
        // BLDCMotor_ControlRunByKey(&motor);
        BLDCMotor_KeyControl(&motor);
        if (motor.isEnable) {
                BLDCMotor_move(&motor, motor.target);

            // HAL_Delay(1);
        }
    }
}

void motor_2804_currentLoopBandwith(void)
{
    PIDController pid_velocity;
    PIDController pid_degree;

    PIDController pid_id;
    //1. ok
    // pid_id.P = 60.0f;
    // pid_id.I = 20.0f;
    // pid_id.D = 0.0f;
    pid_id.P = 60.0f;
    pid_id.I = 20.0f;
    pid_id.D = 0.0f;
    PIDController pid_iq;
    //2. ok
    // pid_iq.P = 60.0f;
    // pid_iq.I = 30.0f;
    // pid_iq.D = 0.0f;
    pid_iq.P = 50.0f;
    pid_iq.I = 60.0f;//40
    pid_iq.D = 0.0f;

    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_AS5600, ControlType_currentClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);
    // PWM3_Init(CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION);
    BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7,EncoderType_MT6835, ControlType_currentClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);

    delay_ms(1000);
    printf("waiting to start motor....\n");
    BLDCMotor_disable(&motor);
    // HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));

    HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));

    motor.target = 0.00f;

    int i = 0;
    int t0,t1;
    while (1)
    {
        // BLDCMotor_ControlRunByKey(&motor);
        BLDCMotor_KeyControl(&motor);
        if (motor.isEnable) {

            static int last_t = 0;
            int now_t = getUs();

            if (now_t - last_t > 1000000) {
                motor.target += 0.01f;
                last_t =  now_t;
            }
            if (motor.target > 0.05) {
                motor.target = 0.00f;
            }
            BLDCMotor_move(&motor, motor.target);

            // t1 = getUs();
            // int delta_t = t1 - t0;
            // UART_SendFloat(1, (float)delta_t);

            // HAL_Delay(1);
        }
    }
}
void motor_2804_currentVelocityLoopBandwith(void)
{
    PIDController pid_degree;
    PIDController pid_id;
    //1. ok
    // pid_id.P = 60.0f;
    // pid_id.I = 20.0f;
    // pid_id.D = 0.0f;
    pid_id.P = 60.0f;
    pid_id.I = 20.0f;
    pid_id.D = 0.0f;
    PIDController pid_iq;
    //2. ok
    // pid_iq.P = 60.0f;
    // pid_iq.I = 30.0f;
    // pid_iq.D = 0.0f;
    pid_iq.P = 50.0f;
    pid_iq.I = 60.0f;
    pid_iq.D = 0.0f;

    //for Hz velocity
    // PIDController pid_velocity;
    // pid_velocity.P = 0.03f;
    // pid_velocity.I = 0.01f;
    // pid_velocity.D = 0.0f;

    PIDController pid_velocity;
    pid_velocity.P = 0.001f;
    pid_velocity.I = 0.0001f;
    pid_velocity.D = 0.0f;

    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_AS5600, ControlType_currentClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);
    // PWM3_Init(CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION);
    BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7,EncoderType_MT6835, ControlType_currentVelocityClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);

    delay_ms(1000);
    printf("waiting to start motor....\n");
    BLDCMotor_disable(&motor);
    HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));


    motor.target = 360.0f;

    int i = 0;
    int last_t = getUs();
    int t0,t1;
    while (1)
    {
        // BLDCMotor_ControlRunByKey(&motor);
        BLDCMotor_KeyControl(&motor);
        if (motor.isEnable) {

            static int last_t = 0;
            int now_t = getUs();
            float delta_target = 360.0f*2;

            if (now_t - last_t > 2000000) {
                motor.target += delta_target;
                last_t =  now_t;
            }
            if (motor.target > delta_target*4) {
                motor.target = delta_target;
            }
            BLDCMotor_move(&motor, motor.target);
            // t1 = getUs();
            // int delta_t = t1 - t0;
            // UART_SendFloat(1, (float)delta_t);

            // HAL_Delay(1);
        }
    }
}
void motor_2804_currentVelocityAngleLoopBandwith(void)
{
    PIDController pid_id;
    //1. ok
    // pid_id.P = 60.0f;
    // pid_id.I = 20.0f;
    // pid_id.D = 0.0f;
    pid_id.P = 60.0f;
    pid_id.I = 20.0f;
    pid_id.D = 0.0f;
    PIDController pid_iq;
    //2. ok
    // pid_iq.P = 60.0f;
    // pid_iq.I = 30.0f;
    // pid_iq.D = 0.0f;
    pid_iq.P = 50.0f;
    pid_iq.I = 60.0f;
    pid_iq.D = 0.0f;

    PIDController pid_velocity;
    // pid_velocity.P = 0.001f;
    // pid_velocity.I = 0.0001f;
    // pid_velocity.D = 0.0f;
    pid_velocity.P = 0.001f;
    pid_velocity.I = 0.0001f;
    pid_velocity.D = 0.0f;
    PIDController pid_degree;
    pid_degree.P = 12.f;
    pid_degree.I = 0.0f;
    pid_degree.D = 0.000f;

    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_AS5600, ControlType_currentClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);
    // PWM3_Init(CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION);
    BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7,EncoderType_MT6835, ControlType_currentVelocityAngleClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);

    delay_ms(1000);
    printf("waiting to start motor....\n");
    BLDCMotor_disable(&motor);
    HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));


    motor.target = 00.0f;

    int i = 0;
    int last_t = getUs();
    int t0,t1;
    while (1)
    {
        // BLDCMotor_ControlRunByKey(&motor);
        BLDCMotor_KeyControl(&motor);
        if (motor.isEnable) {
            static int last_t = 0;
            int now_t = getUs();
            float delta_target = 60.0f;

            if (now_t - last_t > 2000000) {
                motor.target += delta_target;
                last_t =  now_t;
            }
            if (motor.target > delta_target*4) {
                motor.target = delta_target;
            }

            BLDCMotor_move(&motor, motor.target);
            // t1 = getUs();
            // int delta_t = t1 - t0;
            // UART_SendFloat(1, (float)delta_t);

            // HAL_Delay(1);
        }
    }
}
