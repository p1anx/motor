//
// Created by xwj on 10/31/25.
//

#include "pwm.h"
#include "stm32_hal.h"
#include "BLDCMotor.h"
#include "ads1256.h"
#include <stdio.h>

BLDCMotor_t motor;
BLDCDriver_t driver;
FOCMotor_t FOCMotor;
Encoder_t encoder;
AS5600_t as5600;

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
    BLDCDriver_init(&driver, 20e3, 4096, 12, 6);
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
    BLDCMotor_setPhaseVoltage(&motor, 3, 1, _3PI_2);


    while (1)
    {
        BLDCMotor_move(&motor, 3);
        // ADS1256_printVoltage(&ads1256);
        ADS1256_get2Voltage(&ads1256);
        // printf("angle1 = %f, e_angle = %f\n", motor.angle, motor.e_angle);
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

    PIDController_init(&pid, 0.05, 0.0, 0, 5, 5);
    motor.pid = &pid;
    motor.foc_motor.controller = ControlType_velocityClosedLoop;
    motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;

    motor.enable_Port = GPIOC;
    motor.enable_Pin = GPIO_PIN_1;
    motor.target = 2;

    BLDCMotor_enable(&motor);
    delay_ms(2000);
    BLDCMotor_setPhaseVoltage(&motor, 3, 1, _3PI_2);


    while (1)
    {
        BLDCMotor_move(&motor, -30);
        printf("%f,%f, %f, %f, %f\n", motor.target, motor.velocity,motor.foc_motor.voltage_q, motor.angle, motor.e_angle);
        // ADS1256_printVoltage(&ads1256);
        //2.
        // ADS1256_get2Voltage(&ads1256);
        // printf("angle1 = %f, e_angle = %f\n", motor.angle, motor.e_angle);
        // motor.angle = BLDCMotor_getAngle(&motor);
        // HAL_Delay(1000);
    }
}

void test_pwm(void)
{
    PWM6_Init(1000, 4096);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
    while(1)
    {
        printf("test_motor\n");
        delay_ms(1000);

    }
}

void test_motor(void)
{
    //1.
    // test_openloop_v();
    //2.
    test_pid_vel();
}