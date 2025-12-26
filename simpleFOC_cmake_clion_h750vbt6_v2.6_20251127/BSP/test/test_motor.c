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
#include "motor.h"
#include "lowpass_filter.h"

#include <string.h>

extern BLDCMotor_t motor;
extern BLDCDriver_t driver;
extern FOCMotor_t FOCMotor;
extern Encoder_t encoder;
extern AS5600_t as5600;
extern CurrentSense_t currentSense;
extern PIDController pid;

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
    BLDCDriver_init(&driver, 20e3, 4096, 14, 7);
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
        // ADS1256_get2Voltage(&ads1256);
        // printf("angle1 = %f, e_angle = %f\n", motor.angle, motor.e_angle);
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
void test_motor_currentLoop_v0(void)
{

    CurrentSense_Init(&currentSense);
    motor.currentSense = &currentSense;

    Encoder_init(&encoder);
    BLDCMotor_init(&motor, 7);
    BLDCDriver_init(&driver, 20e3, 4096, 14, 14);
    BLDCMotor_linkEncoder(&motor, &encoder);
    BLDCMotor_linkDriver(&motor, &driver);

    BLDCMotor_alignSensor(&motor);

    // 阶跃测试：用于PID参数调试
    // 步骤1: 仅P控制，找到临界P值
    PIDController_init(&pid, 0.8f, 0.0, 0, 5, 2); // 从小P值开始
    motor.pid = &pid;
    motor.foc_motor.controller = ControlType_velocityClosedLoop;
    // motor.foc_motor.foc_modulation = FOCModulationType_SinePWM;
    motor.target = 0.0f; // 低速测试

    // motor.foc_motor.zero_electric_angle = 0.0f;
    BLDCMotor_enable(&motor);
    delay_ms(100);

    printf("=== PID Tuning Mode ===\n");
    printf("Current PID: P=%.2f, I=%.2f, D=%.2f\n", pid.P, pid.I, pid.D);
    printf("Target: %.2f RPS\n", motor.target);
    motor.angle = BLDCMotor_getAngle(&motor);
    printf("Initial Angle: %.2f rad\n", motor.angle);
    printf("Observe: Rise time, Overshoot, Steady-state error\n");

    uint32_t step_time = 0;
    int step = 0;
    currentSense.adc = &currentSense_adc2;

    int isFirst = 1;
    printf("\n=== Motor Status Check ===\n");
    printf("isEnable: %d (should be 1)\n", motor.isEnable);
    printf("zero_electric_angle: %.4f rad\n", motor.foc_motor.zero_electric_angle);
    printf("voltage_limit: %.2f V\n", motor.foc_motor.voltage_limit);
    printf("pole_pairs: %d\n", motor.foc_motor.pole_pairs);
    printf("controller: %d (2=velocityClosedLoop)\n", motor.foc_motor.controller);
    printf("===========================\n\n");

    // 【诊断】添加调试输出
    printf("\n=== Starting Control Loop ===\n");
    printf("Press any key or wait...\n\n");
    delay_ms(1000);
    // 【关键】校准后重新初始化所有状态变量

    static int debug_count = 0;
    // 1.
    //  while (1)
    //  {
    //      BLDCMotor_move(&motor, -2);
    //
    //      // 【添加】详细诊断输出
    //      if (debug_count++ % 1000 == 0) {
    //          printf("T:%.2f V:%.2f Uq:%.2f angle:%.2f e_ang:%.2f Ua:%.2f Ub:%.2f Uc:%.2f\n",
    //                 motor.target,
    //                 motor.velocity,
    //                 motor.foc_motor.voltage_q,
    //                 motor.angle,
    //                 motor.e_angle,
    //                 motor.Ua, motor.Ub, motor.Uc);
    //      }
    //  }
    while (1)
    {

        BLDCMotor_move(&motor, 4);
        // if (debug_count++ % 1000 == 0)
        // {
        //     printf("motor:%f, %f, %f, %f\n",motor.e_angle,motor.target,motor.velocity, motor.pid->error_prev, motor.pid->output_prev);
        // }
        // 2.
        printf("v:%f,%f,%f\n", motor.target, motor.velocity, motor.foc_motor.voltage_q);
        delay_ms(1);
    }
}

void test_motor_velocityLoop(void)
{

    PIDController pid_a;
    pid_a.P = 0.9f;
    pid_a.I = 0.1f;
    pid_a.D = 0.0f;
    MotorFOC_Init(&motor, pid_a, ControlType_velocityClosedLoop);
    delay_ms(1000);
    static int debug_count = 0;
    char buffer[20];
    int last_t = HAL_GetTick();
    while (1)
    {

        //1.
        int current_t = HAL_GetTick();
        if (current_t - last_t > 1)
        {
            BLDCMotor_move(&motor, 2);
            UART_SendDataFloat(&motor.velocity, 1);
            last_t = current_t;
        }

        // if (debug_count++ % 1000 == 0)
        // {
        // printf("motor:%f, %f, %f, %f\n",motor.e_angle,motor.target,motor.velocity, motor.pid->error_prev, motor.pid->output_prev);
        // printf("motor:%f, %f, %f, %f\n",motor.e_angle,motor.target,motor.velocity, motor.pid->error_prev, motor.pid->output_prev);
        // printf("v:%f,%f,%f\n",motor.target, motor.velocity, motor.foc_motor.voltage_q);
        // }
        // 2.
        // printf("v:%f,%f,%f\n",motor.target, motor.velocity, motor.foc_motor.voltage_q);
        // delay_ms(2);
        // 3.

        // if (debug_count++ % 1000 == 0)
        // {
        //
        //     int t1 = HAL_GetTick();
        //     sprintf(buffer, "%f,%f\n", motor.target, motor.velocity);
        //     HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 100);
        //     int t2 = HAL_GetTick();
        //     printf("delta1 t = %d\n", t2 - t1);
        //
        //     t1 = HAL_GetTick();
        //     printf("motor:%f, %f, %f, %f\n", motor.e_angle, motor.target, motor.velocity, motor.pid->error_prev, motor.pid->output_prev);
        //     t2 = HAL_GetTick();
        //     printf("delta2 t = %d\n", t2 - t1);
        // }
        //4.
        // sprintf(buffer, "%f,%f\n", motor.target, motor.velocity);
        // HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 100);
        UART_SendDataFloat(&motor.velocity, 1);

    }
}

LowPassFilter_t lpf_a;
LowPassFilter_t lpf_b;
void test_motor_currentLoop(void)
{

    PIDController pid_a;
    pid_a.P = 0.9f;
    pid_a.I = 0.1f;
    pid_a.D = 0.0f;
    MotorFOC_Init(&motor, pid_a, ControlType_velocityClosedLoop);

    LowPassFilter_init(&lpf_a, 0.05f);
    LowPassFilter_init(&lpf_b, 0.05f);
    while (1)
    {

        BLDCMotor_move(&motor, 3);

        // if (debug_count++ % 1000 == 0)
        // {
        //     printf("motor:%f, %f, %f, %f\n",motor.e_angle,motor.target,motor.velocity, motor.pid->error_prev, motor.pid->output_prev);
        // }
        // 2.
        // printf("v:%f,%f,%f\n",motor.target, motor.velocity, motor.foc_motor.voltage_q);
        // delay_ms(1);
        // 3.
        CurrentSense_read3Current(&currentSense);
        currentSense.i_a = LowPassFilter(&lpf_a, currentSense.i_a);
        currentSense.i_b = LowPassFilter(&lpf_b, currentSense.i_c);

        printf("i:%f, %f, %f, %f\n",motor.velocity, currentSense.i_a, currentSense.i_b, currentSense.i_c);
        delay_ms(4);
        // 4
        //  BLDCMotor_getCurrentDQ(&motor);
        //  printf("i:%f,%f\n", motor.foc_motor.i_q, motor.foc_motor.i_d);
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

    CurrentSense_Init(&currentSense);
    motor.currentSense = &currentSense;

    Encoder_init(&encoder);
    // Encoder_AS5600_Init(&as5600, &as5600_i2c);
    BLDCMotor_init(&motor, 7);
    BLDCDriver_init(&driver, 10e3, 4096, 14, 7);
    BLDCMotor_linkEncoder(&motor, &encoder);
    // BLDCMotor_linkAS5600(&motor, &as5600);
    BLDCMotor_linkDriver(&motor, &driver);

    // // ========== 电角度零点校准 ==========
    // printf("\n=== Electric Angle Alignment ===\n");
    //
    // motor.foc_motor.voltage_sensor_align = 3.0f;  // 校准电压
    // motor.enable_Port = GPIOC;
    // motor.enable_Pin = GPIO_PIN_1;
    //
    // BLDCMotor_enable(&motor);  // 使能电机
    // delay_ms(100);
    //
    // BLDCMotor_alignSensor(&motor);  // 运行校准
    //
    // printf("Calibrated zero_electric_angle = %.4f rad (%.2f deg)\n",
    //        motor.foc_motor.zero_electric_angle,
    //        motor.foc_motor.zero_electric_angle * 180.0f / 3.14159265f);
    //
    // // 【关键】校准后重新初始化所有状态变量
    // printf("Re-initializing states after alignment...\n");
    //
    // // 【修复】使用正确的实例：encoder.as5600_Instance
    // encoder.as5600_Instance.lastReadAngle = AS5600_ReadAngle(&encoder.as5600_Instance);
    // encoder.as5600_Instance.lastAngle = encoder.as5600_Instance.lastReadAngle;
    // encoder.as5600_Instance.lastMeasurement = 0;  // 重置时间戳
    //
    // // 同时初始化motor.as5600（如果有的话）
    // if (motor.as5600 != NULL) {
    //     motor.as5600->lastReadAngle = AS5600_ReadAngle(motor.as5600);
    //     motor.as5600->lastAngle = motor.as5600->lastReadAngle;
    //     motor.as5600->lastMeasurement = 0;
    // }
    //
    // motor.angle = BLDCMotor_getAngle(&motor);
    // motor.lastAngle = motor.angle;
    // motor.velocity = 0;
    // motor.open_loop_timestamp = _micros();
    //
    // printf("States re-initialized!\n");
    // printf("  encoder.as5600_Instance.lastAngle = %d\n", encoder.as5600_Instance.lastReadAngle);
    // printf("  motor.angle = %.4f\n", motor.angle);
    //
    // // 校准后不要disable，保持enable状态
    // delay_ms(500);
    // printf("=== Alignment Complete ===\n\n");

    // ==========================================
    // // ========== 【关键】电角度零点校准 ==========
    // printf("\n=== Electric Angle Alignment ===\n");
    // printf("Motor will move to align position...\n");
    //
    // motor.foc_motor.voltage_sensor_align = 3.0f;  // 校准电压
    // motor.enable_Port = GPIOC;
    // motor.enable_Pin = GPIO_PIN_1;
    // BLDCMotor_enable(&motor);
    // delay_ms(100);
    //
    // // 运行校准程序
    // BLDCMotor_alignSensor(&motor);
    //
    // printf("zero_electric_angle = %.4f rad (%.2f deg)\n",
    //        motor.foc_motor.zero_electric_angle,
    //        motor.foc_motor.zero_electric_angle * 180.0f / 3.14159265f);
    // printf("Alignment complete!\n\n");
    //
    // BLDCMotor_disable(&motor);

    // motor.foc_motor.zero_electric_angle = 6.1f;
    // delay_ms(500);

    // 阶跃测试：用于PID参数调试
    // 步骤1: 仅P控制，找到临界P值
    PIDController_init(&pid, 1.0f, 0.0, 0, 5, 7); // 从小P值开始
    motor.pid = &pid;
    motor.foc_motor.controller = ControlType_velocityClosedLoop;
    motor.foc_motor.foc_modulation = FOCModulationType_SinePWM;
    motor.enable_Port = GPIOC;
    motor.enable_Pin = GPIO_PIN_1;
    motor.target = 0.0f; // 低速测试
    motor.pid_dt = PID_UPDATE_T;

    BLDCMotor_enable(&motor);
    delay_ms(100);

    printf("=== PID Tuning Mode ===\n");
    printf("Current PID: P=%.2f, I=%.2f, D=%.2f\n", pid.P, pid.I, pid.D);
    printf("Target: %.2f RPS\n", motor.target);
    printf("Initial Angle: %.2f rad\n", motor.angle);
    printf("Observe: Rise time, Overshoot, Steady-state error\n");

    uint32_t step_time = 0;
    int step = 0;
    currentSense.adc = &currentSense_adc2;

    int isFirst = 1;
    printf("\n=== Motor Status Check ===\n");
    printf("isEnable: %d (should be 1)\n", motor.isEnable);
    printf("zero_electric_angle: %.4f rad\n", motor.foc_motor.zero_electric_angle);
    printf("voltage_limit: %.2f V\n", motor.foc_motor.voltage_limit);
    printf("pole_pairs: %d\n", motor.foc_motor.pole_pairs);
    printf("controller: %d (2=velocityClosedLoop)\n", motor.foc_motor.controller);
    printf("===========================\n\n");

    // 【诊断】添加调试输出
    printf("\n=== Starting Control Loop ===\n");
    printf("Press any key or wait...\n\n");
    delay_ms(1000);
    // 【关键】校准后重新初始化所有状态变量

    static int debug_count = 0;
    // while (1)
    // {
    //     BLDCMotor_move(&motor, -3);
    //
    //     // 【添加】详细诊断输出
    //     if (debug_count++ % 1000 == 0) {
    //         printf("T:%.2f V:%.2f Uq:%.2f angle:%.2f e_ang:%.2f Ua:%.2f Ub:%.2f Uc:%.2f\n",
    //                motor.target,
    //                motor.velocity,
    //                motor.foc_motor.voltage_q,
    //                motor.angle,
    //                motor.e_angle,
    //                motor.Ua, motor.Ub, motor.Uc);
    //     }
    // }
    while (1)
    {

        BLDCMotor_move(&motor, 3);
        // 1.
        //  if (isFirst < 10)
        //  {
        //  printf("motor:%f,%f, %f, %f, %f\n",motor.foc_motor.zero_electric_angle,motor.target,motor.velocity, motor.pid->error_prev, motor.pid->output_prev);
        //  }
        //  isFirst++;
        //  HAL_Delay(2);
        // 2.

        if (debug_count++ % 1000 == 0)
        {
            printf("motor:%f, %f, %f, %f\n", motor.e_angle, motor.target, motor.velocity, motor.pid->error_prev, motor.pid->output_prev);
        }

        // HAL_Delay(1);
        // float v = CurrentSense_getRawVoltage(&currentSense);
        // printf("/*%*/f,%f, %f\n",motor.target, motor.velocity, v);

        // static int i = 0;
        // if (i == 1000000) // 更频繁的输出用于调试
        // {
        //     float error = motor.target - motor.velocity;
        //     float error_pct = (motor.target != 0) ? (error / motor.target * 100) : 0;
        //
        //     printf("Step%d | T:%.2f V:%.2f Vq:%.2f E:%.3f(%.1f%%)\n", step, motor.target, motor.velocity, motor.foc_motor.voltage_q, error, error_pct);
        //     i = 0;
        // }
        // i++;
        //
        // // 自动阶跃测试
        // if (HAL_GetTick() - step_time > 10000) // 每5秒切换一次
        // {
        //     step++;
        //     step_time = HAL_GetTick();
        //
        //     if (step == 1)
        //     {
        //         motor.target = 2.0;
        //         printf("\n>>> Step to 2.0 RPS <<<\n");
        //     }
        //     else if (step == 2)
        //     {
        //         motor.target = 0.5;
        //         printf("\n>>> Step to 0.5 RPS <<<\n");
        //     }
        //     else if (step == 3)
        //     {
        //         motor.target = -3;
        //         printf("\n>>> Step to 1.5 RPS <<<\n");
        //     }
        //     else
        //     {
        //         step = 0;
        //         motor.target = 1.0;
        //         printf("\n>>> Step to 1.0 RPS <<<\n");
        //     }
        // }
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
    // test_openloop_v();
    // 2.
    // test_pid_vel();
    // 3.
    //  test_openloop_vel_timer();
    // 4.
    //  test_closedloop_vel_timer();
    // 5.
    //  test_motor_as5600();
    // 6.ok
    // test_motor_as5600_tune_pid();
    // 7.
    //  test_motor_as5600_currentLoop();
    // test_motor_as5600_currentLoop_v1();
    // 8.
    //  test_motor_as5600_currentLoop_torque();
    // 9. ok
    // test_motor_velocityLoop();
    // 10.
    test_motor_currentLoop();
}