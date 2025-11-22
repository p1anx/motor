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
#include <stdio.h>

#include <string.h>

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

extern BLDCMotor_t motor;
extern CurrentSense_t currentSense;
extern AS5600_t as5600;
extern Encoder_t encoder;

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
        // 1.
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
        // 1.
        currentSense_get3PhaseVoltage(&currentSense);
        printf("i:%f,%f,%f\n", currentSense.i_a, currentSense.i_b, currentSense.i_c);
        // 2.
        //  CurrentSense_Scan(&currentSense, 1, &current);
        //  printf("i: %f\n", (float)current/4096*3.3);
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
        // 1.
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
        // 1.
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
            // 1.
            //  printf("i:%f, %f, %f, %f\n",motor.velocity, currentSense.i_a, currentSense.i_b, currentSense.i_c);
            //  currentSense.i_a = LowPassFilter(&lpf_a, currentSense.i_a);
            //  currentSense.i_b = LowPassFilter(&lpf_b, currentSense.i_c);
            // 2.
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
    // 1.
    MotorFOC_Init(&motor, pid_a, ControlType_velocityClosedLoop);
    // 2.
    //  MotorFOC_Init(&motor, pid_a, ControlType_velocity_openloop);
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
        // 1.
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
                printf("v:%f, %f, %f,%f,%f\n", motor.target, motor.velocity, motor.foc_motor.voltage_q, motor.angle, motor.e_angle);
            }
            for (i = 0; i < t; i++)
            {
                BLDCMotor_move(&motor, 2);
                printf("v:%f, %f, %f,%f,%f\n", motor.target, motor.velocity, motor.foc_motor.voltage_q, motor.angle, motor.e_angle);
            }
            for (i = 0; i < t; i++)
            {
                BLDCMotor_move(&motor, 3);
                printf("v:%f, %f, %f,%f,%f\n", motor.target, motor.velocity, motor.foc_motor.voltage_q, motor.angle, motor.e_angle);
            }
            for (i = 0; i < t; i++)
            {
                BLDCMotor_move(&motor, 4);
                printf("v:%f, %f, %f,%f,%f\n", motor.target, motor.velocity, motor.foc_motor.voltage_q, motor.angle, motor.e_angle);
            }
            /*last_t = current_t;
            }*/
        }
        else
        {
            BLDCMotor_disable(&motor);
            // currentSense_get3PhaseVoltage(&currentSense);
            // 1.
            // printf("v:%f, %f\n", motor.target, motor.velocity);
            // 2.
        }
        // 1.
        //  printf("v:%f, %f, %f,%f,%f\n", motor.target, motor.velocity, motor.foc_motor.voltage_q,motor.angle, motor.e_angle);
        // 2.
        //  currentSense_get3PhaseCurrent(&currentSense);
        //  printf("i:%f, %f, %f\n", currentSense.i_a, currentSense.i_b, currentSense.i_c);
        // 3.
        //  currentSense_get3PhaseCurrent(&currentSense);
        //  i_list[0] = currentSense.i_a;
        //  i_list[1] = currentSense.i_b;
        //  i_list[2] = currentSense.i_c;
        //  UART_SendDataFloat(i_list, 8);
        // 4.
        //  buffer[0] = motor.target;
        //  buffer[1] = motor.velocity;
        //  UART_SendDataFloat(buffer, 2);
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
        float v = AS5600_getVelocity(&as5600);
        float angle = AS5600_getAngle(&as5600);
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
        float v = BLDCMotor_getVelocity(&motor);
        // float angle =  BLDCMotor_getAngle(&motor);
        float angle = motor.foc_motor.encoder->as5600_Instance.angle;
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
    while (1)
    {
        float mech_angle = BLDCMotor_getAngle(&motor);
        float elec_angle = _electricalAngle(mech_angle, 7);

        printf("Mechanical: %.2f rad (%.1f°), Electrical: %.2f rad (%.1f°)\n", mech_angle, mech_angle * 180 / _PI, elec_angle, elec_angle * 180 / _PI);

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
    // 1.
    MotorFOC_Init(&motor, pid_a, ControlType_velocityClosedLoop);
    // 2.
    //  MotorFOC_Init(&motor, pid_a, ControlType_velocity_openloop);
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
        // 1.
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
            // 1.
            // printf("v:%f, %f\n", motor.target, motor.velocity);
            // 2.
        }
        // 1.
        //  printf("v:%f, %f, %f,%f,%f\n", motor.target, motor.velocity, motor.foc_motor.voltage_q,motor.angle, motor.e_angle);
        // 2.
        currentSense.i_a = LowPassFilter(&lpf_a, currentSense.i_a);
        currentSense.i_b = LowPassFilter(&lpf_b, currentSense.i_b);

        // printf("i:%f, %f, %f\n", currentSense.i_a, currentSense.i_b, currentSense.i_c);
        // buffer[0] = motor.target;
        // buffer[1] = motor.velocity;
        // buffer[2] = currentSense.i_a;
        // buffer[3] = currentSense.i_b;
        // buffer[4] = currentSense.i_c;
        // UART_SendDataFloat(buffer, 5);

        // 5.
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
    // 1.
    //  PWM6_Init(2000, 1000);
    //  HAL_TIM_Base_Start(&htim2);
    //  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc1_value, 1);

    // 2.
    PWM6_Init(10000, 4096);
    // __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 998);
    //
    // HAL_ADCEx_InjectedStart(&hadc1);
    // HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&v, ADC_BUFFER_SIZE);

    // HAL_TIM_Base_Start(&htim2);
    // PWM6_Init(2000, 1000);
    //
    // 3.
    // HAL_TIM_Base_Start_IT(&pwm_tim);
    // HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc1_value, 1);
    // HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&v, ADC_BUFFER_SIZE);
    // 4.

    __HAL_ADC_ENABLE_IT(&hadc1, ADC_IT_JEOC);
    HAL_ADCEx_InjectedStart_IT(&hadc1);
    while (1)
    {

        if (adc_cmp_flag == 1)
        {
            // 1.
            //  adc_cmp_flag = 0;
            //  float current[3];
            //  current[0] = current_rawValue[0]*3.3/4096;
            //  current[1] = current_rawValue[1]*3.3/4096;
            //  current[2] = current_rawValue[2]*3.3/4096;
            //
            //  UART_SendDataFloat(current, 3);
            // 2.
            for (int i = 0; i < ADC_BUFFER_SIZE; i++)
            {
                printf("v:%f\n", v[i] * 3.3 / 4096);
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

        if (adc_cmp_flag == 1)
        {
            for (int i = 0; i < ADC_BUFFER_SIZE; i++)
            {
                printf("v:%f\n", (float)v[i] * 3.3 / 4096);
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

        // UART_SendFloat(4,motor.currentSense->i_d,motor.currentSense->i_q, motor.currentSense->i_a, motor.currentSense->i_b);
        if (currentSense.adc_flag == 1)
        {
            // 1.
            //  float i_a, i_b, i_alpha, i_beta, i_q, i_d;
            //  i_a = motor.currentSense->i_a;
            //  i_b = motor.currentSense->i_b;
            //  Clarke_Transform(i_a, i_b, &i_alpha, &i_beta);
            //  Park_Transform(i_alpha, i_beta, _normalizeAngle(motor.e_angle), &i_d, &i_q);
            //  motor.foc_motor.i_d = i_d;
            //  motor.foc_motor.i_q = i_q;
            //  i_d = LowPassFilter(&lpf_a, i_d);
            //  i_q = LowPassFilter(&lpf_a, i_q);
            //  // UART_SendFloat(6,i_d, i_q, currentSense.i_a, currentSense.i_b, i_beta, i_alpha);
            //  UART_SendFloat(6,i_d, i_q, currentSense.i_a, currentSense.i_b, i_beta, i_alpha);

            // 2.
            UART_SendFloat(5, motor.target, motor.velocity, currentSense.i_a, currentSense.i_b, currentSense.i_c);
            // printf("%f,%f,%f\n", currentSense.i_a, currentSense.i_b, currentSense.i_c);
            // 3.
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
                BLDCMotor_move(&motor, 0.02f);
                // UART_SendFloat(6,motor.target, motor.velocity,motor.currentSense->i_d,motor.currentSense->i_q, motor.currentSense->i_a, motor.currentSense->i_b);
                UART_SendFloat(6, motor.target, motor.velocity, motor.currentSense->i_d, motor.currentSense->i_q, motor.foc_motor.voltage_d, motor.foc_motor.voltage_q);
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
void test_encoder(void)
{
    Encoder_t encoder0;
    // MT6835_Init(&encoder0.mt6835, &mt6835_spi, MT6835_SPI_CS_PORT, MT6835_SPI_CS_PIN);
    // 2.
    encoder0.EncoderType = EncoderType_MT6835;
    Encoder_init0(&encoder0);
    // 3.

    while (1)
    {
        float angle = Encoder_getAngle(&encoder0);
        printf("angle = %f\n", encoder0.angle);
    }
}
void test_motor_encoder(void)
{
    BLDCMotor_initAll(&motor, 7, EncoderType_MT6835, ControlType_velocity_openloop);
    BLDCMotor_enable(&motor);
    while (1)
    {
        BLDCMotor_move(&motor, 1);
    }
}
void test_motor_pid_velocity_ok(void)
{
    // BLDCMotor_initAll(&motor, 7, EncoderType_MT6835, ControlType_velocity_openloop);
    PIDController pid_velocity;
    pid_velocity.P = 0.8f;
    pid_velocity.I = 0.2f;
    pid_velocity.D = 0.0f;
    // BLDCMotor_initPID(&motor, 7, EncoderType_MT6835, ControlType_velocityClosedLoop, pid_velocity);
    BLDCMotor_initPID(&motor, 7, EncoderType_AS5600, ControlType_velocityClosedLoop, pid_velocity);
    delay_ms(2000);
    printf("waiting to start motor....\n");
    BLDCMotor_disable(&motor);

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
                BLDCMotor_move(&motor, 3.0f);
                // UART_SendFloat(6,motor.target,motor.velocity, motor.currentSense->i_d, motor.currentSense->i_q, motor.foc_motor.voltage_d, motor.foc_motor.voltage_q);
                UART_SendFloat(6, motor.target, motor.velocity, motor.foc_motor.voltage_q, (float)motor.direction, motor.angle, motor.e_angle);
                currentSense.adc_flag = 0;
            }
        }
        else
        {
            BLDCMotor_disable(&motor);
        }
    }
}
void test_motor_driver(void)
{
    BLDCDriver_init(&motor.Driver, 10e3, 4096, 15, 15);
    BLDCDriver_setPhaseVoltage(&motor.Driver, 0.5f, 0, _3PI_2);
}
void test_motor_pwm(void)
{
    // BLDCMotor_initAll(&motor, 7, EncoderType_MT6835, ControlType_velocity_openloop);
    PIDController pid_velocity;
    pid_velocity.P = 0.8f;
    pid_velocity.I = 0.2f;
    pid_velocity.D = 0.0f;
    // BLDCMotor_initPID(&motor, 7, EncoderType_MT6835, ControlType_velocityClosedLoop, pid_velocity);
    BLDCMotor_initPID(&motor, 7, EncoderType_AS5600, ControlType_velocityClosedLoop, pid_velocity);
    delay_ms(2000);
    printf("waiting to start motor....\n");
    BLDCMotor_setPhaseVoltage(&motor, 4.0f, 0, _3PI_2);
    // BLDCMotor_disable(&motor);

    int last_t;
    while (1)
    {
    }
}
void test_motor_pid_currentloop(void)
{
    // BLDCMotor_initAll(&motor, 7, EncoderType_MT6835, ControlType_velocity_openloop);
    PIDController pid_id;
    pid_id.P = 40.0f;
    pid_id.I = 20.0f;
    pid_id.D = 0.0f;
    PIDController pid_iq;
    pid_iq.P = 60.0f;
    pid_iq.I = 30.0f;
    pid_iq.D = 0.0f;
    // BLDCMotor_initPID(&motor, 7, EncoderType_MT6835, ControlType_velocityClosedLoop, pid_velocity);
    BLDCMotor_initPID_CurrentDQ(&motor, 7, EncoderType_AS5600, ControlType_currentClosedLoop, pid_id, pid_iq);
    delay_ms(1000);
    printf("waiting to start motor....\n");
    // BLDCMotor_setPhaseVoltage(&motor, 2.0f, 0, _3PI_2);
    BLDCMotor_disable(&motor);

    int last_t;
    while (1)
    {
        if (Key_Scan(0) == KEY_ON)
        {
            motor.isEnable = 1;
            last_t = getUs();
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
                BLDCMotor_move(&motor, 0.4f);
                int now_t = getUs();
                float delta_t = (float)(now_t - last_t) / 1e3;
                last_t = now_t;
                // UART_SendFloat(1,delta_t);
                UART_SendFloat(7, motor.target, motor.currentSense->i_a, motor.currentSense->i_b, motor.currentSense->i_q, motor.currentSense->i_d, motor.foc_motor.voltage_d,
                               motor.foc_motor.voltage_q);
                currentSense.adc_flag = 0;
            }
        }
        else
        {
            BLDCMotor_disable(&motor);
        }
    }
}
void test_motor_pid_currentVelocityLoop(void)
{
    // BLDCMotor_initAll(&motor, 7, EncoderType_MT6835, ControlType_velocity_openloop);
    PIDController pid_id;
    pid_id.P = 40.0f;
    pid_id.I = 20.0f;
    pid_id.D = 0.0f;
    PIDController pid_iq;
    pid_iq.P = 60.0f;
    pid_iq.I = 30.0f;
    pid_iq.D = 0.0f;
    PIDController pid_velocity;
    pid_velocity.P = 0.35f;
    pid_velocity.I = 0.3f;
    pid_velocity.D = 0.0f;
    // BLDCMotor_initPID(&motor, 7, EncoderType_MT6835, ControlType_velocityClosedLoop, pid_velocity);
    BLDCMotor_initPID_CurrentVelocity(&motor, 7, EncoderType_AS5600, ControlType_currentVelocityClosedLoop, pid_id, pid_iq, pid_velocity);
    init_multirate_controller(&MultiRate_Controller, CONFIG_PWM_HZ/5, 1000.0f, 1000.0f);
    delay_ms(1000);
    printf("waiting to start motor....\n");
    // BLDCMotor_setPhaseVoltage(&motor, 2.0f, 0, _3PI_2);
    BLDCMotor_disable(&motor);
    SEGGER_RTT_Config();

    g_motorVar0.target  = 1;
    g_motorVar0.velocity = 2;
    // g_motorVar0.ia = 3;
    // g_motorVar0.ib = 4;
    int last_t;
    // while (1)
    // {
    //     // g_motorVar0.target  += 1;
    //     // g_motorVar0.velocity += motor.velocity;
    //     // g_motorVar0.ia = motor.currentSense->i_a;
    //     // g_motorVar0.ib = motor.currentSense->i_b;
    //     g_motorVar0.target  += 1;
    //     g_motorVar0.velocity += 2;
    //     g_motorVar0.ia += 3;
    //     g_motorVar0.ib += 4;
    //     if (g_motorVar0.target > 1000 || g_motorVar0.target > 1000 || g_motorVar0.ia > 1000 || g_motorVar0.ib > 1000)
    //     {
    //         g_motorVar0.target = 0;
    //         g_motorVar0.velocity = 0;
    //         g_motorVar0.ia = 0;
    //         g_motorVar0.ib = 0;
    //
    //     }
    //     SEGGER_RTT_Write(1, &g_motorVar0, sizeof(g_motorVar0));
    //     HAL_Delay(10);
    //
    // }
    motor.target = 8;
    while (1)
    {
        if (Key_Scan(0) == KEY_ON)
        {
            motor.isEnable = 1;
            last_t = getUs();
        }
        else if (Key_Scan(1) == KEY_ON)
        {
            motor.isEnable = 0;
        }
        if (motor.isEnable)
        {
            BLDCMotor_enable(&motor);
            // if (currentSense.adc_flag == 1)
            if (MultiRate_Controller.current_enable)
            {
                BLDCMotor_move(&motor, motor.target);
                int now_t = getUs();
                float delta_t = (float)(now_t - last_t) / 1e3;
                last_t = now_t;

                // UART_SendFloat(1,delta_t);
                // UART_SendFloat(6, motor.target, motor.velocity, motor.currentSense->i_q, motor.currentSense->i_d, motor.foc_motor.voltage_d, motor.foc_motor.voltage_q);

                //2.
                // UART_SendFloat(6, motor.target, motor.velocity, motor.currentSense->i_a, motor.currentSense->i_b, motor.foc_motor.voltage_d, motor.foc_motor.voltage_q);

                // currentSense.adc_flag = 0;
            }
        }
        else
        {
            BLDCMotor_disable(&motor);
        }
        // char* str = "hello\n";
        // SEGGER_RTT_printf(0, str);
    }
}
void test_motor_pid_VelocityAngleLoop(void)
{
    // BLDCMotor_initAll(&motor, 7, EncoderType_MT6835, ControlType_velocity_openloop);
    PIDController pid_id;
    pid_id.P = 40.0f;
    pid_id.I = 20.0f;
    pid_id.D = 0.0f;
    PIDController pid_iq;
    pid_iq.P = 60.0f;
    pid_iq.I = 30.0f;
    pid_iq.D = 0.0f;
    PIDController pid_velocity;
    pid_velocity.P = 2.0f;
    pid_velocity.I = 0.00f;
    pid_velocity.D = 0.0f;
    PIDController pid_degree;
    pid_degree.P = 0.2f;
    pid_degree.I = 0.0f;
    pid_degree.D = 0.0f;
    // BLDCMotor_initPID(&motor, 7, EncoderType_MT6835, ControlType_velocityClosedLoop, pid_velocity);
    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_AS5600, ControlType_currentVelocityAngleClosedLoop, pid_id, pid_iq, pid_velocity, pid_degree);
    BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_AS5600, ControlType_angleClosedLoop, pid_id, pid_iq, pid_velocity, pid_degree);
    delay_ms(1000);
    printf("waiting to start motor....\n");
    // BLDCMotor_setPhaseVoltage(&motor, 2.0f, 0, _3PI_2);
    BLDCMotor_disable(&motor);

    int last_t;
    while (1)
    {
        if (Key_Scan(0) == KEY_ON)
        {
            motor.isEnable = 1;
            last_t = getUs();
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
                int now_t = getUs();
                static int target_i = 0;
                float delta_t = (float)(now_t - last_t) / 1e3;
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
                    BLDCMotor_move(&motor, 90.0f);
                    break;
                case 1:
                    BLDCMotor_move(&motor, 180.0f);
                    break;
                case 2:
                    BLDCMotor_move(&motor, 270.0f);
                    break;
                // case 3:
                //     BLDCMotor_move(&motor, 359.0f);
                //     break;
                default:
                    BLDCMotor_move(&motor, 90.0f);
                    break;
                }

                // UART_SendFloat(1,delta_t);
                UART_SendFloat(5, motor.target, motor.degree, motor.velocity, motor.foc_motor.voltage_d, motor.foc_motor.voltage_q);
                currentSense.adc_flag = 0;
            }
        }
        else
        {
            BLDCMotor_disable(&motor);
        }
    }
}
void test_motor_pid_currentVelocityAngleLoop(void)
{
    // BLDCMotor_initAll(&motor, 7, EncoderType_MT6835, ControlType_velocity_openloop);
    PIDController pid_id;
    pid_id.P = 40.0f;
    pid_id.I = 20.0f;
    pid_id.D = 0.0f;
    PIDController pid_iq;
    pid_iq.P = 60.0f;
    pid_iq.I = 30.0f;
    pid_iq.D = 0.0f;
    PIDController pid_velocity;
    pid_velocity.P = 0.35f;
    pid_velocity.I = 0.3f;
    pid_velocity.D = 0.0f;
    PIDController pid_degree;
    pid_degree.P = 20.0f;
    pid_degree.I = 1.0f;
    pid_degree.D = 0.0f;
    // BLDCMotor_initPID(&motor, 7, EncoderType_MT6835, ControlType_velocityClosedLoop, pid_velocity);
    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_AS5600, ControlType_currentVelocityAngleClosedLoop, pid_id, pid_iq, pid_velocity, pid_degree);
    BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_AS5600, ControlType_angleClosedLoop, pid_id, pid_iq, pid_velocity, pid_degree);
    delay_ms(1000);
    printf("waiting to start motor....\n");
    // BLDCMotor_setPhaseVoltage(&motor, 2.0f, 0, _3PI_2);
    BLDCMotor_disable(&motor);

    int last_t;
    while (1)
    {
        if (Key_Scan(0) == KEY_ON)
        {
            motor.isEnable = 1;
            last_t = getUs();
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
                BLDCMotor_move(&motor, 90.0f);
                int now_t = getUs();
                float delta_t = (float)(now_t - last_t) / 1e3;
                last_t = now_t;
                // UART_SendFloat(1,delta_t);
                UART_SendFloat(5, motor.target, motor.degree, motor.velocity, motor.foc_motor.voltage_d, motor.foc_motor.voltage_q);
                currentSense.adc_flag = 0;
            }
        }
        else
        {
            BLDCMotor_disable(&motor);
        }
    }
}
void test_motor_pid_currentloop_bandwidth(void)
{
    SEGGER_RTT_Config();
    unsigned int js_t = 0;
    // while (1)
    // {
    //     js_t++;
    //     g_motorVar0.t = js_t;
    //     // g_motorVar0.target = motor.target;
    //     // g_motorVar0.velocity = motor.degree;
    //     g_motorVar0.target = 1;
    //     g_motorVar0.velocity = 2;
    //     SEGGER_RTT_Write(1, &g_motorVar0, sizeof(g_motorVar0));
    //
    // }
    // BLDCMotor_initAll(&motor, 7, EncoderType_MT6835, ControlType_velocity_openloop);
    PIDController pid_id;
    // pid_id.P = 40.0f;
    // pid_id.I = 20.0f;
    // pid_id.D = 0.0f;
    pid_id.P = 60.0f;
    pid_id.I = 20.0f;
    pid_id.D = 0.0f;
    PIDController pid_iq;
    // pid_iq.P = 60.0f;
    // pid_iq.I = 30.0f;
    // pid_iq.D = 0.0f;
    pid_iq.P = 60.0f;
    pid_iq.I = 30.0f;
    pid_iq.D = 0.0f;
    PIDController pid_velocity;
    pid_velocity.P = 0.06f;
    pid_velocity.I = 0.00f;//0.05
    pid_velocity.D = 0.0f;
    PIDController pid_degree;
    pid_degree.P = 0.02f;
    pid_degree.I = 0.01f;
    pid_degree.D = 0.0f;
    // BLDCMotor_initPID(&motor, 7, EncoderType_MT6835, ControlType_velocityClosedLoop, pid_velocity);
    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_AS5600, ControlType_currentVelocityAngleClosedLoop, pid_id, pid_iq, pid_velocity, pid_degree);
    BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_AS5600, ControlType_currentClosedLoop, pid_id, pid_iq, pid_velocity, pid_degree);
    //2.
    // PIDController pid_id;
    // pid_id.P = 40.0f;
    // pid_id.I = 20.0f;
    // pid_id.D = 0.0f;
    // PIDController pid_iq;
    // pid_iq.P = 60.0f;
    // pid_iq.I = 30.0f;
    // pid_iq.D = 0.0f;
    // // BLDCMotor_initPID(&motor, 7, EncoderType_MT6835, ControlType_velocityClosedLoop, pid_velocity);
    // BLDCMotor_initPID_CurrentDQ(&motor, 7, EncoderType_AS5600, ControlType_currentClosedLoop, pid_id, pid_iq);

    init_multirate_controller(&MultiRate_Controller, CONFIG_PWM_HZ/5, 1000.0f, 1000.0f);
    delay_ms(1000);
    printf("waiting to start motor....\n");
    // BLDCMotor_setPhaseVoltage(&motor, 2.0f, 0, _3PI_2);
    BLDCMotor_disable(&motor);
    const int BUFFER_SIZE = 1000;
    float ia_buffer[BUFFER_SIZE];
    float ib_buffer[BUFFER_SIZE];
    int i =  0;
    int tx_flag = 0;

    int last_t;
    while (1)
    {
        if (Key_Scan(0) == KEY_ON)
        {
            motor.isEnable = 1;
            last_t = getUs();
        }
        else if (Key_Scan(1) == KEY_ON)
        {
            motor.isEnable = 0;
        }
        if (motor.isEnable)
        {
            BLDCMotor_enable(&motor);
            // BLDCMotor_move(&motor, 90.2f);
            int now_t = getUs();
            static int target_i = 0;
            float delta_t = (float)(now_t - last_t) / 1e3;
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
                BLDCMotor_move(&motor, 90.0f);
                break;
            case 1:
                BLDCMotor_move(&motor, 180.0f);
                break;
            case 2:
                BLDCMotor_move(&motor, 270.0f);
                break;
                // case 3:
                //     BLDCMotor_move(&motor, 359.0f);
                //     break;
            default:
                BLDCMotor_move(&motor, 90.0f);
                break;
            }
            // js_t++;
            // g_motorVar0.t = js_t;
            // g_motorVar0.target = motor.target;
            // g_motorVar0.velocity = motor.degree;
            // SEGGER_RTT_Write(1, &g_motorVar0, sizeof(g_motorVar0));
        }
        else
        {
            BLDCMotor_disable(&motor);
        }
        // while (1)
        // {
        // js_t++;
        // g_motorVar0.t = js_t;
        // g_motorVar0.target = motor.target;
        // g_motorVar0.velocity = motor.degree;
        // SEGGER_RTT_Write(1, &g_motorVar0, sizeof(g_motorVar0));
        //
        // }
    }
}
void test_motor_pid_currentloopBandwidth(void)
{
    unsigned int js_t = 0;
    PIDController pid_id;
    //1. ok
    // pid_id.P = 60.0f;
    // pid_id.I = 30.0f;
    // pid_id.D = 0.0f;
    pid_id.P = 20.0f;
    pid_id.I = 10.0f;
    pid_id.D = 0.0f;
    PIDController pid_iq;
    //2. ok
    // pid_iq.P = 60.0f;
    // pid_iq.I = 30.0f;
    // pid_iq.D = 0.0f;
    pid_iq.P = 10.0f;
    pid_iq.I = 5.0f;
    pid_iq.D = 0.0f;

    PIDController pid_velocity;
    pid_velocity.P = 0.06f;
    pid_velocity.I = 0.00f;//0.05
    pid_velocity.D = 0.0f;
    PIDController pid_degree;
    pid_degree.P = 0.02f;
    pid_degree.I = 0.01f;
    pid_degree.D = 0.0f;
    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_AS5600, ControlType_currentClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);
    BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7,EncoderType_MT6835, ControlType_currentClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);

    delay_ms(1000);
    printf("waiting to start motor....\n");
    BLDCMotor_disable(&motor);
    motor.target = 0.1f;
    // float t_buffer[100];
    // int t_i = 0;
    char buffer[100];
    int last_t = HAL_GetTick();
    while (1)
    {
        // if (Key_Scan(0) == KEY_ON)
        // {
        //     motor.isEnable = 1;
        // }
        // else if (Key_Scan(1) == KEY_ON)
        // {
        //     motor.isEnable = 0;
        // }
        BLDCMotor_ControlRunByKey(&motor);
        if (motor.isEnable)
        {
            BLDCMotor_enable(&motor);
            int now_t = HAL_GetTick();
            if (now_t - last_t > 1000) {
                motor.target += 0.1f;
                last_t = now_t;
            }
            if (motor.target > 0.2) {
                motor.target = 0.0f;
            }
            BLDCMotor_move(&motor, motor.target);
        }
        else
        {
            BLDCMotor_disable(&motor);
        }
    }
}
void test_motor_pid_openLoopBandwidth(void)
{
    SEGGER_RTT_Config();
    unsigned int js_t = 0;
    PIDController pid_id;
    //1. ok
    // pid_id.P = 60.0f;
    // pid_id.I = 20.0f;
    // pid_id.D = 0.0f;
    pid_id.P = 60.0f;
    pid_id.I = 30.0f;
    pid_id.D = 0.0f;
    PIDController pid_iq;
    //2. ok
    // pid_iq.P = 60.0f;
    // pid_iq.I = 30.0f;
    // pid_iq.D = 0.0f;
    pid_iq.P = 30.0f;
    pid_iq.I = 10.0f;
    pid_iq.D = 0.0f;

    PIDController pid_velocity;
    pid_velocity.P = 0.06f;
    pid_velocity.I = 0.00f;//0.05
    pid_velocity.D = 0.0f;
    PIDController pid_degree;
    pid_degree.P = 0.02f;
    pid_degree.I = 0.01f;
    pid_degree.D = 0.0f;
    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_AS5600, ControlType_currentClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);
    // PWM3_Init(CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION);
    BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7,EncoderType_MT6835, ControlType_velocity_openloop, pid_id, pid_iq, pid_velocity, pid_degree);

    delay_ms(1000);
    printf("waiting to start motor....\n");
    BLDCMotor_disable(&motor);
    // BLDCMotor_SVPWM(&motor, 6, 0,0);
    // int arr = __HAL_TIM_GET_AUTORELOAD(&pwm_tim);
    // int psc = __HAL_TIM_GET_COMPARE(&pwm_tim, TIM_CHANNEL_1);
    // printf("arr = %d, psc = %d\n", arr, psc);

    motor.target = 5.0f;
    int i = 0;
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
            // int t0 = getUs();
            // if (motor.currentSense->isReady)
            // {
            //     UART_SendFloat(2, motor.currentSense->i_a, motor.currentSense->i_b);
            //
            // }
            BLDCMotor_move(&motor, motor.target);
            // if (i++ > 2) {
            //     i = 0;
            //     printf("%f,%f\n", motor.CurrentSense.i_a, motor.CurrentSense.i_b);
            // }
            // HAL_Delay(1);


            //1.
            //2.
            // sprintf(buffer, "%.3f,%.3f\n", motor.currentSense->i_a, motor.currentSense->i_b);
            // if (HAL_UART_GetState(&print_uart) == HAL_UART_STATE_READY)
            // {
            //     HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)buffer, strlen(buffer));
            // }


            // multitarget_current_move(&motor);
        }
        else
        {
            BLDCMotor_disable(&motor);
        }
    }
}
void test_motor_pid_currentVelocityLoopBandwidth(void)
{
    PIDController pid_id;
    //1. ok
    // pid_id.P = 20.0f;
    // pid_id.I = 10.0f;
    // pid_id.D = 0.0f;
    pid_id.P = 10.0f;
    pid_id.I = 0.0f;
    pid_id.D = 0.0f;
    PIDController pid_iq;
    //2. ok
    // pid_iq.P = 10.0f;
    // pid_iq.I = 5.0f;
    // pid_iq.D = 0.0f;
    pid_iq.P = 5.0f;
    pid_iq.I = 0.0f;
    pid_iq.D = 0.0f;

    PIDController pid_velocity;
    pid_velocity.P = 0.4f;
    pid_velocity.I = 0.3f;//0.05
    pid_velocity.D = 0.0f;
    PIDController pid_degree;
    pid_degree.P = 0.03f;
    pid_degree.I = 0.00f;
    pid_degree.D = 0.0f;
    BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_MT6835, ControlType_currentVelocityClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);

    delay_ms(1000);
    printf("waiting to start motor....\n");
    BLDCMotor_disable(&motor);
    motor.target = 5.0f;
    while (1)
    {
        BLDCMotor_ControlRunByKey(&motor);
        if (motor.isEnable)
        {
            BLDCMotor_enable(&motor);
            BLDCMotor_move(&motor, motor.target);
            // multitarget_current_move(&motor);
        }
        else
        {
            BLDCMotor_disable(&motor);
        }
    }
}
void test_motor_pid_currentVelocityRPMLoopBandwidth(void)
{
    SEGGER_RTT_Config();
    unsigned int js_t = 0;
    PIDController pid_id;
    // pid_id.P = 40.0f;
    // pid_id.I = 20.0f;
    // pid_id.D = 0.0f;
    pid_id.P = 60.0f;
    pid_id.I = 20.0f;
    pid_id.D = 0.0f;
    PIDController pid_iq;
    // pid_iq.P = 60.0f;
    // pid_iq.I = 30.0f;
    // pid_iq.D = 0.0f;
    pid_iq.P = 60.0f;
    pid_iq.I = 30.0f;
    pid_iq.D = 0.0f;
    PIDController pid_velocity;
    pid_velocity.P = 0.002f;
    pid_velocity.I = 0.001f;//0.05
    pid_velocity.D = 0.0f;
    // pid_velocity.P = 0.0002f;
    // pid_velocity.I = 0.000f;//0.05
    // pid_velocity.D = 0.0f;
    PIDController pid_degree;
    pid_degree.P = 0.02f;
    pid_degree.I = 0.01f;
    pid_degree.D = 0.0f;
    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_AS5600, ControlType_currentVelocityClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);
    BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_MT6835, ControlType_currentVelocityRPMClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);

    init_multirate_controller(&MultiRate_Controller, CONFIG_PWM_HZ/5, 1000.0f, 1000.0f);
    delay_ms(1000);
    printf("waiting to start motor....\n");
    BLDCMotor_disable(&motor);
    motor.target = 0.0f;
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
            BLDCMotor_move(&motor, motor.target);
            // multitarget_current_move(&motor);
        }
        else
        {
            BLDCMotor_disable(&motor);
        }
    }
}
void test_motor_pid_currentVelocityAngleLoopBandwidth(void)
{
    PIDController pid_id;
    //1. ok
    // pid_id.P = 20.0f;
    // pid_id.I = 10.0f;
    // pid_id.D = 0.0f;
    pid_id.P = 10.0f;
    pid_id.I = 0.0f;
    pid_id.D = 0.0f;
    PIDController pid_iq;
    //2. ok
    // pid_iq.P = 10.0f;
    // pid_iq.I = 5.0f;
    // pid_iq.D = 0.0f;
    pid_iq.P = 5.0f;
    pid_iq.I = 0.0f;
    pid_iq.D = 0.0f;

    PIDController pid_velocity;
    pid_velocity.P = 0.4f;
    pid_velocity.I = 0.1f;//0.05
    pid_velocity.D = 0.0f;
    PIDController pid_degree;
    pid_degree.P = 2.0f;
    pid_degree.I = 0.0f;
    pid_degree.D = 0.0f;
    BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_MT6835, ControlType_currentVelocityAngleClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);

    delay_ms(1000);
    printf("waiting to start motor....\n");
    BLDCMotor_disable(&motor);
    motor.target = _2PI/2;
    while (1)
    {
        BLDCMotor_ControlRunByKey(&motor);
        if (motor.isEnable)
        {
            BLDCMotor_enable(&motor);
            BLDCMotor_move(&motor, motor.target);
            // multitarget_degrees_move(&motor);
        }
        else
        {
            BLDCMotor_disable(&motor);
        }
    }
}
void test_motor_new_driver(void)
{
    unsigned int js_t = 0;
    PIDController pid_id;
    //1. ok
    // pid_id.P = 60.0f;
    // pid_id.I = 20.0f;
    // pid_id.D = 0.0f;
    pid_id.P = 60.0f;
    pid_id.I = 30.0f;
    pid_id.D = 0.0f;
    PIDController pid_iq;
    //2. ok
    // pid_iq.P = 60.0f;
    // pid_iq.I = 30.0f;
    // pid_iq.D = 0.0f;
    pid_iq.P = 30.0f;
    pid_iq.I = 10.0f;
    pid_iq.D = 0.0f;

    PIDController pid_velocity;
    pid_velocity.P = 0.06f;
    pid_velocity.I = 0.00f;//0.05
    pid_velocity.D = 0.0f;
    PIDController pid_degree;
    pid_degree.P = 0.02f;
    pid_degree.I = 0.01f;
    pid_degree.D = 0.0f;
    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_AS5600, ControlType_currentClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);
    // PWM3_Init(CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION);
    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7,EncoderType_MT6835, ControlType_currentVelocityOpenLoop, pid_id, pid_iq, pid_velocity, pid_degree);
    BLDCMotor_initPID_CurrentVelocityAngle_Inline(&motor, 7,EncoderType_MT6835, ControlType_currentVelocityOpenLoopInline, pid_id, pid_iq, pid_velocity, pid_degree);

    delay_ms(1000);
    printf("waiting to start motor....\n");
    BLDCMotor_disable(&motor);
    // BLDCMotor_SVPWM(&motor, 3, 0,_3PI_2);
    // int clk = SystemCoreClock;
    // printf("clk = %d\n", clk);

    int arr = __HAL_TIM_GET_AUTORELOAD(&pwm_tim);
    int psc = pwm_tim.Instance->PSC;
    printf("arr = %d, psc = %d\n", arr, psc);

    BLDCMotor_disable(&motor);
    motor.target = 1.0f;
    int i = 0;
    int last_t = getUs();
    HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));
    while (1)
    {
        BLDCMotor_ControlRunByKey(&motor);
        if (motor.isEnable) {
            BLDCMotor_enable(&motor);
            // // int t0 = getUs();
            // BLDCMotor_move(&motor, motor.target);
            // // int t1 = getUs();
            // // float delta_t = (float)(t1 - t0);
            // // UART_SendFloat(1, delta_t);
            int t0 = getUs();
            if (t0 - last_t > 1000) {
                BLDCMotor_move(&motor, motor.target);
                // UART_SendFloat(3, motor.Driver.Ua, motor.Driver.Ub, motor.Driver.Uc);
                // UART_SendFloat(2, motor.currentSense->i_a, motor.currentSense->i_b);
                // UART_SendFloat_DMA(2, motor.currentSense->i_a, motor.currentSense->i_b);
                // UART_SendFloat(2, motor.currentSense->i_d, motor.currentSense->i_q);
                //2
                if (TxCompleteFlag == 1) {
                    TxCompleteFlag = 0;
                    // sprintf(txDMA_buffer, "%.3f,%.3f\n", motor.currentSense->i_a, motor.currentSense->i_b);
                    sprintf(txDMA_buffer, "%.3f\n", motor.currentSense->i_q);
                    HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));
                }

                last_t = t0;
            }
        }
        else {
            BLDCMotor_disable(&motor);
        }
    }
}
void test_motor_new_driver_currentLoop(void)
{
    unsigned int js_t = 0;
    PIDController pid_id;
    //1. ok
    // pid_id.P = 60.0f;
    // pid_id.I = 20.0f;
    // pid_id.D = 0.0f;
    pid_id.P = 20.0f;
    pid_id.I = 10.0f;
    pid_id.D = 0.0f;
    PIDController pid_iq;
    //2. ok
    // pid_iq.P = 60.0f;
    // pid_iq.I = 30.0f;
    // pid_iq.D = 0.0f;
    pid_iq.P = 20.0f;
    pid_iq.I = 10.0f;
    pid_iq.D = 0.0f;

    PIDController pid_velocity;
    pid_velocity.P = 0.06f;
    pid_velocity.I = 0.00f;//0.05
    pid_velocity.D = 0.0f;
    PIDController pid_degree;
    pid_degree.P = 0.02f;
    pid_degree.I = 0.01f;
    pid_degree.D = 0.0f;
    // BLDCMotor_initPID_CurrentVelocityAngle(&motor, 7, EncoderType_AS5600, ControlType_currentClosedLoopBandwidth, pid_id, pid_iq, pid_velocity, pid_degree);
    // PWM3_Init(CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION);
    BLDCMotor_initPID_CurrentVelocityAngle_Inline(&motor, 7,EncoderType_MT6835, ControlType_currentClosedLoopBandwidthInline, pid_id, pid_iq, pid_velocity, pid_degree);

    delay_ms(1000);
    printf("waiting to start motor....\n");
    BLDCMotor_disable(&motor);


    motor.target = 0.05f;

    int i = 0;
    int last_t = getUs();
    while (1)
    {
        BLDCMotor_ControlRunByKey(&motor);
        if (motor.isEnable) {
            BLDCMotor_enable(&motor);
            int t0 = getUs();
            if (t0 - last_t > 1000) {
                BLDCMotor_move(&motor, motor.target);
                last_t = t0;
            }

            // HAL_Delay(1);
        }
        else {
            BLDCMotor_disable(&motor);
        }
    }
}

extern void test_uart(void);
extern void test_adc_inject(void);

extern void test_3pwm();
void test_main(void)
{

    // PWM3_Init(1000, 4096*2);
    // test_PWM3_Init(1000, 4096*2);
    // BLDCDriver_writePWM(0.1, 0.2,0.4);
    // HAL_UART_Transmit(&huart3, "hello uart\n", strlen("hello uart\n"), 1000);

    // test_3pwm();
    // test_key();
    // test_PWM(1000, 1000);
    // test_adc_inject();
    // test_uart();
    // test_main_pwm();
    // test_main_adc1();
    // test_main_adc2();
    // 1.0ok
    // test_main_adc3_current();
    // 1.
    // test_main_motor_velocityLoop();
    // 2.as5600
    // as5600_example_0();
    // test_main_adc();
    // 3.
    // test_main_openloop_with_current();
    // 4. ok
    // test_main_motor_velocityLoop();
    // 5.
    // test_main_as5600();
    // test_main_encoder();
    // 6.
    // test_electrical_angle();
    // 7.
    // test_main_motor_velocityLoop_with_current();
    // 8.
    // test_main_currentLoop();
    // 9.
    // test_mt6835();
    // test_encoder();
    // 10
    // test_motor_encoder();
    // test_motor_driver();
    // 11. ok
    // test_motor_pid_velocity_ok();
    // 12. ok
    // test_motor_pid_currentloop();
    // 13
    // test_motor_pwm();
    // 14.ok
    // test_motor_pid_currentVelocityLoop();
    // 15.
    // test_motor_pid_currentVelocityAngleLoop();
    // test_motor_pid_VelocityAngleLoop();
    //16.
    // test_motor_pid_currentloop_bandwidth();

    //17.ok -- test currentLoop
    // test_motor_pid_currentloopBandwidth();
    // test_motor_pid_openLoopBandwidth();
    //18.ok -- test velocityLoop
    // test_motor_pid_currentVelocityLoopBandwidth();
    // test_motor_pid_currentVelocityRPMLoopBandwidth();
    //19.
    // test_motor_pid_currentVelocityAngleLoopBandwidth();
    //======================================================
    // test_motor_new_driver();
    test_motor_new_driver_currentLoop();
}