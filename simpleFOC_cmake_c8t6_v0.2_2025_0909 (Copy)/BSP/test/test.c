#include "test.h"
// #include "BLDCMotor.h"
// #include "Encoder.h"
// #include "FOCMotor.h"
#include "BLDCDriver.h"
#include "BLDCMotor.h"
#include "as5600.h"
#include "encoder.h"
#include "foc_base.h"
#include "foc_motor.h"
#include "led.h"
// #include "motor/Inc/BLDCMotor.h"
#include "encoder_as5600.h"
#include "open_loop_FOC.h"
#include "sensor.h"
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
void test_alignSensor_struct(void) {
  BLDCMotor_t motor;
  BLDCDriver_t driver;
  BLDCMotor_init(&motor, 7);
  BLDCDriver3PWM_init(&driver, 12, 6);
  BLDCMotor_linkDriver(&motor, &driver);

  printf("pp = %d\n", motor.foc_motor.pole_pairs);
  printf("power = %f v\n", motor.driver->voltage_power_supply);
  printf("foc foc_modulation = %f\n", motor.foc_motor.foc_modulation);

  driver.functions->enable(&driver);
  BLDCMotor_setPhaseVoltage(&motor, 3, 0, _3PI_2);
  // setTorque(3, _3PI_2);
}

void test_struct_openloop_velocity(void) {
  BLDCMotor_t motor;
  BLDCDriver_t driver;
  Sensor_t sensor;
  BLDCMotor_init(&motor, 7);
  BLDCDriver3PWM_init(&driver, 12, 6);
  BLDCMotor_linkDriver(&motor, &driver);
  FOCMotor_linkSensor(&motor, &sensor);

  motor.foc_motor.controller = ControlType_velocity_openloop;
  // motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;

  printf("pp = %d\n", motor.foc_motor.pole_pairs);
  printf("power = %f v\n", motor.driver->voltage_power_supply);
  printf("foc foc_modulation = %f\n", motor.foc_motor.foc_modulation);

  driver.functions->enable(&driver);
  BLDCMotor_setPhaseVoltage(&motor, 3, 0, _3PI_2);
  _delay(1000);
  while (1) {
    BLDCMotor_move(&motor, 1);
  }
  // setTorque(3, _3PI_2);
}
void test_as5600_new(void) {
  AS5600_Example_Init();
  _delay(2000);
  while (1) {
    AS5600_Example_ReadAngle();
    AS5600_Example_ReadSpeed();
    _delay(1000);
  }
}
void test_as5600_vel1(void) {
  BLDCMotor_t motor;
  BLDCDriver_t driver;
  Sensor_t sensor;
  BLDCMotor_init(&motor, 7);
  BLDCDriver3PWM_init(&driver, 12, 6);
  BLDCMotor_linkDriver(&motor, &driver);
  FOCMotor_linkSensor(&motor, &sensor);

  motor.foc_motor.controller = ControlType_velocity_openloop;
  // motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;

  printf("pp = %d\n", motor.foc_motor.pole_pairs);
  printf("power = %f v\n", motor.driver->voltage_power_supply);
  printf("foc foc_modulation = %f\n", motor.foc_motor.foc_modulation);

  driver.functions->enable(&driver);
  BLDCMotor_setPhaseVoltage(&motor, 3, 0, _3PI_2);
  AS5600_Example_Init();
  _delay(1000);
  while (1) {
    BLDCMotor_move(&motor, 1);
    AS5600_Example_ReadAngle();
    AS5600_Example_ReadSpeed();
  }
}
void test_as5600_shaftAngle(void) {
  extern I2C_HandleTypeDef hi2c2;
  BLDCMotor_t motor;
  BLDCDriver_t driver;
  Sensor_t sensor;
  Encoder_t encoder;
  AS5600_t as5600_0;
  BLDCMotor_init(&motor, 7);
  BLDCDriver3PWM_init(&driver, 12, 10);
  BLDCMotor_linkDriver(&motor, &driver);
  FOCMotor_linkSensor(&motor, &sensor);

  motor.foc_motor.controller = ControlType_velocity_openloop;
  // motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;

  printf("pp = %d\n", motor.foc_motor.pole_pairs);
  printf("power = %f v\n", motor.driver->voltage_power_supply);
  printf("foc foc_modulation = %f\n", motor.foc_motor.foc_modulation);

  driver.functions->enable(&driver);
  BLDCMotor_setPhaseVoltage(&motor, 3, 0, _3PI_2);
  Encoder_AS5600_Init(&encoder.as5600, &hi2c2);
  _delay(1000);
  Encoder_AS5600_ReadAngle(&encoder.as5600);
  while (1) {
    // Encoder_AS5600_ReadAngle(&encoder.as5600);
    // Encoder_AS5600_ReadAngle(as5600_0);
    BLDCMotor_move(&motor, 5);
    float angle = Encoder_getAngle(&encoder);
    printf("as5600 angle = %f\n", angle);
    float vel = Encoder_getVelocity(&encoder);
    printf("as5600 vel = %f\n", vel);
    // _delay(1000);
    // Encoder_AS5600_ReadAngle(as5600_0);
  }
}

void test_pid(void) {
  extern I2C_HandleTypeDef hi2c2;
  BLDCMotor_t motor;
  BLDCDriver_t driver;
  Sensor_t sensor;
  Encoder_t encoder;
  BLDCMotor_init(&motor, 7);
  BLDCDriver3PWM_init(&driver, 12, 5);
  BLDCMotor_linkDriver(&motor, &driver);
  FOCMotor_linkSensor(&motor, &sensor);
  FOCMotor_linkEncoder(&motor, &encoder);

  motor.foc_motor.controller = ControlType_velocity;
  // motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;

  printf("pp = %d\n", motor.foc_motor.pole_pairs);
  printf("power = %f v\n", motor.driver->voltage_power_supply);
  printf("foc foc_modulation = %f\n", motor.foc_motor.foc_modulation);

  driver.functions->enable(&driver);
  BLDCMotor_setPhaseVoltage(&motor, 3, 0, _3PI_2);
  Encoder_AS5600_Init(&encoder.as5600, &hi2c2);
  _delay(1000);
  Encoder_AS5600_ReadAngle(&encoder.as5600);
  while (1) {
    // Encoder_AS5600_ReadAngle(&encoder.as5600);
    // Encoder_AS5600_ReadAngle(as5600_0);

    BLDCMotor_loopFOC(&motor);
    BLDCMotor_move(&motor, 1);
    // float angle = Encoder_getAngle(&encoder);
    // printf("as5600 angle = %f\n", angle);
    // float vel = Encoder_getVelocity(&encoder);
    // printf("as5600 vel = %f\n", vel);
    _delay(100);
    // Encoder_AS5600_ReadAngle(as5600_0);
  }
}
