/*
 * BLDCMotor.h
 * Converted from C++ to C
 *  Created on: 17 Mar 2021
 *      Author: natal
 */

#ifndef INC_BLDCMOTOR_H_
#define INC_BLDCMOTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "../common/base_classes/BLDCDriver.h"
#include "../common/base_classes/FOCMotor.h"
#include "../common/base_classes/Sensor.h"
#include "../common/defaults.h"
#include "../common/foc_utils.h"
#include "../common/time_utils.h"

/**
 * BLDC motor structure
 */
typedef struct {
  FOCMotor_t foc_motor; //!< Base FOC motor structure (must be first member)

  /**
   * BLDCDriver link:
   * - 3PWM
   * - 6PWM
   */
  BLDCDriver_t *driver;

  float Ua, Ub, Uc;    //!< Current phase voltages Ua,Ub and Uc set to motor
  float Ualpha, Ubeta; //!< Phase voltages U alpha and U beta used for inverse
                       //!< Park and Clarke transform

  // open loop variables
  long open_loop_timestamp;
} BLDCMotor_t;

/**
 * Initialize BLDC Motor
 * @param motor - Pointer to BLDCMotor structure
 * @param pp - pole pairs number
 */
static void BLDCMotor_init(BLDCMotor_t *motor, int pp);

/**
 * Function linking a motor and a foc driver
 * @param motor - Pointer to BLDCMotor structure
 * @param driver - BLDCDriver implementing all hardware specific functions
 * necessary for PWM setting
 */
void BLDCMotor_linkDriver(BLDCMotor_t *motor, BLDCDriver_t *driver);

/**
 * Motor hardware init function
 * @param motor - Pointer to BLDCMotor structure
 */
void BLDCMotor_init_hardware(BLDCMotor_t *motor);

/**
 * Motor disable function
 * @param motor - Pointer to BLDCMotor structure
 */
void BLDCMotor_disable(BLDCMotor_t *motor);

/**
 * Motor enable function
 * @param motor - Pointer to BLDCMotor structure
 */
void BLDCMotor_enable(BLDCMotor_t *motor);

/**
 * Function initializing FOC algorithm
 * and aligning sensor's and motors' zero position
 * @param motor - Pointer to BLDCMotor structure
 * @param zero_electric_offset - value of the sensors absolute position
 * electrical offset
 * @param sensor_direction - sensor natural direction - default is Direction_CW
 * @return 0 on success, error code otherwise
 */
int BLDCMotor_initFOC(BLDCMotor_t *motor, float zero_electric_offset,
                      Direction_t sensor_direction);

/**
 * Function running FOC algorithm in real-time
 * it calculates the gets motor angle and sets the appropriate voltages
 * to the phase pwm signals
 * - the faster you can run it the better Arduino UNO ~1ms, Bluepill ~ 100us
 * @param motor - Pointer to BLDCMotor structure
 */
void BLDCMotor_loopFOC(BLDCMotor_t *motor);

/**
 * Function executing the control loops set by the controller parameter of the
 * BLDCMotor.
 * @param motor - Pointer to BLDCMotor structure
 * @param target - Either voltage, angle or velocity based on the
 * motor.controller If it is NOT_SET the motor will use the target set in its
 * variable motor.target This function doesn't need to be run upon each loop
 * execution - depends of the use case
 */
void BLDCMotor_move(BLDCMotor_t *motor, float new_target);

/**
 * Method using FOC to set Uq to the motor at the optimal angle
 * Heart of the FOC algorithm
 * @param motor - Pointer to BLDCMotor structure
 * @param Uq - Current voltage in q axis to set to the motor
 * @param Ud - Current voltage in d axis to set to the motor
 * @param angle_el - current electrical angle of the motor
 */
void BLDCMotor_setPhaseVoltage(BLDCMotor_t *motor, float Uq, float Ud,
                               float angle_el);

/**
 * Sensor alignment to electrical 0 angle of the motor
 * @param motor - Pointer to BLDCMotor structure
 * @return 0 on success, error code otherwise
 */
int BLDCMotor_alignSensor(BLDCMotor_t *motor);

/**
 * Motor and sensor alignment to the sensors absolute 0 angle
 * @param motor - Pointer to BLDCMotor structure
 * @return 0 on success, error code otherwise
 */
int BLDCMotor_absoluteZeroAlign(BLDCMotor_t *motor);

/**
 * Function (iterative) generating open loop movement for target velocity
 * it uses voltage_limit variable
 * @param motor - Pointer to BLDCMotor structure
 * @param target_velocity - target velocity in rad/s
 */
void BLDCMotor_velocityOpenloop(BLDCMotor_t *motor, float target_velocity);

/**
 * Function (iterative) generating open loop movement towards the target angle
 * it uses voltage_limit and velocity_limit variables
 * @param motor - Pointer to BLDCMotor structure
 * @param target_angle - target angle in rad
 */
void BLDCMotor_angleOpenloop(BLDCMotor_t *motor, float target_angle);

#ifdef __cplusplus
}
#endif

#endif /* INC_BLDCMOTOR_H_ */
