//
// Created by xwj on 12/8/25.
//



#include "LoopController.h"
#include "as5600.h"
#include "currentSense.h"
#include "key.h"
#include "lowpass_filter.h"
#include "motor.h"
#include "pwm.h"
#include "usart.h"
#include "vofa.h"

#include <stdio.h>

#include <string.h>

extern BLDCMotor_t motor;
extern CurrentSense_t currentSense;
extern AS5600_t as5600;
extern Encoder_t encoder;
extern char txDMA_buffer[256];
extern int TxCompleteFlag;
typedef struct g_motorVar g_motorVar;


static void motor2804_currentOpenLoopBandwith(void)
{

    BLDCMotor_init3508(&motor, EncoderType_MT6835, ControlType_currentVelocityOpenLoop);

    delay_ms(1000);
    printf("waiting to start motor....\n");
    BLDCMotor_disable(&motor);
    HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));


    motor.target = 1.f;

    int i = 0;
    int last_t = getUs();
    int tx_counter = 0;
    while (1)
    {
        // BLDCMotor_ControlRunByKey(&motor);
        BLDCMotor_KeyControl(&motor);
        // motor.isEnable = 1;
        if (motor.isEnable) {
            BLDCMotor_move(&motor, motor.target);
            if (tx_counter++ % 1000 == 0) {
                //1.
                // UART_SendFloat(3, motor.Driver.Ua, motor.Driver.Ub, motor.Driver.Uc);
                // UART_DMASendVOFA_justFloat4(motor.target, motor.Ua, motor.Ub, motor.Uc);
                //2.
                // UART_SendFloat(4,motor.currentSense->i_a, motor.currentSense->i_b, motor.currentSense->i_d, motor.currentSense->i_q);

                // ia ib id iq
                // UART_SendFloat(4,motor.currentSense->i_a, motor.currentSense->i_b, motor.currentSense->i_d, motor.currentSense->i_q);

#if 0
                float buffer[10];
                buffer[0] = motor.currentSense->i_a;
                buffer[1] = motor.currentSense->i_b;
                buffer[2] = motor.currentSense->i_d;
                buffer[3] = motor.currentSense->i_q;
                VOFA_SendJustFloat(buffer, 4);
#endif


                //ua ub ia ib
                // UART_SendFloat(4, motor.Driver.Ualpha, motor.Driver.Ubeta,motor.CurrentSense.i_a, motor.CurrentSense.i_b);
                // UART_SendFloat(2, motor.currentSense->v_a, motor.currentSense->v_b);

            }


            // HAL_Delay(1);
        }
    }
}

static void motor2804_velocityLoop(void)
{
    HAL_TIM_Base_Start(&us_htim);
    PIDController pid_id;
    PIDController pid_iq;
    PIDController pid_degree;

    PIDController pid_velocity;
    motor.PID_velocity.P = 1.f;
    motor.PID_velocity.I = 0.4f;
    motor.PID_velocity.D = 0.0f;
    motor.PID_velocity.limit = 8;
    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_AS5600, ControlType_currentClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);
    // PWM3_Init(CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION);
    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7,EncoderType_MT6835,ControlType_velocityClosedLoop, pid_id, pid_iq, pid_velocity, pid_degree);
    BLDCMotor_init3508(&motor, EncoderType_MT6835, ControlType_velocityClosedLoop);
    delay_ms(1000);
    BLDCMotor_disable(&motor);

    printf("waiting to start motor....\n");
    HAL_UART_Transmit_DMA(&vofa_huart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));
    // motor.monitorType = Monitor_NULL;
    motor.monitorType = Monitor_Velocity;
    // motor.monitorType = Monitor_IalphaIbetaUabc;

    motor.target = -2.0f;

    int i = 0;
    int last_t = getUs();
    while (1)
    {
        // BLDCMotor_ControlRunByKey(&motor);
        BLDCMotor_KeyControl(&motor);
        if (motor.isEnable) {
            // printf("motor is start\n");
            // static int last_t = 0;
            // int now_t = getUs();
            // float delta_target = 2.0f;
            //
            // if (now_t - last_t > 2000000) {
            //     motor.target += delta_target;
            //     last_t =  now_t;
            // }
            // if (motor.target > delta_target*4) {
            //     motor.target = delta_target;
            // }
            BLDCMotor_move(&motor, motor.target);
            // ia ib id iq
            // UART_SendFloat(4,motor.currentSense->i_a, motor.currentSense->i_b, motor.currentSense->i_d, motor.currentSense->i_q);

            //ua ub ia ib
            // UART_SendFloat(4, motor.Driver.Ualpha, motor.Driver.Ubeta,motor.CurrentSense.i_a, motor.CurrentSense.i_b);
            // UART_DMASendVOFA_justFloat2(motor.target, motor.velocity);

            // HAL_Delay(1);
        }
    }
}
void motor2804_currentLoop(void)
{

    //current pid
    //1. id
    motor.PID_id.P = 30.0f;//10
    motor.PID_id.I = 90.0f; //90
    motor.PID_id.limit = 6;

    //2.
    // motor.PID_id.P = 0.116f;
    // motor.PID_id.I = 471.225f;
    // motor.PID_id.limit = 3;

    motor.PID_iq.P = 30.0f;
    motor.PID_iq.I = 200.f;//33
    motor.PID_iq.limit = 6;
    BLDCMotor_init3508(&motor, EncoderType_MT6835, ControlType_currentClosedLoopBandwidth);
    delay_ms(1000);
    BLDCMotor_disable(&motor);

    printf("waiting to start motor....\n");
    HAL_UART_Transmit_DMA(&vofa_huart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));

    // motor.monitorType = Monitor_TargetIdIq;
    motor.monitorType = Monitor_NULL;
    motor.target = 0.0f;

    int i = 0;
    int last_t = getUs();
    while (1)
    {
        // BLDCMotor_ControlRunByKey(&motor);
        BLDCMotor_KeyControl(&motor);
        if (motor.isEnable) {
            static int last_t = 0;
            int now_t = getUs();
            float delta_target = 0.1f;

            if (now_t - last_t > 1000000) {
                motor.target += delta_target;
                last_t =  now_t;
            }
            if (motor.target > delta_target*4) {
                motor.target = 0;
            }
            BLDCMotor_move(&motor, motor.target);
            // UART_DMASendVOFA_justFloat2(motor.target, motor.velocity);

            // HAL_Delay(1);
        }
    }
}
void motor2804_currentVelocityLoop(void)
{
    motor.PID_id.P = 30.0f;//10
    motor.PID_id.I = 90.0f; //90
    motor.PID_id.limit = 6;

    //2.
    // motor.PID_id.P = 0.116f;
    // motor.PID_id.I = 471.225f;
    // motor.PID_id.limit = 3;

    motor.PID_iq.P = 30.0f;
    motor.PID_iq.I = 200.f;//33
    motor.PID_iq.limit = 6;

    motor.PID_velocity.P = 0.001f;
    motor.PID_velocity.I = 0.000f;
    motor.PID_velocity.limit = 1.f; //A

    BLDCMotor_init3508(&motor, EncoderType_MT6835, ControlType_currentVelocityClosedLoopBandwidth);
    delay_ms(1000);
    BLDCMotor_disable(&motor);

    printf("waiting to start motor....\n");
    HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));

    // motor.target = 360.f;
    motor.target = 0.0f;

    int i = 0;
    int last_t = getUs();
    while (1)
    {
        // BLDCMotor_ControlRunByKey(&motor);
        BLDCMotor_KeyControl(&motor);
        if (motor.isEnable) {
            const int num = 6;
            static int last_t = 0;
            int now_t = getUs();
            float delta_target = 360.f * 1;

            if (now_t - last_t > 1000000) {
                motor.target += delta_target;
                last_t =  now_t;
            }
            if (delta_target > 0) {
                if (motor.target > delta_target* num) {
                    motor.target = 0;
                }
            }
            else {
                if (motor.target < delta_target* num) {
                    motor.target = 0;
                }

            }
            // motor.target = 360.0f;
            BLDCMotor_move(&motor, motor.target);
            // UART_DMASendVOFA_justFloat2(motor.target, motor.velocity);

            // HAL_Delay(1);
        }
    }
}
void motor2804_currentVelocityAngleLoop(void)
{

    motor.PID_id.P = 30.0f;//10
    motor.PID_id.I = 90.0f; //90
    motor.PID_id.limit = 6;

    //2.
    // motor.PID_id.P = 0.116f;
    // motor.PID_id.I = 471.225f;
    // motor.PID_id.limit = 3;

    motor.PID_iq.P = 30.0f;
    motor.PID_iq.I = 200.f;//33
    motor.PID_iq.limit = 6;

    motor.PID_velocity.P = 0.001f;
    motor.PID_velocity.I = 0.0005f;
    motor.PID_velocity.limit = 0.5f; //A

    motor.PID_degree.P = 40.f;
    motor.PID_degree.I = 0.0000f;
    motor.PID_degree.limit = 360*15; //deg/s
    BLDCMotor_init3508(&motor, EncoderType_MT6835, ControlType_currentVelocityAngleClosedLoopBandwidth);
    delay_ms(1000);
    BLDCMotor_disable(&motor);

    printf("waiting to start motor....\n");
    HAL_UART_Transmit_DMA(&vofa_huart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));

    // motor.target = 360.f;
    motor.target = 0.0f;

    int i = 0;
    int last_t = getUs();
    while (1)
    {
        // BLDCMotor_ControlRunByKey(&motor);
        BLDCMotor_KeyControl(&motor);
        if (motor.isEnable) {
            static int last_t = 0;
            int now_t = getUs();
            float delta_target = 1;
            static int target_direction = 0;

            if (now_t - last_t > 1000000) {
                if (motor.target >= delta_target*16) {
                    // motor.target = 0;
                    target_direction = 1;
                }
                else if (motor.target <= 10) {
                    target_direction = 0;
                }
                if (target_direction == 0) {
                    motor.target += delta_target;
                }
                else {
                    motor.target -= delta_target;
                }
                last_t =  now_t;
            }
            BLDCMotor_move(&motor, motor.target);
            // VOFA_SendJustFloat(&motor.degree, 1);
            // UART_DMASendVOFA_justFloat2(motor.target, motor.velocity);

            // HAL_Delay(1);
        }
    }
}


void test_2804(void) {
    // motor2804_currentOpenLoopBandwith();
    // motor2804_velocityLoop();
    // motor2804_currentLoop();
    // motor2804_currentVelocityLoop();
    motor2804_currentVelocityAngleLoop();

}