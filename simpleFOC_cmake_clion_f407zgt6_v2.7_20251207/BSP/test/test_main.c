//
// Created by xwj on 11/4/25.
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
struct g_motorVar
{
    unsigned int t;
    float target;
    float velocity;
    // float ia, ib;
};
char JS_RTT_Buffer[4096];
g_motorVar g_motorVar0;

static void SEGGER_RTT_Config(void)
{
}


extern void test_mt6835();


extern volatile MultiRate_Controller_t MultiRate_Controller;

void multitarget_degrees_move(BLDCMotor_t *motor)
{

    static int last_t = 0;
    static int isFirst = 1;
    static int target_i = 0;
    if (isFirst)
    {
        last_t = HAL_GetTick();
        isFirst = 0;
    }
    int now_t = HAL_GetTick();
    int delta_t = (now_t - last_t);

    if (delta_t > 1000)
    {
        target_i++;
        last_t = now_t;
    }
    if (target_i > 2)
    {
        target_i = 0;
    }
    switch (target_i)
    {
    case 0:
        motor->target = 90.0f;
        break;
    case 1:
        motor->target = 180.0f;
        break;
    case 2:
        motor->target = 270.0f;
        break;
    default:
        motor->target = 0.0f;
        break;
    }
    BLDCMotor_move(motor, motor->target);
}
void multitarget_current_move(BLDCMotor_t *motor)
{
    static int last_t = 0;
    static int isFirst = 1;
    static int target_i = 0;
    if (isFirst)
    {
        last_t = HAL_GetTick();
        isFirst = 0;
    }
    int now_t = HAL_GetTick();
    int delta_t = (now_t - last_t);

    if (delta_t > 1000)
    {
        target_i++;
        last_t = now_t;
    }
    if (target_i > 2)
    {
        target_i = 0;
    }
    switch (target_i)
    {
    case 0:
        BLDCMotor_move(motor, 0.05f);
        break;
    case 1:
        BLDCMotor_move(motor, 0.1f);
        break;
    case 2:
        BLDCMotor_move(motor, 0.2f);
        break;
    default:
        BLDCMotor_move(motor, 0.1f);
        break;
    }
}

void test_key(void)
{
    printf("hello key\n");
    while (1)
    {
        if (Key_Scan(0) == KEY_ON)
        {
            printf("key 0 is ok\n");
            HAL_Delay(1000);
        }
        if (Key_Scan(1) == KEY_ON)
        {
            printf("key 1 is ok\n");
            HAL_Delay(1000);
        }
    }
}

void test_main_as5600(void)
{
    while (1)
    {
        float v = AS5600_getVelocity(&as5600);
        float angle = AS5600_getAngle(&as5600);
        printf("v:%f,%f\n", v, angle);
    }
}
void motor3508_velocityLoop(void)
{
    HAL_TIM_Base_Start(&us_htim);
    PIDController pid_id;
    PIDController pid_iq;
    PIDController pid_degree;

    PIDController pid_velocity;
    motor.PID_velocity.P = 1.f;
    motor.PID_velocity.I = 0.4f;
    motor.PID_velocity.D = 0.0f;
    motor.PID_velocity.limit = 2;
    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_AS5600, ControlType_currentClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);
    // PWM3_Init(CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION);
    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7,EncoderType_MT6835,ControlType_velocityClosedLoop, pid_id, pid_iq, pid_velocity, pid_degree);
    BLDCMotor_init3508(&motor, EncoderType_MT6835, ControlType_velocityClosedLoop);
    delay_ms(1000);
    BLDCMotor_disable(&motor);

    printf("waiting to start motor....\n");
    HAL_UART_Transmit_DMA(&vofa_huart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));

    motor.target = 2.0f;

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
void motor3508_velocityAngleLoop(void)
{
    HAL_TIM_Base_Start(&us_htim);
    PIDController pid_id;
    PIDController pid_iq;
    PIDController pid_degree;

    PIDController pid_velocity;
    motor.PID_velocity.P = 1.f;
    motor.PID_velocity.I = 0.4f;
    motor.PID_velocity.D = 0.0f;
    motor.PID_velocity.limit = 50;

    motor.PID_degree.P = 15.f;
    motor.PID_degree.I = 0.0f;
    motor.PID_degree.D = 0.0f;
    motor.PID_degree.limit = 50;
    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_AS5600, ControlType_currentClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);
    // PWM3_Init(CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION);
    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7,EncoderType_MT6835,ControlType_velocityClosedLoop, pid_id, pid_iq, pid_velocity, pid_degree);
    BLDCMotor_init3508(&motor, EncoderType_MT6835, ControlType_velocityAngleClosedLoop);
    delay_ms(1000);
    BLDCMotor_disable(&motor);

    printf("waiting to start motor....\n");
    HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));

    motor.target = 0.5f;

    int i = 0;
    int last_t = getUs();
    int tx_counter = 0;
    while (1)
    {
        // BLDCMotor_ControlRunByKey(&motor);
        BLDCMotor_KeyControl(&motor);
        if (motor.isEnable) {
            static int last_t = 0;
            int now_t = getUs();
            float delta_target = 0.2f;

            if (now_t - last_t > 2000000) {
                motor.target += delta_target;
                last_t =  now_t;
            }
            if (motor.target > delta_target*4) {
                motor.target = delta_target*0;
            }
            BLDCMotor_move(&motor, motor.target);
            // ia ib id iq
            // UART_SendFloat(4,motor.currentSense->i_a, motor.currentSense->i_b, motor.currentSense->i_d, motor.currentSense->i_q);

            //ua ub ia ib
            // UART_SendFloat(4, motor.Driver.Ualpha, motor.Driver.Ubeta,motor.CurrentSense.i_a, motor.CurrentSense.i_b);
            // UART_DMASendVOFA_justFloat2(motor.target, motor.velocity);

            if (tx_counter++ % 100 == 0) {
                UART_SendFloat(3,motor.target, motor.degree, motor.velocity);

            }

            // HAL_Delay(1);
        }
    }
}
void motor3508_currentLoop(void)
{

    //current pid
    //1. id
    motor.PID_id.P = 2.0f;//10
    motor.PID_id.I = 90.0f; //90
    motor.PID_id.limit = 3;

    //2.
    // motor.PID_id.P = 0.116f;
    // motor.PID_id.I = 471.225f;
    // motor.PID_id.limit = 3;

    motor.PID_iq.P = 2.0f;
    motor.PID_iq.I = 33.f;//33
    motor.PID_iq.limit = 2;
    BLDCMotor_init3508(&motor, EncoderType_MT6835, ControlType_currentClosedLoopBandwidth);
    delay_ms(1000);
    BLDCMotor_disable(&motor);

    printf("waiting to start motor....\n");
    HAL_UART_Transmit_DMA(&vofa_huart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));

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
            float delta_target = 0.2f;

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
void motor3508_currentVelocityLoop(void)
{
    motor.PID_velocity.P = 1.f;
    motor.PID_velocity.I = 0.0f;
    motor.PID_velocity.D = 0.0f;

    //current pid
    motor.PID_id.P = 2.0f;
    motor.PID_id.I = 90.0f;
    motor.PID_id.limit = 3;

    motor.PID_iq.P = 2.0f;
    motor.PID_iq.I = 33.f;
    motor.PID_iq.limit = 3;

    motor.PID_velocity.P = 0.006f;
    motor.PID_velocity.I = 0.002f;
    motor.PID_velocity.limit = 4; //A
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
            float delta_target = -360.f * 4;

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
void motor3508_currentAngleLoop(void)
{
    motor.PID_velocity.P = 1.f;
    motor.PID_velocity.I = 0.0f;
    motor.PID_velocity.D = 0.0f;

    //current pid
    motor.PID_id.P = 10.0f;
    motor.PID_id.I = 5.0f;
    motor.PID_id.limit = 3;

    motor.PID_iq.P = 2.0f;
    motor.PID_iq.I = 1.8f;
    motor.PID_iq.limit = 3;

    motor.PID_degree.P = 2.20f;
    motor.PID_degree.I = 0.50f;
    motor.PID_degree.limit = 2.f; //A
    BLDCMotor_init3508(&motor, EncoderType_MT6835, ControlType_currentAngleClosedLoopBandwidth);
    delay_ms(1000);
    BLDCMotor_disable(&motor);

    printf("waiting to start motor....\n");
    HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));

    // motor.target = 360.f;
    motor.target = 0.5f;

    int i = 0;
    int last_t = getUs();
    while (1)
    {
        // BLDCMotor_ControlRunByKey(&motor);
        BLDCMotor_KeyControl(&motor);
        if (motor.isEnable) {
            static int last_t = 0;
            int now_t = getUs();
            float delta_target = 1.f / 5;

            if (now_t - last_t > 1000000) {
                motor.target += delta_target;
                last_t =  now_t;
            }
            if (motor.target > delta_target*4) {
                motor.target = 0.2f;
            }
            BLDCMotor_move(&motor, motor.target);
            // UART_DMASendVOFA_justFloat2(motor.target, motor.velocity);

            // HAL_Delay(1);
        }
    }
}

void motor3508_currentOpenLoopBandwith(void)
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
void motor3508_currentVelocityAngleLoop(void)
{

    //current pid
    // motor.PID_id.P = 10.0f;
    // motor.PID_id.I = 5.0f;
    // motor.PID_id.limit = 4;
    //
    // motor.PID_iq.P = 2.5f;
    // motor.PID_iq.I = 1.8f;
    // motor.PID_iq.limit = 5;
    //
    // motor.PID_velocity.P = 0.002f;
    // motor.PID_velocity.I = 0.0005f;
    // motor.PID_velocity.limit = 5; //A

    motor.PID_id.P = 2.0f;
    motor.PID_id.I = 90.0f;
    motor.PID_id.limit = 3;

    motor.PID_iq.P = 2.0f;
    motor.PID_iq.I = 33.f;
    motor.PID_iq.limit = 8;

    motor.PID_velocity.P = 0.006f;
    motor.PID_velocity.I = 0.002f;
    motor.PID_velocity.limit = 10; //A

    motor.PID_degree.P = 30.f;
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
            float delta_target = 10;
            static int target_direction = 0;

            if (now_t - last_t > 1000000) {
                if (motor.target >= delta_target*16) {
                    // motor.target = 0;
                    target_direction = 1;
                }
                else if (motor.target <= 20) {
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
// void test_mt6835(void) {
//     Example1_BasicUsage();
// }

extern void test_uart(void);
extern void test_adc_inject(void);
extern void test_2804(void);

extern void test_3pwm();
void test_main(void) {

    test_2804();
    // HAL_TIM_Base_Start(&us_htim);
    // test_mt6835();


    // test_us();
    // test_key0();
    // PWM3_Init(1000, 4096*2);
    // test_PWM3_Init(1000, 4096*2);
    // BLDCDriver_writePWM(0.1, 0.2,0.4);
    // HAL_UART_Transmit(&huart3, "hello uart\n", strlen("hello uart\n"), 1000);

    // test_3pwm();
    // test_key();
    // test_PWM(1000, 1000);
    // test_adc_inject();
    // test_uart();
    //======================================================

    // test_motor_new_driver();
    // test_motor_new_driver_currentLoop();

    //1.ok
    // test_motor_new_driver_currentOpenLoopBandwith();
    // test_motor_new_driver_currentLoopBandwith();
    // test_motor_new_driver_currentVelocityLoopBandwith();
    // test_motor_new_driver_currentVelocityLoopBandwith();

    //3.
    // test_motor_new_driver_currentVelocityAngleLoopBandwith();
    // test_motor_new_driver_currentAngleLoopBandwith();

    //0.
    // motor3508_currentOpenLoopBandwith();
    //1.
    // motor3508_velocityLoop();

    //2.
    // motor3508_currentLoop();

    //3.
    // motor3508_currentVelocityLoop();
    // // motor3508_currentAngleLoop();
    // motor3508_currentVelocityAngleLoop();



    //
    // motor3508_velocityAngleLoop();
}