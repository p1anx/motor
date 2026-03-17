//
// Created by xwj on 1/14/26.
//

#ifndef ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_MOTOR_H
#define ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_MOTOR_H

#include "FOCDriver.h"
#include "LowpassFilter.h"
#include "currentSense.h"
#include "key.h"
#include "sensor.h"
#include "pid.h"
#include "DRV830X.h"

typedef enum
{
  ControllerType_voltage,  //!< Torque control using voltage
  ControllerType_current,  //!< Torque control using voltage
  ControllerType_velocity, //!< Velocity motion control
  ControllerType_angle,    //!< Position/angle motion control
  ControllerType_velocityOpenLoop,
  ControllerType_currentVelocityOpenLoop,
  ControllerType_angle_openloop,
  ControllerType_velocityClosedLoop, //!< Velocity motion control
  ControllerType_velocityAngleClosedLoop, //!< Velocity motion control
  ControllerType_velocityClosedLoopInline, //!< Velocity motion control
  ControllerType_angleClosedLoop,    //!< angle motion control
  ControllerType_currentClosedLoop,  //!< angle motion control
  ControllerType_currentVelocityClosedLoop,  //!< angle motion control
  ControllerType_currentAngleClosedLoop,  //!< angle motion control
  ControllerType_currentVelocityAngleClosedLoop,  //!< angle motion control
  ControllerType_currentClosedLoopBandwidth,  //!< angle motion control
  ControllerType_currentClosedLoopBandwidthInline,  //!< angle motion control
  ControllerType_currentVelocityClosedLoopBandwidth,  //!< angle motion control
  ControllerType_currentAngleClosedLoopBandwidth,  //!< angle motion control
  ControllerType_currentVelocityRPMClosedLoopBandwidth,  //!< angle motion control
  ControllerType_currentVelocityAngleClosedLoopBandwidth,  //!< angle motion control
  ControllerType_currentClosedLoopBandwidth_debug,  //!< angle motion control
} ControllerType_t;



typedef enum
{
  Direction_CW = -1,     // clockwise
  Direction_CCW = 1,     // counter clockwise
  // Direction_UNKNOWN = 0 // not yet known or invalid state
} Direction_t;

typedef struct {
  LowPassFilter_t lowPassFilter_ia;
  LowPassFilter_t lowPassFilter_ib;
  LowPassFilter_t lowPassFilter_ic;
  LowPassFilter_t lowPassFilter_id;
  LowPassFilter_t lowPassFilter_iq;
  LowPassFilter_t lowPassFilter_velocity;
  LowPassFilter_t lowPassFilter_degree;

}Filter_t;

typedef struct Motor_t Motor_t;
struct Motor_t {
  FOCDriver_t FOCDriver;
  FOCDriver_t* pFOCDriver;
  CurrentSense_t* pCurrentSense;
  Sensor_t Sensor;
  Direction_t direction;

  ControllerType_t controllerType;
  KEY_t* pKEY;
  Filter_t filter;
  int pole_pairs;
  float angle, e_angle, ref_angle;
  float velocity, ref_velocity;
  float ref;
  int isEnabled;
  float voltage_alignSensor;
  float zero_electrical_angle;
  float *Ia, *Ib, *Ic, *Id, *Iq;
  float Ud, Uq;
  LowPassFilter_t lpf_ia, lpf_ib, lpf_ic, lpf_id, lpf_iq;
  LowPassFilter_t lpf_velocity, lpf_degree;
  float dt;
  PIDController_t PID_Velocity, PID_Angle, PID_Current, PID_Id, PID_Iq, PID_iVelocity, PID_iVelDegree; // PID_iVelocity for  velocity pid using currentloop
  int freq_currentLoop, freq_velocityLoop, freq_positionLoop, vel_period, pos_period;
  float ref_id, ref_iq;
  float degree;
  bool isODrive;
  DRV8301_CONFIG_t drv830xConfig;



};
void Motor_Driver_Init(void);
int Motor_CurrentSenseInit(Motor_t *motor);
void Motor_linkCurrentSense(Motor_t *motor, CurrentSense_t *pCurrentSense);
int Motor_SensorInit(Motor_t *motor);

void Motor_move(Motor_t *motor, float ref);
void Motor_DriverInit(Motor_t* motor);
void Motor_setPhaseVoltage(Motor_t* motor, float Uq, float Ud, float electrical_angle);
void Motor_velocityOpenLoop(Motor_t *motor, float target_velocity);
float Motor_getVelocity(Motor_t* motor);
float Motor_getAngle(Motor_t* motor);
void Motor_InitConfig(Motor_t* motor);
void Motor_FilterInit(Motor_t* motor);
int Motor_alignSensor(Motor_t *motor);

void Motor_enable(Motor_t* motor);
void Motor_disable(Motor_t* motor);
void Motor_GetCurrentIdIq(Motor_t *motor);
void Motor_velocityClosedLoop(Motor_t *motor, float ref_velocity);
float Motor_GetElectricalAngleCalibrated(Motor_t *motor);
void Motor_PIDInit(Motor_t* motor);
// void Motor_currentClosedLoop(Motor_t *motor, float ref);
static void Motor_currentClosedLoop(Motor_t *motor, float ref);
static void Motor_currentVelocityClosedLoop(Motor_t *motor, float ref);
#endif // ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_MOTOR_H
