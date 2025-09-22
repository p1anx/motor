#include "test.h"
#include "adc.h"
#include "foc_base.h"
#include "foc_motor.h"
#include "lowpass_filter.h"
#include "pid.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_uart.h"
#include "usart.h"
#include "led.h"
#include "BLDCMotor.h"
#include "BLDCDriver.h"
#include "currentSense.h"
#include "pwm.h"

void test_printf(void);
void test_led(void);

void test_3current(void);
void test_openloop_velocity(void);
void test_pwm(void);

void test_main(void)
{
    // test_led();
    // test_printf();
    // test_openloop_velocity();
    test_3current();
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
void test_openloop_velocity(void)
{
    BLDCMotor_t motor;
    BLDCDriver_t driver;
    Sensor_t sensor;
    CurrentSense_t currentSense;

    BLDCMotor_init(&motor, 7);
    BLDCDriver3PWM_init(&driver, 12, 6);
    BLDCMotor_linkDriver(&motor, &driver);
    motor.foc_motor.controller = ControlType_velocity_openloop;
    motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;
    motor.enable_Port = GPIOB;
    motor.enable_Pin = GPIO_PIN_15;

    CurrentSense_Init(&currentSense);
    pwm_init(20e3, 4096);
    BLDCMotor_enable(&motor);
    // BLDCMotor_setPhaseVoltage(&motor, 2, 0, _3PI_2);
    while (1)
    {
        BLDCMotor_move(&motor, 1);
        // CurrentSense_get3Current(&currentSense);
        // // printf("offset = %f, %f, %f\n", currentSense.offset3[0], currentSense.offset3[1], currentSense.offset3[2]);
        // printf("%f, %f, %f\n", currentSense.i3[0], currentSense.i3[1], currentSense.i3[2]);
    }
}
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

    PIDController_init(&pid_q, 1, 0.2, 0, 6, 5);
    PIDController_init(&pid_d, 1, 0.0, 0, 6, 3);

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
