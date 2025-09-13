

#ifndef SRC_BLDCMOTOR_H_
#define SRC_BLDCMOTOR_H_

#include "common/base_classes/BLDCDriver.h"
#include "common/base_classes/FOCMotor.h"
#include "common/base_classes/Sensor.h"
#include "common/defaults.h"
#include "common/foc_utils.h"
#include "common/time_utils.h"

/**
 BLDCMotor class constructor
 @param pp pole pairs number
 @param R  motor phase resistance
 */
BLDCMotor(int pp, float R);

/**
 * Function linking a motor and a foc driver
 *
 * @param driver BLDCDriver class implementing all the hardware specific
 * functions necessary PWM setting
 */
void linkDriver(BLDCDriver *driver);

/**
 * BLDCDriver link:
 * - 3PWM
 * - 6PWM
 */
BLDCDriver *driver;

int initFOC(float zero_electric_offset, Direction sensor_direction);
void loopFOC();
void move(float target);

float Ua, Ub, Uc;    //!< Current phase voltages Ua,Ub and Uc set to motor
float Ualpha, Ubeta; //!< Phase voltages U alpha and U beta used for inverse
                     //!< Park and Clarke transform
void setPhaseVoltage(float Uq, float Ud, float angle_el);
int alignSensor(void);
int alignCurrentSense(void);
int absoluteZeroSearch(void);

float velocityOpenloop(float target_velocity);
float angleOpenloop(float target_angle);
long open_loop_timestamp;

int antiCoggingCalibration(void);

#endif /* SRC_BLDCMOTOR_HPP_ */
