//
// Created by xwj on 10/31/25.
//

#include "pwm.h"
#include "stm32_hal.h"
#include "BLDCMotor.h"
#include "ads1256.h"
#include <stdio.h>
#include "timer.h"
#include "currentSense.h"

BLDCMotor_t motor;
BLDCDriver_t driver;
FOCMotor_t FOCMotor;
Encoder_t encoder;
AS5600_t as5600;
CurrentSense_t currentSense;

PIDController pid;

void test_openloop_v(void)
{
    ADS1256_t ads1256;

    // config for ads1256
    ads1256.configChannel = 1;
    ads1256.configGain = ADS1256_GAIN_1;
    ads1256.configDrate = ADS1256_DRATE_1000SPS;

    ADS1256_ConfigInit(&ads1256);
    HAL_Delay(1000);
    ADS1256_getInfo();

    Encoder_init(&encoder);
    BLDCMotor_init(&motor, 7);
    BLDCDriver_init(&driver, 20e3, 4096, 14, 6);
    BLDCMotor_linkEncoder(&motor, &encoder);
    BLDCMotor_linkDriver(&motor, &driver);

    // PIDController_init(&pid, 0.1, 0.0, 0, 5, 8);
    // motor.pid = &pid;
    motor.foc_motor.controller = ControlType_velocity_openloop;
    motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;

    motor.enable_Port = GPIOC;
    motor.enable_Pin = GPIO_PIN_1;
    motor.target = 2;

    BLDCMotor_enable(&motor);
    BLDCMotor_setPhaseVoltage(&motor, 3, 0, _3PI_2);

    while (1)
    {
        // 1.
        BLDCMotor_move(&motor, 3);
        // ADS1256_printVoltage(&ads1256);
        ADS1256_get2Voltage(&ads1256);
        // printf("angle1 = %f, e_angle = %f\n", motor.angle, motor.e_angle);
    }
}

void test_openloop_vel_timer(void)
{
    ADS1256_t ads1256;

    // config for ads1256
    ads1256.configChannel = 1;
    ads1256.configGain = ADS1256_GAIN_1;
    ads1256.configDrate = ADS1256_DRATE_1000SPS;

    ADS1256_ConfigInit(&ads1256);
    HAL_Delay(1000);
    ADS1256_getInfo();

    Encoder_init(&encoder);
    BLDCMotor_init(&motor, 7);
    BLDCDriver_init(&driver, 20e3, 4096, 14, 6);
    BLDCMotor_linkEncoder(&motor, &encoder);
    BLDCMotor_linkDriver(&motor, &driver);

    // PIDController_init(&pid, 0.1, 0.0, 0, 5, 8);
    // motor.pid = &pid;
    motor.foc_motor.controller = ControlType_velocity_openloop;
    motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;

    motor.enable_Port = GPIOC;
    motor.enable_Pin = GPIO_PIN_1;
    motor.target = 5;
    motor.pid_dt = PID_UPDATE_T;

    pid_update_timer();
    BLDCMotor_enable(&motor);
    BLDCMotor_setPhaseVoltage(&motor, 3, 0, _3PI_2);

    while (1)
    {
        // 1.
        //  ADS1256_printVoltage(&ads1256);
        //  ADS1256_get2Voltage(&ads1256);
        printf("%f,%f, %f\n", motor.angle, motor.e_angle, motor.velocity);
    }
}

void test_closedloop_vel_timer(void)
{
    ADS1256_t ads1256;

    ads1256.configChannel = 1;
    ads1256.configGain = ADS1256_GAIN_1;
    ads1256.configDrate = ADS1256_DRATE_1000SPS;

    ADS1256_ConfigInit(&ads1256);
    HAL_Delay(1000);
    ADS1256_getInfo();

    Encoder_init(&encoder);
    BLDCMotor_init(&motor, 7);
    BLDCDriver_init(&driver, 20e3, 4096, 14, 6);
    BLDCMotor_linkEncoder(&motor, &encoder);
    BLDCMotor_linkDriver(&motor, &driver);

    PIDController_init(&pid, 0.02, 0.0, 0, 5, 8);
    motor.pid = &pid;
    motor.foc_motor.controller = ControlType_velocityClosedLoop;
    motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;

    motor.enable_Port = GPIOC;
    motor.enable_Pin = GPIO_PIN_1;
    motor.target = 60;
    motor.pid_dt = 10;

    pid_update_timer();
    BLDCMotor_enable(&motor);
    BLDCMotor_setPhaseVoltage(&motor, 3, 0, _3PI_2);
    delay_ms(5000);

    while (1)
    {
        // 1.
        //  ADS1256_printVoltage(&ads1256);
        //  ADS1256_get2Voltage(&ads1256);
        printf("%f,%f,%f\n", motor.target, motor.velocity, motor.foc_motor.voltage_q);
    }
}
void test_pid_vel(void)
{
    ADS1256_t ads1256;

    // config for ads1256
    ads1256.configChannel = 1;
    ads1256.configGain = ADS1256_GAIN_1;
    ads1256.configDrate = ADS1256_DRATE_1000SPS;

    ADS1256_ConfigInit(&ads1256);
    HAL_Delay(1000);
    ADS1256_getInfo();

    Encoder_init(&encoder);
    BLDCMotor_init(&motor, 7);
    BLDCDriver_init(&driver, 20e3, 4096, 12, 6);
    BLDCMotor_linkEncoder(&motor, &encoder);
    BLDCMotor_linkDriver(&motor, &driver);

    PIDController_init(&pid, 0.05, 0.0, 0, 5, 3);
    motor.pid = &pid;
    motor.foc_motor.controller = ControlType_velocityClosedLoop;
    motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;

    motor.enable_Port = GPIOC;
    motor.enable_Pin = GPIO_PIN_1;
    motor.target = 2;

    BLDCMotor_enable(&motor);
    delay_ms(2000);
    BLDCMotor_setPhaseVoltage(&motor, 3, 0, _3PI_2);

    while (1)
    {
        BLDCMotor_move(&motor, 30);
        // printf("%f,%f, %f, %f, %f, %f,%f\n", motor.target, motor.velocity,motor.foc_motor.voltage_q, motor.angle, motor.e_angle,motor.Ua, motor.Ub);
        printf("%f,%f\n", motor.velocity, motor.foc_motor.voltage_q);
        // ADS1256_printVoltage(&ads1256);
        // 2.
        // ADS1256_get2Voltage(&ads1256);
        // printf("angle1 = %f, e_angle = %f\n", motor.angle, motor.e_angle);
        // motor.angle = BLDCMotor_getAngle(&motor);
        // HAL_Delay(1000);
    }
}

void test_motor_as5600(void)
{

    ADS1256_t ads1256;

    ads1256.configChannel = 1;
    ads1256.configGain = ADS1256_GAIN_1;
    ads1256.configDrate = ADS1256_DRATE_1000SPS;

    ADS1256_ConfigInit(&ads1256);
    HAL_Delay(1000);
    ADS1256_getInfo();

    Encoder_init(&encoder);
    Encoder_AS5600_Init(&as5600, &as5600_i2c);
    BLDCMotor_init(&motor, 7);
    BLDCDriver_init(&driver, 10e3, 4096, 14, 6);
    BLDCMotor_linkEncoder(&motor, &encoder);
    BLDCMotor_linkAS5600(&motor, &as5600);
    BLDCMotor_linkDriver(&motor, &driver);

    PIDController_init(&pid, 0.4f, 0.1, 0.0f, 5, 7);
    motor.pid = &pid;
    motor.foc_motor.controller = ControlType_velocityClosedLoop;
    // motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;
    motor.foc_motor.foc_modulation = FOCModulationType_SinePWM;

    motor.enable_Port = GPIOC;
    motor.enable_Pin = GPIO_PIN_1;
    motor.target = 3;
    motor.pid_dt = PID_UPDATE_T;

    pid_update_timer();
    // BLDCMotor_setPhaseVoltage(&motor, 3, 0, _3PI_2);
    // delay_ms(5000);
    BLDCMotor_enable(&motor);

    while (1)
    {
        // 1.
        //  ADS1256_printVoltage(&ads1256);
        //  ADS1256_get2Voltage(&ads1256);
        //  printf("%f,%f,%f\n", motor.target, motor.velocity, motor.foc_motor.voltage_q);
        // 2.
        // float vel = BLDCMotor_getVelocityRPM(&motor);
        static int i = 0;
        if (i == 1000000)
        {
            printf("%f,%f,%f,%f\n", motor.target, motor.velocity, motor.foc_motor.voltage_q, motor.angle);
            i = 0;
        }
        i++;
        // printf("set v = %f,%f,%f\n", motor.Ua, motor.Ub, motor.Uc);
        // printf("pid = %f,%f,%f\n", motor.pid->P, motor.pid->I, motor.pid->D);
    }
}

void test_motor_as5600_currentLoop(void)
{
    ADS1256_t ads1256;
    ads1256.configChannel = 1;
    ads1256.configGain = ADS1256_GAIN_1;
    ads1256.configDrate = ADS1256_DRATE_1000SPS;
    ADS1256_ConfigInit(&ads1256);
    HAL_Delay(100);
    ADS1256_getInfo();

    CurrentSense_Init(&currentSense);

    Encoder_init(&encoder);
    Encoder_AS5600_Init(&as5600, &as5600_i2c);
    BLDCMotor_init(&motor, 7);
    BLDCDriver_init(&driver, 20e3, 4096, 14, 7);
    BLDCMotor_linkEncoder(&motor, &encoder);
    BLDCMotor_linkAS5600(&motor, &as5600);
    BLDCMotor_linkDriver(&motor, &driver);

    // 阶跃测试：用于PID参数调试
    // 步骤1: 仅P控制，找到临界P值
    PIDController_init(&pid, 0.4, 0.1, 0, 5, 7); // 从小P值开始
    motor.pid = &pid;
    motor.foc_motor.controller = ControlType_velocityClosedLoop;
    motor.foc_motor.foc_modulation = FOCModulationType_SinePWM;
    motor.enable_Port = GPIOC;
    motor.enable_Pin = GPIO_PIN_1;
    motor.target = 2.0; // 低速测试
    motor.pid_dt = PID_UPDATE_T;

    // pid_update_timer();
    BLDCMotor_enable(&motor);
    delay_ms(100);

    // printf("=== PID Tuning Mode ===\n");
    // printf("Current PID: P=%.2f, I=%.2f, D=%.2f\n", pid.P, pid.I, pid.D);
    // printf("Target: %.2f RPS\n", motor.target);
    // printf("Observe: Rise time, Overshoot, Steady-state error\n");
    // 【关键】预读取编码器角度，初始化lastAngle
    motor.as5600->lastReadAngle = AS5600_ReadAngle(motor.as5600);
    motor.as5600->lastAngle = motor.as5600->lastReadAngle;
    motor.angle = BLDCMotor_getAngle(&motor);
    motor.lastAngle = motor.angle;

    printf("=== PID Tuning Mode ===\n");
    printf("Current PID: P=%.2f, I=%.2f, D=%.2f\n", pid.P, pid.I, pid.D);
    printf("Target: %.2f RPS\n", motor.target);
    printf("Initial Angle: %.2f rad\n", motor.angle);
    printf("Observe: Rise time, Overshoot, Steady-state error\n");

    // 【关键】最后启动PID定时器
    pid_update_timer();
    uint32_t step_time = 0;
    int step = 0;

    int i = 0;
    const int sample_n = 10000;
    // float current[sample_n];
    float current_a[sample_n];
    // float current_b[sample_n];
    // float current_c[sample_n];
    while (1)
    {

        // 1.
        //  CurrentSense_read3Current(&currentSense);
        //  printf("v:%.2f, %.2f, %.2f\n", currentSense.i_a, currentSense.i_b, currentSense.i_c);
        // 2.
        for (i = 0; i < sample_n; i++)
        {
            // float v = CurrentSense_readCurrent(&currentSense);
            CurrentSense_read3Current(&currentSense);
            current_a[i] = currentSense.i_c;
            // current_b[i] = currentSense.i_b;
            // current_c[i] = currentSense.i_c;
        }
        for (i = 0; i < sample_n; i++)
        {
            // printf("%f, %f,%f\n", current_a[i], current_b[i]);
            printf("%f\n", current_a[i]);
        }

        // ADS1256_printVoltage(&ads1256);
        // if (i ==1000000)
        // {
        //     printf("%f\n", motor.velocity);
        //     i= 0;
        // }
        // i++;
    }
}
void test_motor_as5600_currentLoop_v1(void)
{
    ADS1256_t ads1256;
    ads1256.configChannel = 1;
    ads1256.configGain = ADS1256_GAIN_1;
    ads1256.configDrate = ADS1256_DRATE_1000SPS;
    ADS1256_ConfigInit(&ads1256);
    HAL_Delay(100);
    ADS1256_getInfo();

    CurrentSense_Init(&currentSense);
    motor.currentSense = &currentSense;

    Encoder_init(&encoder);
    Encoder_AS5600_Init(&as5600, &as5600_i2c);
    BLDCMotor_init(&motor, 7);
    BLDCDriver_init(&driver, 20e3, 4096, 14, 7);
    BLDCMotor_linkEncoder(&motor, &encoder);
    BLDCMotor_linkAS5600(&motor, &as5600);
    BLDCMotor_linkDriver(&motor, &driver);

    // 阶跃测试：用于PID参数调试
    // 步骤1: 仅P控制，找到临界P值
    PIDController_init(&pid, 0.4, 0.1, 0, 5, 7); // 从小P值开始
    motor.pid = &pid;
    motor.foc_motor.controller = ControlType_velocityClosedLoop;
    motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;
    motor.enable_Port = GPIOC;
    motor.enable_Pin = GPIO_PIN_1;
    motor.target = -2.0; // 低速测试
    motor.pid_dt = PID_UPDATE_T;

    // pid_update_timer();
    BLDCMotor_enable(&motor);
    delay_ms(5000);

    // printf("=== PID Tuning Mode ===\n");
    // printf("Current PID: P=%.2f, I=%.2f, D=%.2f\n", pid.P, pid.I, pid.D);
    // printf("Target: %.2f RPS\n", motor.target);
    // printf("Observe: Rise time, Overshoot, Steady-state error\n");
    // 【关键】预读取编码器角度，初始化lastAngle
    motor.as5600->lastReadAngle = AS5600_ReadAngle(motor.as5600);
    motor.as5600->lastAngle = motor.as5600->lastReadAngle;
    motor.angle = BLDCMotor_getAngle(&motor);
    motor.lastAngle = motor.angle;

    printf("=== PID Tuning Mode ===\n");
    printf("Current PID: P=%.2f, I=%.2f, D=%.2f\n", pid.P, pid.I, pid.D);
    printf("Target: %.2f RPS\n", motor.target);
    printf("Initial Angle: %.2f rad\n", motor.angle);
    printf("Observe: Rise time, Overshoot, Steady-state error\n");

    // 【关键】最后启动PID定时器
    pid_update_timer();
    uint32_t step_time = 0;
    int step = 0;

    int i = 0;
    const int sample_n = 10000;
    // float current[sample_n];
    float current_a[sample_n];
    // float current_b[sample_n];
    // float current_c[sample_n];
    float i_a, i_b, i_alpha, i_beta, i_q, i_d;

    while (1)
    {

        // for (i = 0; i < sample_n; i++)
        // {
        // 1.
        // BLDCMotor_getCurrentDQ(&motor);
        // current_a[i] = motor.foc_motor.i_d;

        // 1.
        // CurrentSense_get3Current(motor->currentSense);
        // i_a = motor->currentSense->i3[0];
        // i_b = motor->currentSense->i3[1];
        // 2.
        // CurrentSense_read3Current(&currentSense);
        //     i_a = currentSense.i_a;
        //     i_b = currentSense.i_c;
        //     printf("%f,%f\n", i_a, i_b);
        // HAL_Delay(2);

        // 3.
        //     CurrentSense_read3Current(&currentSense);
        //     i_a = currentSense.i_a;
        //     i_b = currentSense.i_c;
        //     Clarke_Transform(i_a, i_b, &i_alpha, &i_beta);
        //     Park_Transform(i_alpha, i_beta, _normalizeAngle(motor.e_angle), &i_d, &i_q);
        // printf("%f, %f\n", i_d, i_q);
        // 4.
        for (int i = 0; i < sample_n; i++)
        {
            currentSense.adc = &currentSense_adc3;
            float v = CurrentSense_getRawVoltage(&currentSense);
            // float v= CurrentSense_readCurrent(&currentSense);
            // v = (v-1.25) / 0.02/6;
            current_a[i] = v;
        }
        for (i = 0; i < sample_n; i++)
        {
            printf("%f\n", current_a[i]);
        }

        // HAL_Delay(10);

        // }
        // for (i = 0; i < sample_n; i++)
        // {
        //     printf("%f,%f\n", current_a[i],current_c[i]);
        // }

        // ADS1256_printVoltage(&ads1256);
        // if (i ==1000000)
        // {
        //     printf("%f\n", motor.velocity);
        //     i= 0;
        // }
        // i++;
    }
}
void test_motor_as5600_currentLoop_torque(void)
{
    ADS1256_t ads1256;
    ads1256.configChannel = 1;
    ads1256.configGain = ADS1256_GAIN_1;
    ads1256.configDrate = ADS1256_DRATE_1000SPS;
    ADS1256_ConfigInit(&ads1256);
    HAL_Delay(100);
    ADS1256_getInfo();

    CurrentSense_Init(&currentSense);
    motor.currentSense = &currentSense;

    Encoder_init(&encoder);
    Encoder_AS5600_Init(&as5600, &as5600_i2c);
    BLDCMotor_init(&motor, 7);
    BLDCDriver_init(&driver, 20e3, 4096, 12, 6);
    BLDCMotor_linkEncoder(&motor, &encoder);
    BLDCMotor_linkAS5600(&motor, &as5600);
    BLDCMotor_linkDriver(&motor, &driver);

    // 阶跃测试：用于PID参数调试
    // 步骤1: 仅P控制，找到临界P值
    PIDController_init(&pid, 0.4, 0.1, 0, 5, 5); // 从小P值开始
    motor.pid = &pid;
    motor.foc_motor.controller = ControlType_velocityClosedLoop;
    motor.foc_motor.foc_modulation = FOCModulationType_SinePWM;
    motor.enable_Port = GPIOC;
    motor.enable_Pin = GPIO_PIN_1;
    motor.target = 2.0; // 低速测试
    motor.pid_dt = PID_UPDATE_T;

    // pid_update_timer();
    BLDCMotor_enable(&motor);
    delay_ms(5000);

    // printf("=== PID Tuning Mode ===\n");
    // printf("Current PID: P=%.2f, I=%.2f, D=%.2f\n", pid.P, pid.I, pid.D);
    // printf("Target: %.2f RPS\n", motor.target);
    // printf("Observe: Rise time, Overshoot, Steady-state error\n");
    // 【关键】预读取编码器角度，初始化lastAngle
    motor.as5600->lastReadAngle = AS5600_ReadAngle(motor.as5600);
    motor.as5600->lastAngle = motor.as5600->lastReadAngle;
    motor.angle = BLDCMotor_getAngle(&motor);
    motor.lastAngle = motor.angle;

    printf("=== PID Tuning Mode ===\n");
    printf("Current PID: P=%.2f, I=%.2f, D=%.2f\n", pid.P, pid.I, pid.D);
    printf("Target: %.2f RPS\n", motor.target);
    printf("Initial Angle: %.2f rad\n", motor.angle);
    printf("Observe: Rise time, Overshoot, Steady-state error\n");

    // 【关键】最后启动PID定时器
    // pid_update_timer();
    uint32_t step_time = 0;
    int step = 0;

    int i = 0;
    const int sample_n = 10000;
    // float current[sample_n];
    float current_a[sample_n];
    // float current_b[sample_n];
    // float current_c[sample_n];
    float i_a, i_b, i_alpha, i_beta, i_q, i_d;
    BLDCMotor_setPhaseVoltage(&motor, 1, 0, _3PI_2);

    while (1)
    {
        for (int i = 0; i < sample_n; i++)
        {
            currentSense.adc = &currentSense_adc3;
            float v = CurrentSense_getRawVoltage(&currentSense);
            // float v= CurrentSense_readCurrent(&currentSense);
            // v = (v-1.25) / 0.02/6;
            current_a[i] = v;
        }
        for (i = 0; i < sample_n; i++)
        {
            printf("%f\n", current_a[i]);
        }
    }
}
void test_motor_as5600_tune_pid(void)
{
    ADS1256_t ads1256;
    ads1256.configChannel = 1;
    ads1256.configGain = ADS1256_GAIN_1;
    ads1256.configDrate = ADS1256_DRATE_1000SPS;
    ADS1256_ConfigInit(&ads1256);
    HAL_Delay(1000);
    ADS1256_getInfo();

    Encoder_init(&encoder);
    Encoder_AS5600_Init(&as5600, &as5600_i2c);
    BLDCMotor_init(&motor, 7);
    BLDCDriver_init(&driver, 20e3, 4096, 14, 7);
    BLDCMotor_linkEncoder(&motor, &encoder);
    BLDCMotor_linkAS5600(&motor, &as5600);
    BLDCMotor_linkDriver(&motor, &driver);

    // 阶跃测试：用于PID参数调试
    // 步骤1: 仅P控制，找到临界P值
    PIDController_init(&pid, 0.4, 0.1, 0, 5, 7); // 从小P值开始
    motor.pid = &pid;
    motor.foc_motor.controller = ControlType_velocityClosedLoop;
    motor.foc_motor.foc_modulation = FOCModulationType_SinePWM;
    motor.enable_Port = GPIOC;
    motor.enable_Pin = GPIO_PIN_1;
    motor.target = 1.0; // 低速测试
    motor.pid_dt = PID_UPDATE_T;

    // pid_update_timer();
    BLDCMotor_enable(&motor);
    delay_ms(100);

    // printf("=== PID Tuning Mode ===\n");
    // printf("Current PID: P=%.2f, I=%.2f, D=%.2f\n", pid.P, pid.I, pid.D);
    // printf("Target: %.2f RPS\n", motor.target);
    // printf("Observe: Rise time, Overshoot, Steady-state error\n");
    // 【关键】预读取编码器角度，初始化lastAngle
    motor.as5600->lastReadAngle = AS5600_ReadAngle(motor.as5600);
    motor.as5600->lastAngle = motor.as5600->lastReadAngle;
    motor.angle = BLDCMotor_getAngle(&motor);
    motor.lastAngle = motor.angle;

    printf("=== PID Tuning Mode ===\n");
    printf("Current PID: P=%.2f, I=%.2f, D=%.2f\n", pid.P, pid.I, pid.D);
    printf("Target: %.2f RPS\n", motor.target);
    printf("Initial Angle: %.2f rad\n", motor.angle);
    printf("Observe: Rise time, Overshoot, Steady-state error\n");

    // 【关键】最后启动PID定时器
    pid_update_timer();
    uint32_t step_time = 0;
    int step = 0;

    while (1)
    {
        static int i = 0;
        if (i == 1000000) // 更频繁的输出用于调试
        {
            float error = motor.target - motor.velocity;
            float error_pct = (motor.target != 0) ? (error / motor.target * 100) : 0;

            printf("Step%d | T:%.2f V:%.2f Vq:%.2f E:%.3f(%.1f%%)\n", step, motor.target, motor.velocity, motor.foc_motor.voltage_q, error, error_pct);
            i = 0;
        }
        i++;

        // 自动阶跃测试
        if (HAL_GetTick() - step_time > 10000) // 每5秒切换一次
        {
            step++;
            step_time = HAL_GetTick();

            if (step == 1)
            {
                motor.target = 2.0;
                printf("\n>>> Step to 2.0 RPS <<<\n");
            }
            else if (step == 2)
            {
                motor.target = 0.5;
                printf("\n>>> Step to 0.5 RPS <<<\n");
            }
            else if (step == 3)
            {
                motor.target = -3;
                printf("\n>>> Step to 1.5 RPS <<<\n");
            }
            else
            {
                step = 0;
                motor.target = 1.0;
                printf("\n>>> Step to 1.0 RPS <<<\n");
            }
        }
    }
}
void test_pwm(void)
{
    PWM6_Init(1000, 4096);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
    while (1)
    {
        printf("test_motor\n");
        delay_ms(1000);
    }
}

void test_motor(void)
{
    // 1.
    //  test_openloop_v();
    // 2.
    //  test_pid_vel();
    // 3.
    //  test_openloop_vel_timer();
    // 4.
    //  test_closedloop_vel_timer();
    // 5.
    //  test_motor_as5600();
    // 6.
    //  test_motor_as5600_tune_pid();
    // 7.
    //  test_motor_as5600_currentLoop();
    test_motor_as5600_currentLoop_v1();
    // 8.
    //  test_motor_as5600_currentLoop_torque();
}