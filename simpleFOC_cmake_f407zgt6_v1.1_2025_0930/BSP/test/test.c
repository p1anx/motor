#include "test.h"
#include "adc.h"
#include "encoder.h"
#include "encoder_as5600.h"
#include "foc_base.h"
#include "foc_motor.h"
#include "i2c.h"
#include "lowpass_filter.h"
#include "pid.h"
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

#define ANGLE _2PI

BLDCMotor_t motor;
BLDCDriver_t driver;
FOCMotor_t FOCMotor;
Encoder_t encoder;
AS5600_t as5600;
PIDController pid_velocity;
void test_printf(void);
void test_led(void);

void test_encoder(void);
void test_3current(void);
void test_openloop_velocity(void);
void test_pwm(void);
void test_as5600(void);

void test_velocity_closedloop(void);
void test_main(void)
{
    // test_led();
    // test_printf();
    // test_3current();
    // test_as5600();
    // test_openloop_velocity();
    // test_velocity_closedloop();
    test_encoder();
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
    // FOCMotor_init(&foc_motor);
    // Encoder_AS5600_Init(&as5600, &hi2c1);
    // Encoder_linkAS5600(&encoder, &as5600);
    // FOCMotor_linkEncoder(&FOCMotor, &encoder);
    BLDCMotor_linkAS5600(&motor, &as5600);

    motor.FOCMotor = &FOCMotor; // neccessary
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
        BLDCMotor_move(&motor, 2);
        _delay(20);
        // motor.angle = BLDCMotor_getAngle(&motor);
        motor.angle = AS5600_getAngle_test();
        printf("targe = %f\n", motor.target);
        printf("angle = %f\n", motor.angle);

        // last. test
        // printf("v limit = %f\n", motor.driver->voltage_limit);
        // HAL_Delay(1000);
    }
}

void test_velocity_closedloop(void)
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
