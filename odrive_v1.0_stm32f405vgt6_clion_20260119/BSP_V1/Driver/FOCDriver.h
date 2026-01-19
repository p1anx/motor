//
// Created by xwj on 1/14/26.
//

#ifndef ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_FOCDRIVER_H
#define ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_FOCDRIVER_H
#include "pwm.h"


typedef enum
{
  FOCModulationType_SinePWM,        //!< Sinusoidal PWM modulation
  FOCModulationType_SpaceVectorPWM, //!< Space vector modulation method
  FOCModulationType_Trapezoid_120,
  FOCModulationType_Trapezoid_150
} FOCModulationType_t;

typedef struct FOCDriver_t FOCDriver_t;
struct FOCDriver_t {
//   PWM_Type pwm_type;
//   float voltage, voltage_limit;
//
//
// };

  PWM_t pwm;
  PWM_t* pPWM;
  int pwm_frequency;                //!< pwm frequency value in hertz
  int pwm_resolution;
  float voltage_power_supply; //!< power supply voltage
  float voltage_limit;        //!< limiting voltage set to the motor
  float Ua, Ub, Uc;
  float Ualpha, Ubeta;
  float zero_electric_angle;
  float e_angle;
  // PWM_DriverType PWM_DriverType;
  //
  //
  FOCModulationType_t FOCModulationType;

};

void FOCDriver_PWM_Init(void);
void FOCDriver_PWM_Example(void);
void FOCDriver_setPhaseVoltage(FOCDriver_t *driver, float Uq, float Ud, float electrical_angle);
void FOCDriver_setPwm(FOCDriver_t *driver, float Ua, float Ub, float Uc);
#endif // ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_FOCDRIVER_H
