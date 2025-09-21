#ifndef __BLDCMOTOR_H
#define __BLDCMOTOR_H

#include "BLDCDriver.h"
#include "foc_motor.h"

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
void BLDCMotor_init(BLDCMotor_t *motor, int pp);
void BLDCMotor_linkDriver(BLDCMotor_t *motor, BLDCDriver_t *_driver);
void BLDCMotor_setPhaseVoltage(BLDCMotor_t *motor, float Uq, float Ud,
                               float angle_el);
void BLDCMotor_move(BLDCMotor_t *motor, float new_target);
#endif // !__BLDCMOTOR_H
