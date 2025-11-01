#ifndef __BLDCMOTOR_H
#define __BLDCMOTOR_H

#include "stm32_hal.h"
#include "BLDCDriver.h"
#include "foc_motor.h"
#include "pid.h"

typedef struct CurrentSense_t CurrentSense_t;
typedef struct
{
    FOCMotor_t foc_motor; //!< Base FOC motor structure (must be first member)
    FOCMotor_t *FOCMotor; //!< Base FOC motor structure (must be first member)

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
    GPIO_TypeDef *enable_Port;
    uint16_t enable_Pin;
    PIDController *pid;
    float velocity;
    float angle;
    float e_angle;
    float target;
    // float target_iq;
    // float target_id;
    bool isEnable;
    AS5600_t *as5600;
    CurrentSense_t *currentSense;

} BLDCMotor_t;
float BLDCMotor_getAngle(BLDCMotor_t *motor);
float BLDCMotor_getElectricalAngle(BLDCMotor_t *motor);
float BLDCMotor_getVelocity(BLDCMotor_t *motor);
float BLDCMotor_getVelocityRPM(BLDCMotor_t *motor);
void BLDCMotor_init(BLDCMotor_t *motor, int pp);
void BLDCMotor_linkDriver(BLDCMotor_t *motor, BLDCDriver_t *_driver);
void BLDCMotor_linkEncoder(BLDCMotor_t *motor, Encoder_t *encoder);
void BLDCMotor_setPhaseVoltage(BLDCMotor_t *motor, float Uq, float Ud, float angle_el);
void BLDCMotor_move(BLDCMotor_t *motor, float new_target);
int BLDCMotor_enable(BLDCMotor_t *motor);
int BLDCMotor_disable(BLDCMotor_t *motor);
void BLDCMotor_loopFOC(BLDCMotor_t *motor);
void BLDCMotor_run(BLDCMotor_t *motor);
int BLDCMotor_velocityClosedLoop(BLDCMotor_t *motor, float target);
void BLDCMotor_linkAS5600(BLDCMotor_t *motor, AS5600_t *as5600);

void BLDCMotor_velocityOpenloop(BLDCMotor_t *motor, float target_velocity);

int BLDCMotor_currentClosedLoop(BLDCMotor_t *motor, float target);

float BLDCMotor_getCurrentDQ(BLDCMotor_t *motor);
int BLDCMotor_velocityClosedLoop_without_i(BLDCMotor_t *motor, float target);
#endif // !__BLDCMOTOR_H
