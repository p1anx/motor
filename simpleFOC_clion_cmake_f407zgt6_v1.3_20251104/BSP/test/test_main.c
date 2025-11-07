//
// Created by xwj on 11/4/25.
//
#include "key.h"
#include "motor.h"
#include <stdio.h>
#include "pwm.h"
#include "as5600.h"
#include "currentSense.h"
#include "lowpass_filter.h"

extern BLDCMotor_t motor;
extern CurrentSense_t currentSense;
extern AS5600_t as5600;
extern Encoder_t encoder;


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

void test_main_pwm(void)
{

    PWM6_Init(1000, 4096);
    // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
    while (1)
    {
        printf("test_motor\n");
        delay_ms(1000);
    }
}
void test_main_openloop(void)
{
    PIDController pid_a;
    pid_a.P = 0.9f;
    pid_a.I = 0.1f;
    pid_a.D = 0.0f;
    // MotorFOC_Init(&motor, pid_a, ControlType_velocityClosedLoop);
    MotorFOC_Init(&motor, pid_a, ControlType_velocity_openloop);
    // HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_SET);
    delay_ms(1000);
    static int debug_count = 0;
    char buffer[20];
    int last_t = HAL_GetTick();
    while (1)
    {
        //1.
        if (Key_Scan(0) == KEY_ON)
        {
            // BLDCMotor_enable(&motor);
            // motor.isEnable = 1;
        }
        if (motor.isEnable == 1)
        {
            BLDCMotor_move(&motor, 2);
        }
            // UART_SendDataFloat(&motor.velocity, 1);
    }

}
void test_main_adc(void)
{
    CurrentSense_Scan_Init(&currentSense);

    int current;
    while (1)
    {
        //1.
        currentSense_get3PhaseVoltage(&currentSense);
        printf("i:%f,%f,%f\n", currentSense.i_a, currentSense.i_b, currentSense.i_c);
        //2.
        // CurrentSense_Scan(&currentSense, 1, &current);
        // printf("i: %f\n", (float)current/4096*3.3);
        delay_ms(1000);

    }

}

void test_main_openloop_with_current(void)
{
    PIDController pid_a;
    pid_a.P = 0.9f;
    pid_a.I = 0.1f;
    pid_a.D = 0.0f;
    // MotorFOC_Init(&motor, pid_a, ControlType_velocityClosedLoop);
    CurrentSense_Scan_Init(&currentSense);
    MotorFOC_Init_NotCurrent(&motor, pid_a, ControlType_velocity_openloop);
    // HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_SET);
    delay_ms(1000);
    static int debug_count = 0;
    char buffer[20];
    int last_t = HAL_GetTick();
    while (1)
    {
        //1.
        if (Key_Scan(0) == KEY_ON)
        {
            motor.isEnable = 1;
        }
        else if (Key_Scan(1) == KEY_ON)
        {
            motor.isEnable = 0;
        }
        if (motor.isEnable)
        {
            BLDCMotor_enable(&motor);
            BLDCMotor_move(&motor, -1);
            // currentSense_get3PhaseVoltage(&currentSense);
            currentSense_get3PhaseCurrent(&currentSense);
            printf("i:%f, %f, %f\n", currentSense.i_a, currentSense.i_b, currentSense.i_c);
        }
        else
        {
            BLDCMotor_disable(&motor);
            // currentSense_get3PhaseVoltage(&currentSense);
            currentSense_get3PhaseCurrent(&currentSense);
            printf("i:%f, %f, %f\n", currentSense.i_a, currentSense.i_b, currentSense.i_c);
        }
        // UART_SendDataFloat(&motor.velocity, 1);
    }

}

void test_main_openloop_with_current_v1(void)
{
    PIDController pid_a;
    pid_a.P = 0.9f;
    pid_a.I = 0.1f;
    pid_a.D = 0.0f;
    // MotorFOC_Init(&motor, pid_a, ControlType_velocityClosedLoop);
    // CurrentSense_Scan_Init(&currentSense);
    MotorFOC_Init(&motor, pid_a, ControlType_velocity_openloop);
    // HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_SET);
    delay_ms(1000);
    static int debug_count = 0;
    char buffer[20];
    int last_t = HAL_GetTick();
    float i_list[3];
    while (1)
    {
        //1.
        if (Key_Scan(0) == KEY_ON)
        {
            motor.isEnable = 1;
        }
        else if (Key_Scan(1) == KEY_ON)
        {
            motor.isEnable = 0;
        }
        if (motor.isEnable)
        {
            BLDCMotor_enable(&motor);
            BLDCMotor_move(&motor, 1);

            CurrentSense_read3Current(&currentSense);
            // currentSense.i_a = LowPassFilter(&lpf_a, currentSense.i_a);
            // currentSense.i_b = LowPassFilter(&lpf_b, currentSense.i_c);

            // printf("i:%f, %f, %f, %f\n",motor.velocity, currentSense.i_a, currentSense.i_b, currentSense.i_c);

            i_list[0] = currentSense.i_a;
            i_list[1] = currentSense.i_b;
            i_list[2] = currentSense.i_c;
            UART_SendDataFloat(i_list, 3);
            // currentSense_get3PhaseVoltage(&currentSense);
            // printf("i:%f, %f, %f\n", currentSense.i_a, currentSense.i_b, currentSense.i_c);
        }
        else
        {
            BLDCMotor_disable(&motor);
            CurrentSense_read3Current(&currentSense);
            //1.
            // printf("i:%f, %f, %f, %f\n",motor.velocity, currentSense.i_a, currentSense.i_b, currentSense.i_c);
            // currentSense.i_a = LowPassFilter(&lpf_a, currentSense.i_a);
            // currentSense.i_b = LowPassFilter(&lpf_b, currentSense.i_c);
            //2.
            i_list[0] = currentSense.i_a;
            i_list[1] = currentSense.i_b;
            i_list[2] = currentSense.i_c;
            UART_SendFloat(3, currentSense.i_a, currentSense.i_b, currentSense.i_c);
            // UART_SendDataFloat(i_list, 3);

            // currentSense_get3PhaseVoltage(&currentSense);
            // printf("i:%f, %f, %f\n", currentSense.i_a, currentSense.i_b, currentSense.i_c);
        }
        // UART_SendDataFloat(&motor.velocity, 1);
    }

}
void test_main_motor_velocityLoop(void)
{
    PIDController pid_a;
    pid_a.P = 1.0f;
    pid_a.I = 0.3f;
    pid_a.D = 0.0f;
    //1.
    MotorFOC_Init(&motor, pid_a, ControlType_velocityClosedLoop);
    //2.
    // MotorFOC_Init(&motor, pid_a, ControlType_velocity_openloop);
    delay_ms(1000);
    static int debug_count = 0;
    float buffer[20];
    int last_t = HAL_GetTick();
    float i_list[3];
    const float target_angle[3] = {_PI_2, _PI, _3PI_2};
    float set_v = -4.0f;
    int i = 0;
    int isFirst = 1;
    // while (1)
    // {
    //
    //     if (isFirst)
    //     {
    //
    //     for (i = 0; i < 3; i++)
    //     {
    //     BLDCMotor_enable(&motor);
    //
    //     BLDCMotor_setPhaseVoltage(&motor, set_v, 0, target_angle[i]);
    //     float now_angle = BLDCMotor_getAngle(&motor);
    //     float e_angle = _electricalAngle(now_angle, 7);
    //     printf("angle = %f, target_e_angle = %f, e_angle = %f\n",now_angle, target_angle[i], e_angle);
    //     delay_ms(2000);
    //     }
    //         isFirst = 0;
    //
    //     }
    //     if (isFirst == 0)
    //     {
    //         BLDCMotor_disable(&motor);
    //     }
    //
    //
    //
    // }
    while (1)
    {
        //1.
        int current_t = HAL_GetTick();
        if (Key_Scan(0) == KEY_ON)
        {
            motor.isEnable = 1;
        }
        else if (Key_Scan(1) == KEY_ON)
        {
            motor.isEnable = 0;
        }
        if (motor.isEnable)
        {
            /*if (current_t - last_t > 10)
            {*/
            const int t = 1000;
                BLDCMotor_enable(&motor);
                for (i = 0; i < t; i++)
                {
                    BLDCMotor_move(&motor, 1);
                    printf("v:%f, %f, %f,%f,%f\n", motor.target, motor.velocity, motor.foc_motor.voltage_q,motor.angle, motor.e_angle);

                }
            for (i = 0; i < t; i++)
            {
                BLDCMotor_move(&motor, 2);
                printf("v:%f, %f, %f,%f,%f\n", motor.target, motor.velocity, motor.foc_motor.voltage_q,motor.angle, motor.e_angle);

            }
            for (i = 0; i < t; i++)
            {
                BLDCMotor_move(&motor, 3);
                printf("v:%f, %f, %f,%f,%f\n", motor.target, motor.velocity, motor.foc_motor.voltage_q,motor.angle, motor.e_angle);

            }
            for (i = 0; i < t; i++)
            {
                BLDCMotor_move(&motor, 4);
                printf("v:%f, %f, %f,%f,%f\n", motor.target, motor.velocity, motor.foc_motor.voltage_q,motor.angle, motor.e_angle);

            }
            /*last_t = current_t;
            }*/
        }
        else
        {
            BLDCMotor_disable(&motor);
            // currentSense_get3PhaseVoltage(&currentSense);
            //1.
            // printf("v:%f, %f\n", motor.target, motor.velocity);
            //2.
        }
        //1.
        // printf("v:%f, %f, %f,%f,%f\n", motor.target, motor.velocity, motor.foc_motor.voltage_q,motor.angle, motor.e_angle);
        //2.
        // currentSense_get3PhaseCurrent(&currentSense);
        // printf("i:%f, %f, %f\n", currentSense.i_a, currentSense.i_b, currentSense.i_c);
        //3.
        // currentSense_get3PhaseCurrent(&currentSense);
        // i_list[0] = currentSense.i_a;
        // i_list[1] = currentSense.i_b;
        // i_list[2] = currentSense.i_c;
        // UART_SendDataFloat(i_list, 8);
        //4.
        // buffer[0] = motor.target;
        // buffer[1] = motor.velocity;
        // UART_SendDataFloat(buffer, 2);
        int last_t = HAL_GetTick();
        // if (debug_count++ %10 == 0)
        // {
        //
        //     printf("delta t = %d\n", last_t - current_t);
        //     UART_SendDataFloat(i_list, 3);
        //
        // }
        delay_ms(1);
        // UART_SendDataFloat(&motor.velocity, 1);
    }

}
void test_main_as5600(void)
{
    while (1)
    {
       float v =  AS5600_getVelocity(&as5600);
       float angle =  AS5600_getAngle(&as5600);
        printf("v:%f,%f\n", v, angle);

    }

}
void test_main_encoder(void)
{

    BLDCMotor_init(&motor, 7);
    BLDCMotor_linkEncoder(&motor, &encoder);
    int debug_count = 0;
    while (1)
    {
        float v =  BLDCMotor_getVelocity(&motor);
        // float angle =  BLDCMotor_getAngle(&motor);
        float angle =  motor.foc_motor.encoder->as5600_Instance.angle;
        float e_angle = _electricalAngle(angle, 7);

        if (debug_count++ % 1 == 0)
        {
            printf("v:%f,%f,%f\n", v, angle, e_angle);

        }
        delay_ms(1);

    }

}
void test_electrical_angle(void)
{
    Encoder_init(&encoder);
    BLDCMotor_init(&motor, 7);
    BLDCMotor_linkEncoder(&motor, &encoder);

    printf("\n=== Testing Electrical Angle Calculation ===\n");

    // 手动转动转子，记录角度变化
    // for (int i = 0; i < 10; i++) {
    while (1) {
        float mech_angle = BLDCMotor_getAngle(&motor);
        float elec_angle = _electricalAngle(mech_angle, 7);

        printf("Mechanical: %.2f rad (%.1f°), Electrical: %.2f rad (%.1f°)\n",
               mech_angle, mech_angle*180/_PI,
               elec_angle, elec_angle*180/_PI);

        HAL_Delay(500);
    }

    printf("\n观察：机械角度递增时，电角度应该递增（正转）或递减（反转）\n");
    printf("如果电角度变化方向与实际旋转相反，需要修改极对数符号\n");
}
void test_main_motor_velocityLoop_with_current(void)
{
    PIDController pid_a;
    pid_a.P = 1.0f;
    pid_a.I = 0.3f;
    pid_a.D = 0.0f;
    //1.
    MotorFOC_Init(&motor, pid_a, ControlType_velocityClosedLoop);
    //2.
    // MotorFOC_Init(&motor, pid_a, ControlType_velocity_openloop);
    delay_ms(1000);
    static int debug_count = 0;
    float buffer[20];
    int last_t = HAL_GetTick();
    float i_list[3];
    const float target_angle[3] = {_PI_2, _PI, _3PI_2};
    LowPassFilter_t lpf_a;
    LowPassFilter_t lpf_b;
    LowPassFilter_init(&lpf_a, 0.5);
    LowPassFilter_init(&lpf_b, 0.5);
    while (1)
    {
        //1.
        currentSense_get3PhaseCurrent(&currentSense);
        int current_t = HAL_GetTick();
        if (Key_Scan(0) == KEY_ON)
        {
            motor.isEnable = 1;
        }
        else if (Key_Scan(1) == KEY_ON)
        {
            motor.isEnable = 0;
        }
        if (motor.isEnable)
        {
            /*if (current_t - last_t > 10)
            {*/
            const int t = 1000;
                BLDCMotor_enable(&motor);
                BLDCMotor_move(&motor, 4);
            /*last_t = current_t;
            }*/
        }
        else
        {
            BLDCMotor_disable(&motor);
            // currentSense_get3PhaseVoltage(&currentSense);
            //1.
            // printf("v:%f, %f\n", motor.target, motor.velocity);
            //2.
        }
        //1.
        // printf("v:%f, %f, %f,%f,%f\n", motor.target, motor.velocity, motor.foc_motor.voltage_q,motor.angle, motor.e_angle);
        //2.
        currentSense.i_a = LowPassFilter(&lpf_a, currentSense.i_a);
        currentSense.i_b = LowPassFilter(&lpf_b, currentSense.i_b);

        // printf("i:%f, %f, %f\n", currentSense.i_a, currentSense.i_b, currentSense.i_c);
        // buffer[0] = motor.target;
        // buffer[1] = motor.velocity;
        // buffer[2] = currentSense.i_a;
        // buffer[3] = currentSense.i_b;
        // buffer[4] = currentSense.i_c;
        // UART_SendDataFloat(buffer, 5);

        //5.
        float i_a, i_b, i_alpha, i_beta, i_q, i_d;
        i_a = currentSense.i_a;
        i_b = currentSense.i_b;
        Clarke_Transform(i_a, i_b, &i_alpha, &i_beta);
        Park_Transform(i_alpha, i_beta, _normalizeAngle(motor.e_angle), &i_d, &i_q);
        i_d = LowPassFilter(&lpf_a, i_d);
        i_q = LowPassFilter(&lpf_a, i_q);
        motor.foc_motor.i_d = i_d;
        motor.foc_motor.i_q = i_q;
        buffer[0] = motor.target;
        buffer[1] = motor.velocity;
        buffer[2] = i_d;
        buffer[3] = i_q;
        UART_SendDataFloat(buffer, 4);

        // if (debug_count++ %10 == 0)
        // {
        //
        //     printf("delta t = %d\n", last_t - current_t);
        //     UART_SendDataFloat(i_list, 3);
        //
        // }
        delay_ms(1);
        // UART_SendDataFloat(&motor.velocity, 1);
    }

}
extern int adc1_value;
#define ADC_BUFFER_SIZE 1000
uint16_t v[ADC_BUFFER_SIZE];
extern int adc_cmp_flag;
int current_rawValue[ADC_BUFFER_SIZE];
void test_main_adc1(void)
{
    //1.
    // PWM6_Init(2000, 1000);
    // HAL_TIM_Base_Start(&htim2);
    // HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc1_value, 1);


    //2.
    PWM6_Init(10000, 4096);
    // __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 998);
    //
    // HAL_ADCEx_InjectedStart(&hadc1);
    // HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&v, ADC_BUFFER_SIZE);

    // HAL_TIM_Base_Start(&htim2);
    // PWM6_Init(2000, 1000);
    //
    //3.
    // HAL_TIM_Base_Start_IT(&pwm_tim);
    // HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc1_value, 1);
    // HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&v, ADC_BUFFER_SIZE);
    //4.

    __HAL_ADC_ENABLE_IT(&hadc1, ADC_IT_JEOC);
    HAL_ADCEx_InjectedStart_IT(&hadc1);
    while (1)
    {

        if ( adc_cmp_flag == 1)
        {
            //1.
            // adc_cmp_flag = 0;
            // float current[3];
            // current[0] = current_rawValue[0]*3.3/4096;
            // current[1] = current_rawValue[1]*3.3/4096;
            // current[2] = current_rawValue[2]*3.3/4096;
            //
            // UART_SendDataFloat(current, 3);
            //2.
            for (int i = 0; i < ADC_BUFFER_SIZE; i++)
            {
                printf("v:%f\n", v[i]*3.3/4096);
            }
            adc_cmp_flag = 0;
            // printf("v1:%f\n", (float)adc1_value * 3.3 /4096);
        }
        // for (int i = 0; i < ADC_BUFFER_SIZE; i++)
        // {
        //     printf("v:%f\n", (float)v[i] *3.3/4096);
        // }
    }

}

void test_main_adc2(void)
{
    PWM6_Init(10000, 4096);
    __HAL_ADC_ENABLE_IT(&hadc1, ADC_IT_JEOC);
    HAL_ADCEx_InjectedStart_IT(&hadc1);
    while (1)
    {

        if ( adc_cmp_flag == 1)
        {
            for (int i = 0; i < ADC_BUFFER_SIZE; i++)
            {
                printf("v:%f\n", (float)v[i]*3.3/4096);
                // printf("v:%d\n", v[i]);
            // UART_SendDataFloat(&v[i], 1);
            }
            adc_cmp_flag = 0;

            // printf("v1:%f\n", (float)adc1_value * 3.3 /4096);
        }
    }

}
void test_main_adc3_current(void)
{
    PIDController pid_a;
    pid_a.P = 0.7f;
    pid_a.I = 0.3f;
    pid_a.D = 0.0f;
    MotorFOC_InitADC(&motor, pid_a, ControlType_velocityClosedLoop);
    delay_ms(1000);
    LowPassFilter_t lpf_a;
    LowPassFilter_t lpf_b;
    LowPassFilter_init(&lpf_a, 0.05);
    LowPassFilter_init(&lpf_b, 0.05);


    while (1)
    {

        if (Key_Scan(0) == KEY_ON)
        {
            motor.isEnable = 1;
        }
        else if (Key_Scan(1) == KEY_ON)
        {
            motor.isEnable = 0;
        }
        if (motor.isEnable)
        {
            BLDCMotor_enable(&motor);
            BLDCMotor_move(&motor, 5);
            // UART_SendFloat(4, motor.target, motor.velocity, motor.angle, motor.e_angle);

        }
        else
        {
            BLDCMotor_disable(&motor);
        }

        UART_SendFloat(4,motor.currentSense->i_d,motor.currentSense->i_q, motor.currentSense->i_a, motor.currentSense->i_b);
        if (currentSense.adc_flag == 1)
        {
            //1.
            // float i_a, i_b, i_alpha, i_beta, i_q, i_d;
            // i_a = motor.currentSense->i_a;
            // i_b = motor.currentSense->i_b;
            // Clarke_Transform(i_a, i_b, &i_alpha, &i_beta);
            // Park_Transform(i_alpha, i_beta, _normalizeAngle(motor.e_angle), &i_d, &i_q);
            // motor.foc_motor.i_d = i_d;
            // motor.foc_motor.i_q = i_q;
            // i_d = LowPassFilter(&lpf_a, i_d);
            // i_q = LowPassFilter(&lpf_a, i_q);
            // // UART_SendFloat(6,i_d, i_q, currentSense.i_a, currentSense.i_b, i_beta, i_alpha);
            // UART_SendFloat(6,i_d, i_q, currentSense.i_a, currentSense.i_b, i_beta, i_alpha);

            //2.
            // UART_SendFloat(5,motor.target,motor.velocity, currentSense.i_a, currentSense.i_b, currentSense.i_c);
            // printf("%f,%f,%f\n", currentSense.i_a, currentSense.i_b, currentSense.i_c);
            //3.
            // BLDCMotor_getCurrentDQ_noFilter(&motor);
            currentSense.adc_flag = 0;

        }

    }

}

void test_main_currentLoop(void)
{
    PIDController pid_i_q;
    pid_i_q.P = 50.0f;
    pid_i_q.I = 20.0f;
    pid_i_q.D = 0.0f;
    PIDController pid_i_d;
    pid_i_d.P = 2.0f;
    pid_i_d.I = 0.5f;
    pid_i_d.D = 0.0f;
    // MotorFOC_InitADC(&motor, pid_i_q, ControlType_currentClosedLoop);
    MotorFOC_InitCurrentLoopPID(&motor, pid_i_d, pid_i_q, ControlType_currentClosedLoop);
    // printf("pid:%f, %f, %f\n", motor.foc_motor.PID_i_q.P, motor.foc_motor.PID_i_q.I, motor.foc_motor.PID_i_q.D);
    delay_ms(1000);
    LowPassFilter_t lpf_a;
    LowPassFilter_t lpf_b;
    LowPassFilter_init(&lpf_a, 0.05);
    LowPassFilter_init(&lpf_b, 0.05);

    while (1)
    {

        if (Key_Scan(0) == KEY_ON)
        {
            motor.isEnable = 1;
        }
        else if (Key_Scan(1) == KEY_ON)
        {
            motor.isEnable = 0;
        }
        if (motor.isEnable)
        {
            BLDCMotor_enable(&motor);
            if (currentSense.adc_flag == 1)
            {
                BLDCMotor_move(&motor, 0.06f);
                // UART_SendFloat(6,motor.target, motor.velocity,motor.currentSense->i_d,motor.currentSense->i_q, motor.currentSense->i_a, motor.currentSense->i_b);
                UART_SendFloat(6,motor.target,motor.velocity, motor.currentSense->i_d, motor.currentSense->i_q, motor.foc_motor.voltage_d, motor.foc_motor.voltage_q);
                currentSense.adc_flag = 0;

            }
            // UART_SendFloat(4, motor.target, motor.velocity, motor.angle, motor.e_angle);

        }
        else
        {
            BLDCMotor_disable(&motor);
        }

        {

        }

    }

}
// void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc)
// // void HAL_ADCEx_InjectedConvCallback(ADC_HandleTypeDef *hadc)
// {
//     if (hadc->Instance == ADC1)
//     {
//         //1.
//         static  int i = 0;
//         HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
//         if (adc_cmp_flag == 0)
//         {
//             v[i++] = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
//             if (i == ADC_BUFFER_SIZE)
//             {
//                 adc_cmp_flag = 1;
//                 i = 0;
//             }
//
//         }
//
//         //2.
//         // current_rawValue[0] = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
//         // current_rawValue[1] = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);
//         // current_rawValue[2] = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3);
//         // adc_cmp_flag = 1;
//         //3.
//
//
//     }
// }
void test_main(void)
{
    // test_main_pwm();
    // test_main_adc1();
    // test_main_adc2();
    //1.0ok
    // test_main_adc3_current();
    //1.
    // test_main_motor_velocityLoop();
    //2.as5600
    // as5600_example_0();
    // test_main_adc();
    //3.
    // test_main_openloop_with_current();
    //4. ok
    // test_main_motor_velocityLoop();
    //5.
    // test_main_as5600();
    // test_main_encoder();
    //6.
    // test_electrical_angle();
    //7.
    // test_main_motor_velocityLoop_with_current();
    //8.
    test_main_currentLoop();

}