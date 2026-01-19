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
#include "configMotor.h"
#include "configDriver.h"


#define CONFIG_UART_PRINTF huart2

typedef struct {
  // PIDController_t PID_Current, PID_Velocity, PID_Angle;
  PIDController_t PID_Velocity, PID_Angle, PID_Current,PID_Id, PID_Iq, PID_iVelocity, PID_iVelDegree; // PID_iVelocity for  velocity pid using currentloop
}PID_t ;

typedef struct Config_t Config_t;
struct Config_t {
  ConfigSTM32_t *pSTM32;
  ConfigDriver_t *pDriver;
  ConfigMotor_t *pMotor;
  FOCDriver_t *pFOCDriver;
  Sensor_t *pSensor;
  CurrentSense_t *pCurrentSense;

};

void ConfigMotor(Motor_t* motor);
// void ConfigMotor_Motor(Motor_t* motor, const Motor_t* motor_u);
void ConfigMotor_Motor(Motor_t* motor, const Motor_t* motor_u, ConfigSTM32_t* pConfigSTM32);
// void ConfigMotor_KEY(Motor_t* motor, KEY_t *key);
// void ConfigMotor_KEY(Motor_t* motor,ConfigSTM32_t* pConfigSTM32, KEY_t *key);
void ConfigMotor_KEY(Motor_t* motor, KEY_t *key,ConfigSTM32_t* pConfigSTM32);
void ConfigMotor_Filter(Motor_t* motor, const Filter_t* filter, ConfigSTM32_t* pConfigSTM32);
// void ConfigMotor_PID(Motor_t* motor, const PID_t* PID);
void ConfigMotor_PID(Motor_t* motor, const PID_t* PID, ConfigSTM32_t* pConfigSTM32);
#endif // ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_CONFIG_H
