/*
 * FOCMotor.c
 * Ported from:
 * https://github.com/simplefoc/Arduino-FOC/blob/master/src/common/base_classes/FOCMotor.cpp
 * Converted from C++ to C
 */

#include "FOCMotor.h"
#include "Encoder.h"
#include <stdio.h>

/**
 * Default initialization - setting all variables to default values
 */
void FOCMotor_init(FOCMotor_t *motor) {
  if (!motor)
    return;

  // maximum angular velocity to be used for positioning
  motor->velocity_limit = DEF_VEL_LIM;
  // maximum voltage to be set to the motor
  motor->voltage_limit = DEF_POWER_SUPPLY;

  // index search velocity
  motor->velocity_index_search = DEF_INDEX_SEARCH_TARGET_VELOCITY;
  // sensor and motor align voltage
  motor->voltage_sensor_align = DEF_VOLTAGE_SENSOR_ALIGN;

  // electric angle of the zero angle
  motor->zero_electric_angle = 0;

  // default modulation is SinePWM
  motor->foc_modulation = FOCModulationType_SinePWM;

  // default target value
  motor->target = 0;
  motor->voltage_d = 0;
  motor->voltage_q = 0;

  // state variables initialization
  motor->shaft_angle = 0;
  motor->shaft_velocity = 0;
  motor->shaft_velocity_sp = 0;
  motor->shaft_angle_sp = 0;

  // motor configuration parameters
  motor->pole_pairs = 0;

  // default control type
  motor->controller = ControlType_voltage;

  // Initialize PID controller with default values
  PIDController_init(&motor->PID_velocity, DEF_PID_VEL_P, DEF_PID_VEL_I,
                     DEF_PID_VEL_D, DEF_PID_VEL_U_RAMP, DEF_POWER_SUPPLY);

  // Initialize P controller for angle with default values
  PIDController_init(&motor->P_angle, DEF_P_ANGLE_P, 0, 0, 1e10, DEF_VEL_LIM);

  // Initialize low pass filter with default values
  LowPassFilter_init(&motor->LPF_velocity, DEF_VEL_FILTER_Tf);

  // sensor
  motor->sensor = NULL;
}

/**
 * Sensor communication methods
 */
void FOCMotor_linkSensor(FOCMotor_t *motor, Sensor_t *_sensor) {
  if (!motor)
    return;
  motor->sensor = _sensor;
  printf("linked to sensor\n");
}

// shaft angle calculation
float FOCMotor_shaftAngle(FOCMotor_t *motor) {
  if (!motor)
    return 0;
  // if no sensor linked return stored shaft_angle
  if (!motor->sensor)
    return motor->shaft_angle;
  return Encoder_getAngle(motor->sensor);
}

// shaft velocity calculation
float FOCMotor_shaftVelocity(FOCMotor_t *motor) {
  if (!motor)
    return 0;
  // if no sensor linked return 0
  if (!motor->sensor)
    return 0;
  return LowPassFilter_calculate(&motor->LPF_velocity,
                                 Sensor_getVelocity(motor->sensor));
}

// Virtual function placeholders - these should be implemented by derived
// structs Default implementations that do nothing
void FOCMotor_init_hardware(FOCMotor_t *motor) {
  // This is a virtual function in C++ - should be implemented by BLDCMotor
  (void)motor; // suppress unused parameter warning
}

void FOCMotor_disable(FOCMotor_t *motor) {
  // This is a virtual function in C++ - should be implemented by BLDCMotor
  (void)motor; // suppress unused parameter warning
}

void FOCMotor_enable(FOCMotor_t *motor) {
  // This is a virtual function in C++ - should be implemented by BLDCMotor
  (void)motor; // suppress unused parameter warning
}

int FOCMotor_initFOC(FOCMotor_t *motor, float zero_electric_offset,
                     Direction_t sensor_direction) {
  // This is a virtual function in C++ - should be implemented by BLDCMotor
  (void)motor;
  (void)zero_electric_offset;
  (void)sensor_direction;
  return 0;
}

void FOCMotor_loopFOC(FOCMotor_t *motor) {
  // This is a virtual function in C++ - should be implemented by BLDCMotor
  (void)motor; // suppress unused parameter warning
}

void FOCMotor_move(FOCMotor_t *motor, float new_target) {
  // This is a virtual function in C++ - should be implemented by BLDCMotor
  (void)motor;
  (void)new_target;
}
