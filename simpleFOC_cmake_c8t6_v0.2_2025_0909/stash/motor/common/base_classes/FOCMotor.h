/*
 * FOCMotor.h
 * Ported from:
 * https://github.com/simplefoc/Arduino-FOC/blob/master/src/common/base_classes/FOCMotor.h
 * Converted from C++ to C
 */

#ifndef FOCMOTOR_H_
#define FOCMOTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "../defaults.h"
#include "../foc_utils.h"
#include "../lowpass_filter.h"
#include "../pid.h"
#include "../time_utils.h"
#include "Sensor.h"

/**
 * Motion control type enumeration
 */
typedef enum {
  ControlType_voltage,  //!< Torque control using voltage
  ControlType_velocity, //!< Velocity motion control
  ControlType_angle,    //!< Position/angle motion control
  ControlType_velocity_openloop,
  ControlType_angle_openloop
} ControlType_t;

/**
 * FOC modulation type enumeration
 */
typedef enum {
  FOCModulationType_SinePWM,        //!< Sinusoidal PWM modulation
  FOCModulationType_SpaceVectorPWM, //!< Space vector modulation method
  FOCModulationType_Trapezoid_120,
  FOCModulationType_Trapezoid_150
} FOCModulationType_t;

/**
 * Forward declaration for motor function pointers
 */
typedef struct FOCMotor_t FOCMotor_t;

/**
 * FOC Motor function pointers structure
 */
typedef struct {
  /** Motor hardware init function */
  void (*init)(FOCMotor_t *motor);
  /** Motor disable function */
  void (*disable)(FOCMotor_t *motor);
  /** Motor enable function */
  void (*enable)(FOCMotor_t *motor);
  /**
   * Function initializing FOC algorithm
   * and aligning sensor's and motors' zero position
   */
  int (*initFOC)(FOCMotor_t *motor, float zero_electric_offset,
                 Direction_t sensor_direction);
  /**
   * Function running FOC algorithm in real-time
   * it calculates the gets motor angle and sets the appropriate voltages
   * to the phase pwm signals
   */
  void (*loopFOC)(FOCMotor_t *motor);
  /**
   * Function executing the control loops set by the controller parameter
   */
  void (*move)(FOCMotor_t *motor, float target);
} FOCMotorFunctions_t;

/**
 * Generic FOC motor structure
 */
struct FOCMotor_t {
  FOCMotorFunctions_t *functions; //!< Function pointer table

  // state variables
  float target;            //!< current target value - depends of the controller
  float shaft_angle;       //!< current motor angle
  float shaft_velocity;    //!< current motor velocity
  float shaft_velocity_sp; //!< current target velocity
  float shaft_angle_sp;    //!< current target angle
  float voltage_q;         //!< current voltage u_q set
  float voltage_d;         //!< current voltage u_d set

  // motor configuration parameters
  float voltage_sensor_align;  //!< sensor and motor align voltage parameter
  float velocity_index_search; //!< target velocity for index search
  int pole_pairs;              //!< Motor pole pairs number

  // limiting variables
  float voltage_limit;  //!< Voltage limiting variable - global limit
  float velocity_limit; //!< Velocity limiting variable - global limit

  float zero_electric_angle; //!< absolute zero electric angle - if available

  // configuration structures
  ControlType_t
      controller; //!< parameter determining the control loop to be used
  FOCModulationType_t
      foc_modulation; //!< parameter determining modulation algorithm
  PIDController
      PID_velocity; //!< parameter determining the velocity PI configuration
  PIDController P_angle; //!< parameter determining the position P configuration
  LowPassFilter LPF_velocity; //!< parameter determining the velocity Low pass
                              //!< filter configuration

  /**
   * Sensor link:
   * - Encoder
   * - MagneticSensor*
   * - HallSensor
   */
  Sensor_t *sensor;
};

/**
 * Initialize FOCMotor structure
 * @param motor - Pointer to FOCMotor structure
 */
static void FOCMotor_init(FOCMotor_t *motor);

/**
 * Function linking a motor and a sensor
 * @param motor - Pointer to FOCMotor structure
 * @param sensor - Sensor wrapper for the FOC algorithm to read motor angle and
 * velocity
 */
void FOCMotor_linkSensor(FOCMotor_t *motor, Sensor_t *sensor);

/**
 * Shaft angle calculation in radians [rad]
 * @param motor - Pointer to FOCMotor structure
 * @return Current shaft angle in radians
 */
float FOCMotor_shaftAngle(FOCMotor_t *motor);

/**
 * Shaft velocity calculation function in radian per second [rad/s]
 * It implements low pass filtering
 * @param motor - Pointer to FOCMotor structure
 * @return Current shaft velocity in rad/s
 */
float FOCMotor_shaftVelocity(FOCMotor_t *motor);

#ifdef __cplusplus
}
#endif

#endif /* FOCMOTOR_H_ */
