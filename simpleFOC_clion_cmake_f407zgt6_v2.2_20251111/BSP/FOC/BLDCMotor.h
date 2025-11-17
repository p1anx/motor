#ifndef __BLDCMOTOR_H
#define __BLDCMOTOR_H

#include "stm32_hal.h"
#include "BLDCDriver.h"
#include "foc_motor.h"
#include "pid.h"
#include "as5600.h"
#include "currentSense.h"

#define MOTOR_ENABLE_PORT GPIOA
#define MOTOR_ENABLE_PIN GPIO_PIN_9

#define MOTOR_DIRECTION AS5600_CCW_INCREASE

#define  CONFIG_VoltageSupply   12
#define  CONFIG_VoltageLimit    12
#define  CONFIG_PWM_HZ          20e3
#define  CONFIG_PWM_RESOLUTION  4096

typedef enum
{
    Direction_CW = 1,     // clockwise
    Direction_CCW = -1,   // counter clockwise
    // Direction_UNKNOWN = 0 // not yet known or invalid state
} Direction_t;

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
    long open_loop_timestamp;
    GPIO_TypeDef *enable_Port;
    uint16_t enable_Pin;
    PIDController *pid;
    PIDController PID;
    PIDController PID_id;
    PIDController PID_iq;
    PIDController PID_velocity;
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
    LowPassFilter_t lpf_id, lpf_iq, lpf_velocity, lpf_angle, lpf_degree;

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
#endif // !__BLDCMOTOR_H
