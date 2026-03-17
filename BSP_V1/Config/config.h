//
// Created by xwj on 1/13/26.
//

#ifndef ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_CONFIG_H
#define ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_CONFIG_H
#include "stm32_hal.h"
#include "FOCDriver.h"
#include "sensor.h"
#include "currentSense.h"
#include "Motor.h"

#include "configSTM32.h"


#define CONFIG_PWM_FREQUENCY 10e3
// #define CONFIG_CONTROLLER_TYPE ControllerType_velocityOpenLoop
// #define CONFIG_CONTROLLER_TYPE ControllerType_velocityClosedLoop
// #define CONFIG_CONTROLLER_TYPE ControllerType_currentClosedLoop
// #define CONFIG_CONTROLLER_TYPE ControllerType_currentVelocityClosedLoop
#define CONFIG_CONTROLLER_TYPE ControllerType_currentVelocityAngleClosedLoop

#define CONFIG_UART_PRINTF huart2

typedef struct {
  // PIDController_t PID_Current, PID_Velocity, PID_Angle;
  PIDController_t PID_Velocity, PID_Angle, PID_Current,PID_Id, PID_Iq, PID_iVelocity, PID_iVelDegree; // PID_iVelocity for  velocity pid using currentloop
}PID_t ;

typedef struct Config_t Config_t;
struct Config_t {
  ConfigSTM32_t *pSTM32;
  FOCDriver_t *pFOCDriver;
  Sensor_t *pSensor;
  CurrentSense_t *pCurrentSense;

};

void ConfigMotor(Motor_t* motor);
void ConfigMotor_Init(Motor_t* motor, CurrentSense_t* pCurrentSense, const FOCDriver_t* pFOCDriver, ConfigSTM32_t* pConfigSTM32);
// void ConfigMotor_Motor(Motor_t* motor, const Motor_t* motor_u);
void ConfigMotor_Motor(Motor_t* motor, const Motor_t* motor_u, ConfigSTM32_t* pConfigSTM32);
// void ConfigMotor_KEY(Motor_t* motor, KEY_t *key);
// void ConfigMotor_KEY(Motor_t* motor,ConfigSTM32_t* pConfigSTM32, KEY_t *key);
void ConfigMotor_KEY(Motor_t* motor, KEY_t *key,ConfigSTM32_t* pConfigSTM32);
void ConfigMotor_Filter(Motor_t* motor, const Filter_t* filter, ConfigSTM32_t* pConfigSTM32);
// void ConfigMotor_PID(Motor_t* motor, const PID_t* PID);
void ConfigMotor_PID(Motor_t* motor, const PID_t* PID, ConfigSTM32_t* pConfigSTM32);
// void ConfigMotor_CurrentSense(Motor_t* motor, CurrentSense_t* pCurrentSense, DRV8301_CONFIG_t* pDRV803xConfig, ConfigSTM32_t* pConfigSTM32);
// void ConfigMotor_CurrentSense(Motor_t* motor,CurrentSense_t* mainCurrentSense, CurrentSense_t* pCurrentSense, DRV8301_CONFIG_t* pDRV803xConfig, ConfigSTM32_t* pConfigSTM32);
// void ConfigMotor_CurrentSense(Motor_t* motor,CurrentSense_t* pCurrentSense, CurrentSense_t* pCurrentSenseVar, DRV8301_CONFIG_t* pDRV803xConfig, ConfigSTM32_t* pConfigSTM32);
void ConfigMotor_CurrentSense(Motor_t* motor,CurrentSense_t* pCurrentSense, CurrentSense_t* pCurrentSenseVar, ConfigSTM32_t* pConfigSTM32);
#endif // ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_CONFIG_H
