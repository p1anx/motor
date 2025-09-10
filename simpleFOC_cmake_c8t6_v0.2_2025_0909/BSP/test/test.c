#include "test.h"
// #include "BLDCMotor.h"
// #include "Encoder.h"
// #include "FOCMotor.h"
#include "BLDCDriver.h"
#include "as5600.h"
#include "foc_base.h"
#include "led.h"
#include "open_loop_FOC.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include <stdio.h>

void test_led(void) { led_blink(1000); }

void test_as5600_angle(void) {

  float angle = getAngle();
  printf("angle = %f\n", angle);
  HAL_Delay(100);
}

void test_torque(void) { setTorque(3, _PI_2); }

void test_velocityOpenLoop(void) {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
  velocityOpenloop(1);
}
#define LEN 3
void test_positionClosedLoop(void) {
  float target_angle[LEN] = {0, 90, 180};
  float kp = 0.133;
  float sensor_angle;

  float zero_electric_angle = 0;
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);

  setTorque(3, _3PI_2);
  HAL_Delay(1000);
  zero_electric_angle = _normalizeAngle(7 * getAngle() - zero_electric_angle);
  setTorque(0, _3PI_2);

  int i = 0;

  while (1) {

    sensor_angle = getAngle();

    setTorque(
        _constrain(kp * (target_angle[i] - sensor_angle) * 90 / _PI_2, -6, 6),
        _normalizeAngle(7 * getAngle() - zero_electric_angle));
    printf("now2 target_angle = %f\n", target_angle[i]);
    i++;
    if (i == LEN) {
      i = 0;
    }
    printf("now2 angle = %f\n", sensor_angle);
    printf("zero_electric_angle = %f\n", zero_electric_angle);

    HAL_Delay(1000);
  }
}

// void test_sensor(void) {
//   Encoder_t encoder; // these pins, and values are actually hard coded
//   Encoder_init(&encoder, 2, 3, 800, 0);
//   Encoder_getAngle(&encoder);
//   printf("angle = %f\n", encoder.angle);
// }

// void test_motor(void) {
//   BLDCMotor_t motor;
//   BLDCDriver_t driver;     // these pins are currently hard coded
//                            // encoder instance
//   Encoder_t encoder;       // these pins, and values are actually hard coded
//   float start_angle = 0.0; //[rad]
//   motor.foc_motor.voltage_limit = 10;
//   motor.foc_motor.controller = ControlType_angle;
//
//   Encoder_init(&encoder, 2, 3, 800, 0);
//   BLDCDriver3PWM_init(&driver, 9, 5, 6, 8);
//   BLDCDriver3PWM_enable(&driver);
//   BLDCMotor_init(&motor, 7);
//
//   // Encoder_init_hardware(&encoder);
//   FOCMotor_linkSensor(&motor.foc_motor, (Sensor_t *)&encoder);
//   driver.voltage_power_supply = 11;
//   // BLDCDriver3PWM_init_hardware(&driver);
//   BLDCMotor_linkDriver(&motor, (BLDCDriver_t *)&driver);
//   motor.foc_motor.voltage_sensor_align = 3;  //[V]
//   motor.foc_motor.velocity_index_search = 3; //[rad/s]
//   motor.foc_motor.controller = ControlType_angle;
//
//   // PIDController_init(&motor.foc_motor.PID_velocity, 0.2, 20, 0, 1000, 10);
//   motor.foc_motor.voltage_limit = 10;
//
//   // LowPassFilter_init(&motor.foc_motor.LPF_velocity, 0.01);
//   // PIDController_init(&motor.foc_motor.P_angle, 20, 0, 0, 0, 0);
//   motor.foc_motor.velocity_limit = 8;
//   motor.foc_motor.foc_modulation = FOCModulationType_SinePWM;
//
//   // BLDCMotor_init_hardware(&motor);
//   // BLDCMotor_initFOC(&motor, NOT_SET, Direction_CW);
//
//   _delay(1000);
//
//   BLDCMotor_setPhaseVoltage(&motor, 3, 0, _PI_2);
// }
void test_alignSensor_struct(void) {
  BLDCDriver3PWM_enable();
  alignSensor();
  float target_angle[LEN] = {0, 90, 180};
  float kp = 0.133;
  float sensor_angle;

  int i = 0;

  while (1) {

    sensor_angle = getAngle();
    setPhaseVoltage(
        _constrain(kp * (target_angle[i] - sensor_angle) * 90 / _PI_2, -6, 6),
        0, getElectricalAngle());

    // setTorque(
    //     _constrain(kp * (target_angle[i] - sensor_angle) * 90 / _PI_2,
    //     -6, 6), _normalizeAngle(7 * getAngle() - zero_electric_angle));
    printf("now2 target_angle = %f\n", target_angle[i]);
    i++;
    if (i == LEN) {
      i = 0;
    }
    printf("now2 angle = %f\n", sensor_angle);
    // printf("zero_electric_angle = %f\n", zero_electric_angle);

    HAL_Delay(1000);
  }
}

void test_alignSensor(void) {
  BLDCDriver3PWM_enable();
  alignSensor();
  float target_angle[LEN] = {0, 90, 180};
  float kp = 0.133;
  float sensor_angle;

  int i = 0;

  while (1) {

    sensor_angle = getAngle();
    setPhaseVoltage(
        _constrain(kp * (target_angle[i] - sensor_angle) * 90 / _PI_2, -6, 6),
        0, getElectricalAngle());

    // setTorque(
    //     _constrain(kp * (target_angle[i] - sensor_angle) * 90 / _PI_2,
    //     -6, 6), _normalizeAngle(7 * getAngle() - zero_electric_angle));
    printf("now2 target_angle = %f\n", target_angle[i]);
    i++;
    if (i == LEN) {
      i = 0;
    }
    printf("now2 angle = %f\n", sensor_angle);
    // printf("zero_electric_angle = %f\n", zero_electric_angle);

    HAL_Delay(1000);
  }
}
void test_pid(void) {
  BLDCDriver3PWM_enable();
  alignSensor();
  float target_angle[LEN] = {0, 90, 180};
  float kp = 0.133;
  float sensor_angle;

  int i = 0;

  while (1) {

    sensor_angle = getAngle();
    setPhaseVoltage(
        _constrain(kp * (target_angle[i] - sensor_angle) * 90 / _PI_2, -6, 6),
        0, getElectricalAngle());

    // setTorque(
    //     _constrain(kp * (target_angle[i] - sensor_angle) * 90 / _PI_2,
    //     -6, 6), _normalizeAngle(7 * getAngle() - zero_electric_angle));
    printf("now2 target_angle = %f\n", target_angle[i]);
    i++;
    if (i == LEN) {
      i = 0;
    }
    printf("now2 angle = %f\n", sensor_angle);
    // printf("zero_electric_angle = %f\n", zero_electric_angle);

    HAL_Delay(1000);
  }
}
