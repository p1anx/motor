/*
 * BLDCMotor.c
 * Ported from:
 * https://github.com/simplefoc/Arduino-FOC/blob/master/src/BLDCMotor.cpp
 * Converted from C++ to C
 */

#ifndef SRC_BLDCMOTOR_C_
#define SRC_BLDCMOTOR_C_

#include "BLDCMotor.h"
#include "BLDCDriver3PWM.h"
#include <stdio.h>

// BLDCMotor initialization
// - pp            - pole pair number
static void BLDCMotor_init(BLDCMotor_t *motor, int pp) {
  if (!motor)
    return;

  // Initialize base FOCMotor
  FOCMotor_init(&motor->foc_motor);

  // save pole pairs number
  motor->foc_motor.pole_pairs = pp;

  // Initialize phase voltages
  motor->Ua = 0;
  motor->Ub = 0;
  motor->Uc = 0;
  motor->Ualpha = 0;
  motor->Ubeta = 0;

  // Initialize driver pointer
  motor->driver = NULL;

  // Initialize open loop timestamp
  motor->open_loop_timestamp = 0;
}

/**
 * Link the driver which controls the motor
 */
void BLDCMotor_linkDriver(BLDCMotor_t *motor, BLDCDriver_t *_driver) {
  if (!motor)
    return;
  motor->driver = _driver;
  printf("linked to driver\n");
}

// init hardware pins
void BLDCMotor_init_hardware(BLDCMotor_t *motor) {
  if (!motor || !motor->driver)
    return;

  //  if(monitor_port) monitor_port->println("MOT: Initialise variables.");
  // sanity check for the voltage limit configuration
  if (motor->foc_motor.voltage_limit > motor->driver->voltage_limit) {
    motor->foc_motor.voltage_limit = motor->driver->voltage_limit;
  }
  // constrain voltage for sensor alignment
  if (motor->foc_motor.voltage_sensor_align > motor->foc_motor.voltage_limit) {
    motor->foc_motor.voltage_sensor_align = motor->foc_motor.voltage_limit;
  }
  // update the controller limits
  motor->foc_motor.PID_velocity.limit = motor->foc_motor.voltage_limit;
  motor->foc_motor.P_angle.limit = motor->foc_motor.velocity_limit;

  _delay(500);
  // enable motor
  //  if(monitor_port) monitor_port->println("MOT: Enable driver.");
  BLDCMotor_enable(motor);
  _delay(500);
}

// disable motor driver
void BLDCMotor_disable(BLDCMotor_t *motor) {
  if (!motor || !motor->driver)
    return;

  // set zero to PWM
  BLDCDriver_setPwm(motor->driver, 0, 0, 0);
  // disable the driver
  BLDCDriver_disable(motor->driver);
}

// enable motor driver
void BLDCMotor_enable(BLDCMotor_t *motor) {
  if (!motor || !motor->driver)
    return;

  // enable the driver
  BLDCDriver_enable(motor->driver);
  // set zero to PWM
  BLDCDriver_setPwm(motor->driver, 0, 0, 0);
}

/**
  FOC functions
*/
// FOC initialization function
int BLDCMotor_initFOC(BLDCMotor_t *motor, float zero_electric_offset,
                      Direction_t sensor_direction) {
  if (!motor)
    return 0;

  int exit_flag = 1;
  // align motor if necessary
  // alignment necessary for encoders!
  if (zero_electric_offset != NOT_SET) {
    // absolute zero offset provided - no need to align
    motor->foc_motor.zero_electric_angle = zero_electric_offset;
    // set the sensor direction - default CW
    if (motor->foc_motor.sensor) {
      motor->foc_motor.sensor->natural_direction = sensor_direction;
    }
  } else {
    // sensor and motor alignment
    _delay(500);
    exit_flag = BLDCMotor_alignSensor(motor);
    _delay(500);
  }
  //  if(monitor_port) monitor_port->println("MOT: Motor ready.");

  return exit_flag;
}

// Encoder alignment to electrical 0 angle
int BLDCMotor_alignSensor(BLDCMotor_t *motor) {
  if (!motor)
    return 0;

  //  if(monitor_port) monitor_port->println("MOT: Align sensor.");
  // align the electrical phases of the motor and sensor
  // set angle -90 degrees

  float start_angle = FOCMotor_shaftAngle(&motor->foc_motor);
  for (int i = 0; i <= 5; i++) {
    float angle = _3PI_2 + _2PI * i / 6.0f;
    BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_sensor_align, 0,
                              angle);
    _delay(200);
  }
  float mid_angle = FOCMotor_shaftAngle(&motor->foc_motor);
  for (int i = 5; i >= 0; i--) {
    float angle = _3PI_2 + _2PI * i / 6.0f;
    BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_sensor_align, 0,
                              angle);
    _delay(200);
  }
  if (mid_angle < start_angle) {
    //    if(monitor_port) monitor_port->println("MOT: natural_direction==CCW");
    if (motor->foc_motor.sensor) {
      motor->foc_motor.sensor->natural_direction = Direction_CCW;
    }
  } else if (mid_angle == start_angle) {
    //    if(monitor_port) monitor_port->println("MOT: Sensor failed to notice
    //    movement");
  } else {
    //    if(monitor_port) monitor_port->println("MOT: natural_direction==CW");
  }

  // let the motor stabilize for 2 sec
  _delay(2000);
  // set sensor to zero
  if (motor->foc_motor.sensor) {
    Sensor_initRelativeZero(motor->foc_motor.sensor);
  }
  _delay(500);
  BLDCMotor_setPhaseVoltage(motor, 0, 0, 0);
  _delay(200);

  // find the index if available
  int exit_flag = BLDCMotor_absoluteZeroAlign(motor);
  _delay(500);
  //  if(monitor_port){
  //    if(exit_flag< 0 ) monitor_port->println("MOT: Error: Not found!");
  //    if(exit_flag> 0 ) monitor_port->println("MOT: Success!");
  //    else  monitor_port->println("MOT: Not available!");
  //  }
  return exit_flag;
}

// Encoder alignment the absolute zero angle
// - to the index
int BLDCMotor_absoluteZeroAlign(BLDCMotor_t *motor) {
  if (!motor || !motor->foc_motor.sensor)
    return 0;

  //  if(monitor_port) monitor_port->println("MOT: Absolute zero align.");
  // if no absolute zero return
  if (!Sensor_hasAbsoluteZero(motor->foc_motor.sensor))
    return 0;

  //  if(monitor_port && sensor->needsAbsoluteZeroSearch())
  //  monitor_port->println("MOT: Searching...");
  // search the absolute zero with small velocity
  while (Sensor_needsAbsoluteZeroSearch(motor->foc_motor.sensor) &&
         motor->foc_motor.shaft_angle < _2PI) {
    BLDCMotor_loopFOC(motor);
    motor->foc_motor.voltage_q =
        PIDController_calculate(&motor->foc_motor.PID_velocity,
                                motor->foc_motor.velocity_index_search -
                                    FOCMotor_shaftVelocity(&motor->foc_motor));
  }
  motor->foc_motor.voltage_q = 0;
  // disable motor
  BLDCMotor_setPhaseVoltage(motor, 0, 0, 0);

  // align absolute zero if it has been found
  if (!Sensor_needsAbsoluteZeroSearch(motor->foc_motor.sensor)) {
    // align the sensor with the absolute zero
    float zero_offset = Sensor_initAbsoluteZero(motor->foc_motor.sensor);
    // remember zero electric angle
    motor->foc_motor.zero_electric_angle = _normalizeAngle(
        _electricalAngle(zero_offset, motor->foc_motor.pole_pairs));
  }
  // return bool if zero found
  return !Sensor_needsAbsoluteZeroSearch(motor->foc_motor.sensor) ? 1 : -1;
}

// Iterative function looping FOC algorithm, setting Uq on the Motor
// The faster it can be run the better
void BLDCMotor_loopFOC(BLDCMotor_t *motor) {
  if (!motor)
    return;

  // shaft angle
  motor->foc_motor.shaft_angle = FOCMotor_shaftAngle(&motor->foc_motor);
  // set the phase voltage - FOC heart function :)
  BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q,
                            motor->foc_motor.voltage_d,
                            _electricalAngle(motor->foc_motor.shaft_angle,
                                             motor->foc_motor.pole_pairs));
}

// Iterative function running outer loop of the FOC algorithm
// Behavior of this function is determined by the motor.controller variable
// It runs either angle, velocity or voltage loop
// - needs to be called iteratively it is asynchronous function
// - if target is not set it uses motor.target value
void BLDCMotor_move(BLDCMotor_t *motor, float new_target) {
  if (!motor)
    return;

  // set internal target variable
  if (new_target != NOT_SET)
    motor->foc_motor.target = new_target;
  // get angular velocity
  motor->foc_motor.shaft_velocity = FOCMotor_shaftVelocity(&motor->foc_motor);
  // choose control loop
  switch (motor->foc_motor.controller) {
  case ControlType_voltage:
    motor->foc_motor.voltage_q = motor->foc_motor.target;
    break;
  case ControlType_angle:
    // angle set point
    // include angle loop
    motor->foc_motor.shaft_angle_sp = motor->foc_motor.target;
    motor->foc_motor.shaft_velocity_sp = PIDController_calculate(
        &motor->foc_motor.P_angle,
        motor->foc_motor.shaft_angle_sp - motor->foc_motor.shaft_angle);
    motor->foc_motor.voltage_q = PIDController_calculate(
        &motor->foc_motor.PID_velocity,
        motor->foc_motor.shaft_velocity_sp - motor->foc_motor.shaft_velocity);
    break;
  case ControlType_velocity:
    // velocity set point
    // include velocity loop
    motor->foc_motor.shaft_velocity_sp = motor->foc_motor.target;
    motor->foc_motor.voltage_q = PIDController_calculate(
        &motor->foc_motor.PID_velocity,
        motor->foc_motor.shaft_velocity_sp - motor->foc_motor.shaft_velocity);
    break;
  case ControlType_velocity_openloop:
    // velocity control in open loop
    // loopFOC should not be called
    motor->foc_motor.shaft_velocity_sp = motor->foc_motor.target;
    BLDCMotor_velocityOpenloop(motor, motor->foc_motor.shaft_velocity_sp);
    break;
  case ControlType_angle_openloop:
    // angle control in open loop
    // loopFOC should not be called
    motor->foc_motor.shaft_angle_sp = motor->foc_motor.target;
    BLDCMotor_angleOpenloop(motor, motor->foc_motor.shaft_angle_sp);
    break;
  }
}

// Method using FOC to set Uq and Ud to the motor at the optimal angle
// Function implementing Space Vector PWM and Sine PWM algorithms
//
// Function using sine approximation
// regular sin + cos ~300us    (no memory usage)
// approx  _sin + _cos ~110us  (400Byte ~ 20% of memory)
void BLDCMotor_setPhaseVoltage(BLDCMotor_t *motor, float Uq, float Ud,
                               float angle_el) {
  if (!motor || !motor->driver)
    return;

  const int centered = 1;
  int sector;
  float _ca, _sa;

  switch (motor->foc_motor.foc_modulation) {
  case FOCModulationType_Trapezoid_120:
    // see https://www.youtube.com/watch?v=InzXA7mWBWE Slide 5
    {
      static int trap_120_map[6][3] = {
          {0, 1, -1}, {-1, 1, 0}, {-1, 0, 1}, {0, -1, 1},
          {1, -1, 0}, {1, 0, -1} // each is 60 degrees with values for 3 phases
                                 // of 1=positive -1=negative 0=high-z
      };
      // static int trap_120_state = 0;
      sector =
          (int)(6 * (_normalizeAngle(angle_el + _PI / 6.0f +
                                     motor->foc_motor.zero_electric_angle) /
                     _2PI)); // adding PI/6 to align with other modes

      motor->Ua = Uq + trap_120_map[sector][0] * Uq;
      motor->Ub = Uq + trap_120_map[sector][1] * Uq;
      motor->Uc = Uq + trap_120_map[sector][2] * Uq;

      if (centered) {
        motor->Ua += (motor->driver->voltage_limit) / 2 - Uq;
        motor->Ub += (motor->driver->voltage_limit) / 2 - Uq;
        motor->Uc += (motor->driver->voltage_limit) / 2 - Uq;
      }
    }
    break;

  case FOCModulationType_Trapezoid_150:
    // see https://www.youtube.com/watch?v=InzXA7mWBWE Slide 8
    {
      static int trap_150_map[12][3] = {
          {0, 1, -1},  {-1, 1, -1}, {-1, 1, 0}, {-1, 1, 1}, {-1, 0, 1},
          {-1, -1, 1}, {0, -1, 1},  {1, -1, 1}, {1, -1, 0}, {1, -1, -1},
          {1, 0, -1},  {1, 1, -1} // each is 30 degrees with values for 3 phases
                                  // of 1=positive -1=negative 0=high-z
      };
      // static int trap_150_state = 0;
      sector =
          (int)(12 * (_normalizeAngle(angle_el + _PI / 6.0f +
                                      motor->foc_motor.zero_electric_angle) /
                      _2PI)); // adding PI/6 to align with other modes

      motor->Ua = Uq + trap_150_map[sector][0] * Uq;
      motor->Ub = Uq + trap_150_map[sector][1] * Uq;
      motor->Uc = Uq + trap_150_map[sector][2] * Uq;

      // center
      if (centered) {
        motor->Ua += (motor->driver->voltage_limit) / 2 - Uq;
        motor->Ub += (motor->driver->voltage_limit) / 2 - Uq;
        motor->Uc += (motor->driver->voltage_limit) / 2 - Uq;
      }
    }
    break;

  case FOCModulationType_SinePWM:
    // Sinusoidal PWM modulation
    // Inverse Park + Clarke transformation

    // angle normalization in between 0 and 2pi
    // only necessary if using _sin and _cos - approximation functions
    angle_el = _normalizeAngle(angle_el + motor->foc_motor.zero_electric_angle);
    _ca = _cos(angle_el);
    _sa = _sin(angle_el);
    // Inverse park transform
    motor->Ualpha = _ca * Ud - _sa * Uq; // -sin(angle) * Uq;
    motor->Ubeta = _sa * Ud + _ca * Uq;  //  cos(angle) * Uq;

    // Clarke transform
    motor->Ua = motor->Ualpha + motor->driver->voltage_limit / 2;
    motor->Ub = -0.5f * motor->Ualpha + _SQRT3_2 * motor->Ubeta +
                motor->driver->voltage_limit / 2;
    motor->Uc = -0.5f * motor->Ualpha - _SQRT3_2 * motor->Ubeta +
                motor->driver->voltage_limit / 2;

    if (!centered) {
      float Umin = fminf(motor->Ua, fminf(motor->Ub, motor->Uc));
      motor->Ua -= Umin;
      motor->Ub -= Umin;
      motor->Uc -= Umin;
    }

    break;

  case FOCModulationType_SpaceVectorPWM:
    // Nice video explaining the SpaceVectorModulation (SVPWM) algorithm
    // https://www.youtube.com/watch?v=QMSWUMEAejg

    // if negative voltages change inverse the phase
    // angle + 180degrees
    if (Uq < 0)
      angle_el += _PI;
    Uq = fabsf(Uq);

    // angle normalisation in between 0 and 2pi
    // only necessary if using _sin and _cos - approximation functions
    angle_el = _normalizeAngle(angle_el + motor->foc_motor.zero_electric_angle +
                               _PI_2);

    // find the sector we are in currently
    sector = (int)floorf(angle_el / _PI_3) + 1;
    // calculate the duty cycles
    float T1 = _SQRT3 * _sin(sector * _PI_3 - angle_el) * Uq /
               motor->driver->voltage_limit;
    float T2 = _SQRT3 * _sin(angle_el - (sector - 1.0f) * _PI_3) * Uq /
               motor->driver->voltage_limit;
    // two versions possible
    float T0 = 0; // pulled to 0 - better for low power supply voltage
    if (centered) {
      T0 = 1 - T1 - T2; // centered around driver->voltage_limit/2
    }

    // calculate the duty cycles(times)
    float Ta, Tb, Tc;
    switch (sector) {
    case 1:
      Ta = T1 + T2 + T0 / 2;
      Tb = T2 + T0 / 2;
      Tc = T0 / 2;
      break;
    case 2:
      Ta = T1 + T0 / 2;
      Tb = T1 + T2 + T0 / 2;
      Tc = T0 / 2;
      break;
    case 3:
      Ta = T0 / 2;
      Tb = T1 + T2 + T0 / 2;
      Tc = T2 + T0 / 2;
      break;
    case 4:
      Ta = T0 / 2;
      Tb = T1 + T0 / 2;
      Tc = T1 + T2 + T0 / 2;
      break;
    case 5:
      Ta = T2 + T0 / 2;
      Tb = T0 / 2;
      Tc = T1 + T2 + T0 / 2;
      break;
    case 6:
      Ta = T1 + T2 + T0 / 2;
      Tb = T0 / 2;
      Tc = T1 + T0 / 2;
      break;
    default:
      // possible error state
      Ta = 0;
      Tb = 0;
      Tc = 0;
    }

    // calculate the phase voltages and center
    motor->Ua = Ta * motor->driver->voltage_limit;
    motor->Ub = Tb * motor->driver->voltage_limit;
    motor->Uc = Tc * motor->driver->voltage_limit;
    break;
  }

  // set the voltages in driver
  BLDCDriver3PWM_setPwm(motor->driver, motor->Ua, motor->Ub, motor->Uc);
}

// Function (iterative) generating open loop movement for target velocity
// - target_velocity - rad/s
// it uses voltage_limit variable
void BLDCMotor_velocityOpenloop(BLDCMotor_t *motor, float target_velocity) {
  if (!motor)
    return;

  // get current timestamp
  unsigned long now_us = _micros();
  // calculate the sample time from last call
  float Ts = (now_us - motor->open_loop_timestamp) * 1e-6f;

  // calculate the necessary angle to achieve target velocity
  motor->foc_motor.shaft_angle =
      _normalizeAngle(motor->foc_motor.shaft_angle + target_velocity * Ts);

  // set the maximal allowed voltage (voltage_limit) with the necessary angle
  BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_limit, 0,
                            _electricalAngle(motor->foc_motor.shaft_angle,
                                             motor->foc_motor.pole_pairs));

  // save timestamp for next call
  motor->open_loop_timestamp = now_us;
}

// Function (iterative) generating open loop movement towards the target angle
// - target_angle - rad
// it uses voltage_limit and velocity_limit variables
void BLDCMotor_angleOpenloop(BLDCMotor_t *motor, float target_angle) {
  if (!motor)
    return;

  // get current timestamp
  unsigned long now_us = _micros();
  // calculate the sample time from last call
  float Ts = (now_us - motor->open_loop_timestamp) * 1e-6f;

  // calculate the necessary angle to move from current position towards target
  // angle with maximal velocity (velocity_limit)
  if (fabsf(target_angle - motor->foc_motor.shaft_angle) >
      fabsf(motor->foc_motor.velocity_limit * Ts))
    motor->foc_motor.shaft_angle +=
        _sign(target_angle - motor->foc_motor.shaft_angle) *
        fabsf(motor->foc_motor.velocity_limit) * Ts;
  else
    motor->foc_motor.shaft_angle = target_angle;

  // set the maximal allowed voltage (voltage_limit) with the necessary angle
  BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_limit, 0,
                            _electricalAngle(motor->foc_motor.shaft_angle,
                                             motor->foc_motor.pole_pairs));

  // save timestamp for next call
  motor->open_loop_timestamp = now_us;
}

#endif /* SRC_BLDCMOTOR_C_ */
