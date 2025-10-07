#include "test.h"
#include "adc.h"
#include "encoder.h"
#include "encoder_as5600.h"
#include "foc_base.h"
#include "foc_motor.h"
#include "i2c.h"
#include "lowpass_filter.h"
#include "pid.h"
#include "stm32_hal.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_uart.h"
#include "tim.h"
#include "usart.h"
#include "led.h"
#include "BLDCMotor.h"
#include "BLDCDriver.h"
#include "currentSense.h"
#include "pwm.h"
#include <stdio.h>
#include "as5600.h"
#include "encoder_as5600.h"
#include "hallSensor.h"

#define ANGLE _2PI

BLDCMotor_t motor;
BLDCDriver_t driver;
FOCMotor_t FOCMotor;
Encoder_t encoder;
AS5600_t as5600;
CurrentSense_t currentSense;
LowPassFilter_t lpf_a;
LowPassFilter_t lpf_b;

PIDController pid_velocity;
PIDController pid_id;
PIDController pid_iq;

void test_velocity_closedloop_without_i(void);
void test_printf(void);
void test_led(void);

void test_encoder(void);
void test_3current(void);
void test_openloop_velocity(void);
void test_pwm(void);
void test_as5600(void);

void test_hallSensor_57BLDC(void);
void test_current_closedloop(void);
void test_velocity_closedloop(void);
void test_main(void)
{
    // test_led();
    // test_printf();
    // test_3current();
    // test_as5600();
    // test_openloop_velocity();
    // test_velocity_closedloop();
    // test_encoder();
    // test_current_closedloop();
    // test_velocity_closedloop_without_i();
    test_hallSensor_57BLDC();
}
void test_printf(void)
{
    while (1)
    {
        printf("hello test\n");
        HAL_Delay(1000);
    }
}

void test_led(void)
{
    while (1)
    {
        LED1_TOGGLE();
        HAL_Delay(1000);
    }
}

void test_pwm(void)
{

    while (1)
    {
    }
}

// void test_openloop_velocity(void)
// {
//     BLDCMotor_t motor;
//     BLDCDriver_t driver;
//     Sensor_t sensor;
//     CurrentSense_t currentSense;
//
//     BLDCMotor_init(&motor, 7);
//     BLDCDriver3PWM_init(&driver, 12, 6);
//     BLDCMotor_linkDriver(&motor, &driver);
//     motor.foc_motor.controller = ControlType_velocity_openloop;
//     motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;
//     motor.enable_Port = GPIOB;
//     motor.enable_Pin = GPIO_PIN_15;
//
//     CurrentSense_Init(&currentSense);
//     pwm_init(20e3, 4096);
//     BLDCMotor_enable(&motor);
//     // BLDCMotor_setPhaseVoltage(&motor, 2, 0, _3PI_2);
//     while (1)
//     {
//         BLDCMotor_move(&motor, 1);
//         // CurrentSense_get3Current(&currentSense);
//         // // printf("offset = %f, %f, %f\n", currentSense.offset3[0], currentSense.offset3[1], currentSense.offset3[2]);
//         // printf("%f, %f, %f\n", currentSense.i3[0], currentSense.i3[1], currentSense.i3[2]);
//     }
// }

void test_3current(void)
{
    // BLDCMotor_t motor;
    // BLDCDriver_t driver;
    // Sensor_t sensor;
    // CurrentSense_t currentSense;
    //
    // BLDCMotor_init(&motor, 7);
    // BLDCDriver3PWM_init(&driver, 12, 10);
    // BLDCMotor_linkDriver(&motor, &driver);
    // motor.foc_motor.controller = ControlType_velocity_openloop;
    // motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;
    //
    // CurrentSense_Init(&currentSense);
    // // float current_offset = CurrentSense_getOffset(&currentSense);
    // while (1)
    // {
    //     CurrentSense_get3Current(&currentSense);
    //     // printf("%f, %f, %f\n", currentSense.offset3[0], currentSense.offset3[1], currentSense.offset3[2]);
    //     printf("%f, %f, %f\n", currentSense.i3[0], currentSense.i3[1], currentSense.i3[2]);
    // }
    BLDCMotor_t motor;
    BLDCDriver_t driver;
    Sensor_t sensor;
    CurrentSense_t currentSense;
    LowPassFilter_t lpf_d;
    LowPassFilter_t lpf_q;
    PIDController pid;
    PIDController pid_d;
    PIDController pid_q;

    BLDCMotor_init(&motor, 7);
    BLDCDriver3PWM_init(&driver, 12, 6);
    BLDCMotor_linkDriver(&motor, &driver);
    // motor.foc_motor.controller = ControlType_velocity_openloop;
    motor.foc_motor.controller = ControlType_current;
    motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;
    motor.enable_Port = GPIOB;
    motor.enable_Pin = GPIO_PIN_15;

    CurrentSense_Init(&currentSense);
    pwm_init(20e3, 4096);
    BLDCMotor_enable(&motor);

    LowPassFilter_init(&lpf_d, 0.05);
    LowPassFilter_init(&lpf_q, 0.05);

    PIDController_init(&pid_q, 5, 0.2, 0, 6, 5);
    PIDController_init(&pid_d, 1, 0.0, 0, 6, 5);

    motor.foc_motor.PID_current_d = &pid_d;
    motor.foc_motor.PID_current_q = &pid_q;
    // BLDCMotor_setPhaseVoltage(&motor, 3, 0, _3PI_2);
    float iq, id, i_alpha, i_beta;
    float ia, ib, ic;
    while (1)
    {
        // BLDCMotor_loopFOC(&motor);

        BLDCMotor_move(&motor, 0.3);
        CurrentSense_get3Current(&currentSense);
        // // // printf("offset = %f, %f, %f\n", currentSense.offset3[0], currentSense.offset3[1], currentSense.offset3[2]);
        // // printf("%f, %f, %f\n", currentSense.i3[0], currentSense.i3[1], currentSense.i3[2]);
        //
        // // ia = LowPassFilter(&lpf, currentSense.i3[0]);
        // // ib = LowPassFilter(&lpf, currentSense.i3[1]);
        // // ic = LowPassFilter(&lpf, currentSense.i3[2]);
        Clarke_Transform(currentSense.i3[0], currentSense.i3[1], &i_alpha, &i_beta);
        Park_Transform(i_alpha, i_beta, _normalizeAngle(_electricalAngle(motor.foc_motor.shaft_angle, motor.foc_motor.pole_pairs)), &id, &iq);
        id = LowPassFilter(&lpf_d, id);
        iq = LowPassFilter(&lpf_q, iq);
        motor.foc_motor.i_d = id;
        motor.foc_motor.i_q = iq;

        printf("%f, %f, %f, %f, %f\n", id, iq, motor.foc_motor.target, motor.foc_motor.voltage_q, motor.foc_motor.voltage_d);
        // printf("%f, %f, %f\n", ia, ib, ic);
    }
}

void test_as5600(void)
{
    // FOCMotor_init(&foc_motor);
    Encoder_AS5600_Init(&as5600, &hi2c1);
    Encoder_linkAS5600(&encoder, &as5600);
    FOCMotor_linkEncoder(&FOCMotor, &encoder);
    // BLDCMotor_linkEncoder(&motor, &encoder);
    BLDCMotor_init(&motor, 7);

    BLDCDriver3PWM_init(&driver, 12, 6);
    BLDCMotor_linkDriver(&motor, &driver);
    pwm_init(20e3, 4096);

    PIDController_init(&pid_velocity, 0.1, 0.0, 0, 5, 8);
    motor.pid = &pid_velocity;
    motor.foc_motor.controller = ControlType_velocity;
    // motor.foc_motor.controller = ControlType_velocity_openloop;
    motor.foc_motor.foc_modulation = FOCModulationType_SinePWM;

    motor.enable_Port = GPIOB;
    motor.enable_Pin = GPIO_PIN_15;
    motor.target = 60;

    // CurrentSense_Init(&currentSense);
    BLDCMotor_enable(&motor);
    timer_init(&htim3, 100);
    while (1)
    {

        // BLDCMotor_loopFOC(&motor);
        // BLDCMotor_move(&motor, 60);
        // float angle = FOCMotor_shaftAngle(&FOCMotor);
        // float angle = getAngle();
        float angle = AS5600_getAngle(&as5600);
        // float vel = getVelocity();
        float vel = AS5600_getVelocity(&as5600);
        // float angle = Encoder_getAngle(&encoder);
        // float vel = FOCMotor_shaftVelocity(&motor.foc_motor);
        // float vel = BLDCMotor_getVelocity(&motor);
        // printf("vel = %f\n", motor.velocity);
        printf("vel = %f\n", vel);
        // printf("target = %f\n", motor.target);
        // printf("angle = %f\n", motor.angle);
        printf("as5600 angle = %f\n", angle);
        // printf("%f,%f, %f, %f\n", motor.foc_motor.shaft_angle, motor.foc_motor.target, motor.foc_motor.shaft_velocity, motor.foc_motor.voltage_q);
        // HAL_Delay(20);
        // printf("pid = %f,%f, %f\n", motor.pid->P, motor.pid->I, motor.pid->D);

        HAL_Delay(1000);
    }
}

float AS5600_getAngle_test(void)
{
    uint8_t as5600_readBuffer[2];
    int angle_buffer;
    float angle;
    HAL_I2C_Mem_Read(&as5600_i2c, AS5600_ADDRESS_RD, 0x0c, I2C_MEMADD_SIZE_8BIT, as5600_readBuffer, 2, HAL_MAX_DELAY);
    angle_buffer = as5600_readBuffer[0] << 8 | as5600_readBuffer[1];
    angle = (float)angle_buffer * ANGLE / 4096;
    return angle;
}
void test_openloop_velocity(void)
// void test_velocity_closedloop(void)
{

    // link to encoder and driver

    Encoder_init(&encoder);
    BLDCMotor_init(&motor, 7);
    BLDCDriver_init(&driver, 20e3, 4096, 12, 6);
    BLDCMotor_linkEncoder(&motor, &encoder);
    BLDCMotor_linkDriver(&motor, &driver);

    // BLDCDriver3PWM_init(&driver, 12, 6);
    // pwm_init(20e3, 4096);

    PIDController_init(&pid_velocity, 0.1, 0.0, 0, 5, 8);
    motor.pid = &pid_velocity;
    motor.foc_motor.controller = ControlType_velocity_openloop;
    motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;
    // motor.FOCMotor->controller = ControlType_velocity_openloop;
    // motor.FOCMotor->foc_modulation = FOCModulationType_SinePWM;

    motor.enable_Port = GPIOB;
    motor.enable_Pin = GPIO_PIN_15;
    motor.target = 2;

    // CurrentSense_Init(&currentSense);
    BLDCMotor_enable(&motor);
    BLDCMotor_setPhaseVoltage(&motor, 3, 1, _3PI_2);
    while (1)
    {

        // 1. encoder test
        // float angle = BLDCMotor_getAngle(&motor);
        // printf("angle = %f\n", angle);
        // delay_ms(1000);

        // 2. openloop test
        BLDCMotor_move(&motor, 1);
        // float angle = BLDCMotor_getAngle(&motor);
        // printf("angle = %f\n", angle);
        // PRINT_INFO("motor move");

        // 3. print driver info
        // PRINT_INFO("motor v = %f, motor v limit = %f, pwm Hz = %d, pwm resolution = %d", motor.driver->voltage_power_supply, motor.driver->voltage_limit, motor.driver->pwm_frequency,
        // motor.driver->pwm_resolution);
        // 4.
        // BLDCMotor_getElectricalAngle(&motor);
        printf("angle1 = %f, e_angle = %f\n", motor.angle, motor.e_angle);
    }
}

void test_velocity_closedloop_v0(void)
{
    motor.FOCMotor = &FOCMotor; // neccessary

    // motor.FOCMotor->encoder = ;
    // 1.
    Encoder_AS5600_Init(&as5600, &hi2c1);
    Encoder_linkAS5600(&encoder, &as5600);
    FOCMotor_linkEncoder(&FOCMotor, &encoder);

    // FOCMotor_linkAS5600(&FOCMotor, &as5600);
    //

    // BLDCMotor_linkAS5600(&motor, &as5600);

    // BLDCMotor_linkEncoder(&motor, &encoder);
    BLDCMotor_init(&motor, 7);

    BLDCDriver3PWM_init(&driver, 12, 6);
    BLDCMotor_linkDriver(&motor, &driver);
    pwm_init(20e3, 4096);

    PIDController_init(&pid_velocity, 0.1, 0.0, 0, 5, 8);
    motor.pid = &pid_velocity;
    // motor.foc_motor.controller = ControlType_velocity;
    motor.FOCMotor->controller = ControlType_velocity_openloop;
    motor.FOCMotor->foc_modulation = FOCModulationType_SinePWM;

    motor.enable_Port = GPIOB;
    motor.enable_Pin = GPIO_PIN_15;
    motor.target = 2;

    // CurrentSense_Init(&currentSense);
    BLDCMotor_enable(&motor);
    // timer_init(&htim3, 100);
    // BLDCMotor_setPhaseVoltage(&motor, 3, 0, _3PI_2);
    while (1)
    {

        // BLDCMotor_loopFOC(&motor);
        // float angle = FOCMotor_shaftAngle(&FOCMotor);
        // float angle = getAngle();
        // float vel = getVelocity();
        // float angle = Encoder_getAngle(&encoder);
        // float vel = FOCMotor_shaftVelocity(&motor.foc_motor);
        // float vel = BLDCMotor_getVelocity(&motor);
        // printf("vel = %f\n", motor.velocity);
        // printf("target = %f\n", motor.target);
        // printf("angle = %f\n", motor.angle);
        // printf("%f,%f, %f, %f\n", motor.foc_motor.shaft_angle, motor.foc_motor.target, motor.foc_motor.shaft_velocity, motor.foc_motor.voltage_q);
        // HAL_Delay(20);
        // printf("pid = %f,%f, %f\n", motor.pid->P, motor.pid->I, motor.pid->D);

        // 1.
        // BLDCMotor_move(&motor, 1);
        // float angle = BLDCMotor_getAngle(&motor);
        // float vel = BLDCMotor_getVelocity(&motor);
        // printf("as5600 angle = %f\n", angle);
        // printf("vel = %f\n", vel);

        // 2.
        // printf("target\n");
        // BLDCMotor_move(&motor, 2);
        // _delay(20);
        // motor.angle = BLDCMotor_getAngle(&motor);
        // printf("targe = %f\n", motor.target);
        // printf("angle = %f\n", motor.angle);

        // 3.
        // motor.angle = BLDCMotor_getAngle(&motor);
        // motor.angle = FOCMotor_shaftAngle(&FOCMotor);
        // printf("angle1 = %f\n", motor.angle);
        // HAL_Delay(1000);

        // last. test
        // printf("v limit = %f\n", motor.driver->voltage_limit);
        // 4.
        float angle = Encoder_getAngle(&encoder);
        printf("angle1 = %f\n", angle);
    }
}
void test_encoder(void)
{
    // 1.
    // Encoder_AS5600_Init(&as5600, &hi2c1);
    // encoder.as5600 = &as5600;

    // 2. focmotor use encoder
    // FOCMotor_linkEncoder(&FOCMotor, &encoder);

    // 3. bldcmotor use encoder
    BLDCMotor_linkEncoder(&motor, &encoder);
    Encoder_init(&encoder);
    while (1)
    {
        // 1.
        //  float angle = Encoder_getAngle(&encoder);
        //  printf("angle0 = %f\n", angle);
        //  _delay(1000);

        // 2. focmotor use encoder
        // float angle = FOCMotor_shaftAngle(&FOCMotor);
        // printf("angle0 = %f\n", angle);
        // _delay(1000);

        // 3. bldcmotor use encoder
        float angle = BLDCMotor_getAngle(&motor);
        printf("angle0 = %f\n", angle);
        _delay(1000);
    }
}

void test_current_closedloop_v0(void)
{
    // link to encoder and driver

    Encoder_init(&encoder);
    BLDCMotor_init(&motor, 7);
    BLDCDriver_init(&driver, 20e3, 4096, 12, 6);
    BLDCMotor_linkEncoder(&motor, &encoder);
    BLDCMotor_linkDriver(&motor, &driver);

    // BLDCDriver3PWM_init(&driver, 12, 6);
    // pwm_init(20e3, 4096);

    PIDController_init(&pid_velocity, 0.2, 0.0, 0, 5, 8);
    motor.pid = &pid_velocity;
    // motor.foc_motor.controller = ControlType_currentClosedLoop;
    motor.foc_motor.controller = ControlType_velocity_openloop;
    motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;

    motor.enable_Port = GPIOB;
    motor.enable_Pin = GPIO_PIN_15;
    motor.target = 2;

    CurrentSense_Init(&currentSense);
    BLDCMotor_enable(&motor);
    LowPassFilter_init(&lpf_a, 1);
    LowPassFilter_init(&lpf_b, 1);
    // BLDCMotor_setPhaseVoltage(&motor, 3, 1, _3PI_2);
    float i_a, i_b, i_alpha, i_beta, i_q, i_d;
    while (1)
    {

        // 1. encoder test
        // float angle = BLDCMotor_getAngle(&motor);
        // printf("angle = %f\n", angle);
        // delay_ms(1000);
        // float vel = BLDCMotor_getVelocityRPM(&motor);
        // printf("vel = %f rpm\n", vel);

        // 2. openloop test
        // BLDCMotor_move(&motor, 1);
        // float angle = BLDCMotor_getAngle(&motor);
        // printf("angle = %f\n", angle);
        // PRINT_INFO("motor move");

        // 3. print driver info
        // PRINT_INFO("motor v = %f, motor v limit = %f, pwm Hz = %d, pwm resolution = %d", motor.driver->voltage_power_supply, motor.driver->voltage_limit, motor.driver->pwm_frequency,
        // motor.driver->pwm_resolution);
        // 4. velocityClosedLoop
        BLDCMotor_move(&motor, 1);
        // delay_ms(50);
        // PRINT_INFO("velocity = %f", motor.velocity);
        //
        // // 5. print pid parameter
        // PRINT_INFO("p = %f, i = %f, d = %f", motor.pid->P, motor.pid->I, motor.pid->D);
        // PRINT_INFO("output v = %f", motor.foc_motor.voltage_q);

        // 6. test current sample
        // CurrentSense_get3Current(&currentSense);
        // printf("%f, %f, %f\n", currentSense.i3[0], currentSense.i3[1], currentSense.i3[2]);

        // 7. test current sample
        CurrentSense_get3Current(&currentSense);
        i_a = currentSense.i3[0];
        i_b = currentSense.i3[1];
        // i_a = LowPassFilter(&lpf_a, i_a);
        // i_b = LowPassFilter(&lpf_b, i_b);
        // 7.1 test angle
        // float angle = _normalizeAngle(_electricalAngle(motor.angle, motor.foc_motor.pole_pairs));
        // 7.2
        // printf("angle = %f\n", motor.e_angle);
        // delay_ms(20);
        // printf("%f, %f\n", i_a, i_b);
        //
        // motor.angle = BLDCMotor_getAngle(&motor);
        Clarke_Transform(i_a, i_b, &i_alpha, &i_beta);
        Park_Transform(i_alpha, i_beta, motor.e_angle, &i_d, &i_q);
        i_d = LowPassFilter(&lpf_a, i_d);
        i_q = LowPassFilter(&lpf_b, i_q);
        printf("%f, %f\n", i_d, i_q);
    }
}
void test_current_closedloop(void)
{
    // link to encoder and driver
    printf("===========================\n");
    PRINT_INFO("Current Closed Loop");
    printf("===========================\n");

    Encoder_init(&encoder);
    BLDCMotor_init(&motor, 7);
    BLDCDriver_init(&driver, 20e3, 4096, 12, 6);
    BLDCMotor_linkEncoder(&motor, &encoder);
    BLDCMotor_linkDriver(&motor, &driver);

    // BLDCDriver3PWM_init(&driver, 12, 6);
    // pwm_init(20e3, 4096);

    // PIDController_init(&pid_velocity, 0.2, 0.0, 0, 5, 8);
    PIDController_init(&pid_id, 8, 0.3, 0, 50, 6);
    PIDController_init(&pid_iq, 8, 0.5, 0, 50, 6);
    // motor.pid = &pid_velocity;
    motor.foc_motor.PID_current_d = &pid_id;
    motor.foc_motor.PID_current_q = &pid_iq;
    motor.foc_motor.controller = ControlType_currentClosedLoop;
    // motor.foc_motor.controller = ControlType_velocity_openloop;
    motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;
    // motor.foc_motor.foc_modulation = FOCModulationType_SinePWM;

    motor.enable_Port = GPIOB;
    motor.enable_Pin = GPIO_PIN_15;
    motor.target = 2;

    BLDCMotor_enable(&motor);

    motor.currentSense = &currentSense;
    CurrentSense_Init(motor.currentSense);
    BLDCMotor_setPhaseVoltage(&motor, 2, 0, 0);
    BLDCMotor_disable(&motor);
    PRINT_INFO("please enable motor!!!!");
    // Debug_Transformations();
    while (1)
    {

        // 1.
        if (motor.isEnable)
        {
            // printf("motor is enabled\n");
            // 1.
            BLDCMotor_move(&motor, 0.2);
            // delay_ms(10);
            // 1.1
            // printf(">");
            // printf("target:%f,iq:%f,id:%f,", motor.target, motor.foc_motor.i_q, motor.foc_motor.i_d);
            // printf("\r\n");
            // 1.2
            // static int i = 0;
            // if (i > 500)
            // {
            //     printf(">");
            //     printf("t:%f,iq:%f,id:%f,", motor.target, motor.foc_motor.i_q, motor.foc_motor.i_d);
            //     printf("vq:%f,vd:%f", motor.foc_motor.voltage_q, motor.foc_motor.voltage_d);
            //     printf("\r\n");
            //     i = 0;
            // }
            // i++;
            // 2.

            // BLDCMotor_getElectricalAngle(&motor);
            // BLDCMotor_getCurrentDQ(&motor);
            // printf(">");
            // printf("iq:%f,id:%f", motor.foc_motor.i_q, motor.foc_motor.i_d);
            // printf("\r\n");
        }
        // delay_ms(10);
        // delay_ms(10);
        // BLDCMotor_getCurrentDQ(&motor);
        // float id = motor.foc_motor.i_d;
        // float iq = motor.foc_motor.i_q;
        // printf("%f, %f\n", id, iq);
        //
        // 2.
        // BLDCMotor_getElectricalAngle(&motor);
        // printf(">angle:%f,e_angle:%f, normal_angle:%f\r\n", motor.angle, motor.e_angle, _normalizeAngle(motor.e_angle));
        // delay_ms(1000);

        // 7. test current sample
    }
}

void test_velocity_closedloop(void)
{
    Encoder_init(&encoder);
    BLDCMotor_init(&motor, 7);
    BLDCDriver_init(&driver, 20e3, 4096, 12, 6);
    BLDCMotor_linkEncoder(&motor, &encoder);
    BLDCMotor_linkDriver(&motor, &driver);

    // BLDCDriver3PWM_init(&driver, 12, 6);
    // pwm_init(20e3, 4096);

    PIDController_init(&pid_velocity, 0.05, 0.0, 0, 5, 8);
    PIDController_init(&pid_id, 0.2, 0.0, 0, 5, 6);
    PIDController_init(&pid_iq, 2, 0.5, 0, 5, 6);
    motor.foc_motor.PID_velocityLoop = &pid_velocity;
    motor.foc_motor.PID_current_d = &pid_id;
    motor.foc_motor.PID_current_q = &pid_iq;
    // motor.foc_motor.controller = ControlType_currentClosedLoop;
    motor.foc_motor.controller = ControlType_velocityClosedLoop;
    // motor.foc_motor.controller = ControlType_velocity_openloop;
    motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;

    motor.enable_Port = GPIOB;
    motor.enable_Pin = GPIO_PIN_15;
    motor.target = 2;

    BLDCMotor_enable(&motor);

    motor.currentSense = &currentSense;
    CurrentSense_Init(motor.currentSense);
    BLDCMotor_setPhaseVoltage(&motor, 2, 0, _3PI_2);
    while (1)
    {

        // 1.
        BLDCMotor_move(&motor, 60);
        printf(">");
        printf("vq:%f,vd:%f,", motor.foc_motor.voltage_q, motor.foc_motor.voltage_d);
        printf("vel_t:%f,vel:%f,", motor.target, motor.velocity);
        printf("e_angle:%f,", motor.e_angle);
        printf("angle:%f,", motor.angle);
        printf("\r\n");
        // 2.
        // BLDCMotor_getElectricalAngle(&motor);
        // printf("angle:%f,\n", motor.angle);
    }
}

void test_velocity_closedloop_without_i(void)
{
    Encoder_init(&encoder);
    BLDCMotor_init(&motor, 7);
    BLDCDriver_init(&driver, 20e3, 4096, 12, 6);
    BLDCMotor_linkEncoder(&motor, &encoder);
    BLDCMotor_linkDriver(&motor, &driver);

    // BLDCDriver3PWM_init(&driver, 12, 6);
    // pwm_init(20e3, 4096);

    PIDController_init(&pid_velocity, 0.05, 0.005, 0, 50, 6);
    PIDController_init(&pid_id, 0.2, 0.0, 0, 5, 6);
    PIDController_init(&pid_iq, 2, 0.5, 0, 5, 6);
    motor.foc_motor.PID_velocityLoop = &pid_velocity;
    motor.foc_motor.PID_current_d = &pid_id;
    motor.foc_motor.PID_current_q = &pid_iq;
    // motor.foc_motor.controller = ControlType_currentClosedLoop;
    motor.foc_motor.controller = ControlType_velocity;
    // motor.foc_motor.controller = ControlType_velocity_openloop;
    motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;

    motor.enable_Port = GPIOB;
    motor.enable_Pin = GPIO_PIN_15;
    motor.target = 2;

    BLDCMotor_enable(&motor);

    motor.currentSense = &currentSense;
    CurrentSense_Init(motor.currentSense);
    BLDCMotor_setPhaseVoltage(&motor, 2, 0, _3PI_2);

    BLDCMotor_disable(&motor);
    while (1)
    {

        // 1.
        if (motor.isEnable)
        {
            BLDCMotor_move(&motor, 180);
            delay_ms(5);
            printf(">");
            printf("vq:%f,vd:%f,", motor.foc_motor.voltage_q, motor.foc_motor.voltage_d);
            printf("vel_t:%f,vel:%f,", motor.target, motor.velocity);
            printf("e_angle:%f,", motor.e_angle);
            printf("angle:%f,", motor.angle);
            printf("\r\n");
        }
        // 2.
        // BLDCMotor_getElectricalAngle(&motor);
        // printf("angle:%f,\n", motor.angle);
    }
}
void test_hallSensor_57BLDC(void)
{
    // link to encoder and driver
    printf("===========================\n");
    PRINT_INFO("Hallsensor");
    printf("===========================\n");

    Encoder_init(&encoder);
    BLDCMotor_init(&motor, 2);
    BLDCDriver_init(&driver, 20e3, 4096, 24, 2);
    BLDCMotor_linkEncoder(&motor, &encoder);
    BLDCMotor_linkDriver(&motor, &driver);

    // BLDCDriver3PWM_init(&driver, 12, 6);
    // pwm_init(20e3, 4096);

    // PIDController_init(&pid_velocity, 0.2, 0.0, 0, 5, 8);
    PIDController_init(&pid_id, 8, 0.3, 0, 50, 6);
    PIDController_init(&pid_iq, 8, 0.5, 0, 50, 6);
    // motor.pid = &pid_velocity;
    motor.foc_motor.PID_current_d = &pid_id;
    motor.foc_motor.PID_current_q = &pid_iq;
    // motor.foc_motor.controller = ControlType_currentClosedLoop;
    motor.foc_motor.controller = ControlType_velocity_openloop;
    motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;
    // motor.foc_motor.foc_modulation = FOCModulationType_SinePWM;

    motor.enable_Port = GPIOB;
    motor.enable_Pin = GPIO_PIN_15;
    motor.target = 2;

    BLDCMotor_enable(&motor);

    motor.currentSense = &currentSense;
    CurrentSense_Init(motor.currentSense);
    BLDCMotor_setPhaseVoltage(&motor, 2, 0, 0);
    BLDCMotor_disable(&motor);
    PRINT_INFO("please enable motor!!!!");
    HallSensor_init();
    // Debug_Transformations();
    while (1)
    {

        // 1.
        if (motor.isEnable)
        {
            // printf("motor is enabled\n");
            // 1.
            BLDCMotor_move(&motor, 20);
            // delay_ms(10);
            // 1.1
            // printf(">");
            // printf("target:%f,iq:%f,id:%f,", motor.target, motor.foc_motor.i_q, motor.foc_motor.i_d);
            // printf("\r\n");
            // 1.2
            // static int i = 0;
            // if (i > 500)
            // {
            //     printf(">");
            //     printf("t:%f,iq:%f,id:%f,", motor.target, motor.foc_motor.i_q, motor.foc_motor.i_d);
            //     printf("vq:%f,vd:%f", motor.foc_motor.voltage_q, motor.foc_motor.voltage_d);
            //     printf("\r\n");
            //     i = 0;
            // }
            // i++;
            // 2.

            // BLDCMotor_getElectricalAngle(&motor);
            // BLDCMotor_getCurrentDQ(&motor);
            // printf(">");
            // printf("iq:%f,id:%f", motor.foc_motor.i_q, motor.foc_motor.i_d);
            // printf("\r\n");
        }
        // delay_ms(10);
        // delay_ms(10);
        // BLDCMotor_getCurrentDQ(&motor);
        // float id = motor.foc_motor.i_d;
        // float iq = motor.foc_motor.i_q;
        // printf("%f, %f\n", id, iq);
        //
        // 2.
        // BLDCMotor_getElectricalAngle(&motor);
        // printf(">angle:%f,e_angle:%f, normal_angle:%f\r\n", motor.angle, motor.e_angle, _normalizeAngle(motor.e_angle));
        // delay_ms(1000);

        // 7. test current sample
    }
}
