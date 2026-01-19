#ifndef __BLDCMOTOR_H
#define __BLDCMOTOR_H

#include "stm32_hal.h"
#include "BLDCDriver.h"
#include "foc_motor.h"
#include "pid.h"
#include "as5600.h"
#include "currentSense.h"

#define MOTOR_ENABLE_PORT GPIOD
#define MOTOR_ENABLE_PIN GPIO_PIN_13

#define MOTOR_DIRECTION AS5600_CCW_INCREASE




typedef enum
{
    Direction_CW = -1,     // clockwise
    Direction_CCW = 1,     // counter clockwise
    // Direction_UNKNOWN = 0 // not yet known or invalid state
} Direction_t;
typedef enum {
  Monitor_NULL,
  Monitor_Velocity,
  Monitor_Angle,
  Monitor_IaIbIdIq,
  Monitor_IaIbIdIqUabc,
  Monitor_IdIq,
  Monitor_TargetIdIq,
  Monitor_UaUbUc,
  Monitor_IalphaIbetaUabc,


}MotorMonitor_Type;
typedef enum {
  InCurrentLoop,
  InVelocityLoop,
  InPositionLoop,
}MotorLoopType;

typedef struct CurrentSense_t CurrentSense_t;

typedef struct
{
    FOCMotor_t foc_motor; //!< Base FOC motor structure (must be first member)
    FOCMotor_t *FOCMotor; //!< Base FOC motor structure (must be first member)
    Encoder_t encoder;

    /**
     * BLDCDriver link:
     * - 3PWM
     * - 6PWM
     */
    BLDCDriver_t *driver;
    BLDCDriver_t Driver;

    float Ua, Ub, Uc;    //!< Current phase voltages Ua,Ub and Uc set to motor
    float Ualpha, Ubeta; //!< Phase voltages U alpha and U beta used for inverse
                         //!< Park and Clarke transform

    // open loop variables
    uint32_t open_loop_timestamp;
    GPIO_TypeDef *enable_Port;
    uint16_t enable_Pin;
    PIDController *pid;
    PIDController PID;
    PIDController PID_id;
    PIDController PID_iq;
    PIDController PID_velocity;
    PIDController PID_velocityOnly;
    PIDController PID_angle;
    PIDController PID_degree;
    float velocity;
    float angle;
    float degree;
    float e_angle;
    float target;
    // float target_iq;
    // float target_id;
    bool isEnable;
    AS5600_t *as5600;
    CurrentSense_t *currentSense;
    CurrentSense_t CurrentSense;
    int pid_dt;
    float lastAngle;
    Direction_t direction;
    LowPassFilter_t lpf_id, lpf_iq, lpf_velocity, lpf_angle, lpf_degree, lpf_ia, lpf_ib, lpf_eAngle;
   LowPassFilter_t lpf_ia0, lpf_ib0, lpf_id0, lpf_iq0;
   LowPassFilter_t filter_velocity, filter_angle;
    int deadzone_enable;
    float deadzone_uq;
  float target_iq;
  float Iq_ref_ff; // forward
  float target_id;
  float target_velocity;
  uint32_t delta_us;
  float velocity_limit, iq_limit, id_limit;
  float lastVelocity;
  ControlType_t controllerType;
  MotorMonitor_Type monitorType;
  MotorLoopType motorLoopType;


} BLDCMotor_t;
// typedef struct {
//
//
// }MotorMonitor_t;
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
int BLDCMotor_angleClosedLoop(BLDCMotor_t *motor, float target);

float BLDCMotor_getCurrentDQ(BLDCMotor_t *motor);
int BLDCMotor_velocityClosedLoop_without_i(BLDCMotor_t *motor, float target);
void BLDCMotor_velocityOpenloop_with_timer(BLDCMotor_t *motor, float target_velocity);
int BLDCMotor_alignSensor(BLDCMotor_t *motor);
int BLDCMotor_alignSensor_AutoCorrect(BLDCMotor_t *motor);
float BLDCMotor_getCurrentDQ_noFilter(BLDCMotor_t *motor);
// void BLDCMotor_initAll(BLDCMotor_t *motor, int pp);
// void BLDCMotor_initAll(BLDCMotor_t *motor, int pp, EncoderType_enum encoderType);
void BLDCMotor_initAll(BLDCMotor_t *motor, int pp, EncoderType_enum encoderType, ControlType_t controllerType);
void BLDCMotor_initPID(BLDCMotor_t *motor, int pp, EncoderType_enum encoderType, ControlType_t controllerType, PIDController pid_param);
void BLDCMotor_initPID_CurrentDQ(BLDCMotor_t *motor, int pp, EncoderType_enum encoderType, ControlType_t controllerType, PIDController pid_id, PIDController pid_iq);
void BLDCMotor_initPID_CurrentVelocity(BLDCMotor_t *motor, int pp, EncoderType_enum encoderType, ControlType_t controllerType, PIDController pid_id, PIDController pid_iq, PIDController pid_velocity);
int BLDCMotor_currentVelocityClosedLoop(BLDCMotor_t *motor, float target);
int BLDCMotor_currentVelocityAngleClosedLoop(BLDCMotor_t *motor, float target);
float BLDCMotor_getAngleDegrees(BLDCMotor_t *motor);
void BLDCMotor_initPID_CurrentVelocityAngle(BLDCMotor_t *motor, int pp, EncoderType_enum encoderType, ControlType_t controllerType, PIDController pid_id, PIDController pid_iq, PIDController pid_velocity, PIDController pid_degree);
int BLDCMotor_currentClosedLoop_with_bandwidth(BLDCMotor_t *motor, float target);
int BLDCMotor_positionCascade_with_bandwidth(BLDCMotor_t *motor, float target_degree);
int BLDCMotor_currentClosedLoopBandwith(BLDCMotor_t *motor, float target);
int BLDCMotor_currentVelocityClosedLoopBandwith(BLDCMotor_t *motor, float target);
int BLDCMotor_currentVelocityAngleClosedLoopBandwith(BLDCMotor_t *motor, float target);
int BLDCMotor_currentClosedLoopBandwith_debug(BLDCMotor_t *motor, float target);
int BLDCMotor_currentVelocityRPMClosedLoopBandwith(BLDCMotor_t *motor, float target);
void BLDCMotor_ControlRunByKey(BLDCMotor_t *motor);
void BLDCMotor_SVPWM(BLDCMotor_t *motor, float Uq, float Ud, float electrical_angle);
void BLDCMotor_currentVelocityOpenloop(BLDCMotor_t *motor, float target_velocity);
void BLDCMotor_initPID_CurrentVelocityAngle_Inline(BLDCMotor_t *motor, int pp, EncoderType_enum encoderType, ControlType_t controllerType, PIDController pid_id, PIDController pid_iq, PIDController pid_velocity, PIDController pid_degree);
int BLDCMotor_currentClosedLoopBandwithInline(BLDCMotor_t *motor, float target);
void BLDCMotor_currentVelocityOpenloopInline(BLDCMotor_t *motor, float target_velocity);
void BLDCMotor_KeyControl(BLDCMotor_t *motor);
float BLDCMotor_getCurrentDQ_Polling(BLDCMotor_t *motor);
int BLDCMotor_velocityClosedLoopInline(BLDCMotor_t *motor, float target);
// float BLDCMotor_outputRamp(BLDCMotor_t *motor);
float BLDCMotor_outputVelocityRamp(BLDCMotor_t *motor, float output_ramp);
int BLDCMotor_currentAngleClosedLoopBandwith(BLDCMotor_t *motor, float target);
void BLDCMotor_PIDInit(BLDCMotor_t *motor);
void BLDCMotor_LowpassFilterInit(BLDCMotor_t *motor);
void BLDCMotor_init3508(BLDCMotor_t *motor, EncoderType_enum encoderType, ControlType_t controllerType);
int BLDCMotor_velocityAngleClosedLoop(BLDCMotor_t *motor, float target);
int BLDCMotor_currentAngleClosedLoopBandwith(BLDCMotor_t *motor, float target);
int BLDCMotor_Monitor(BLDCMotor_t *motor, MotorMonitor_Type monitorType);
#endif // !__BLDCMOTOR_H
