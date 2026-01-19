//
// Created by xwj on 1/19/26.
//

#include "configODrive.h"

#include "config.h"
#include "configSTM32.h"
#include "LowpassFilter.h"
#include "DRV830X.h"

#include <stdio.h>

extern ADC_HandleTypeDef hadc1;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim1;

extern KEY_t key_enableMotor;

extern FOCDriver_t FOCDriver_3508;
extern Motor_t Motor_3508;
extern Filter_t Filter;
extern Filter_t Filter_MOTOR3508;
extern CurrentSense_t CurrentSense;

ConfigSTM32_t configSTM32F405VGT6 = {
  .sensor_spi = &hspi3,
  .CS_GPIO_Port = GPIOC,
  .CS_GPIO_Pin = GPIO_PIN_4,
  .sensor_direction = 0,

  .DRV830X_CS_GPIO_Port = GPIOC,
  .DRV830X_CS_GPIO_Pin = GPIO_PIN_13,

  .EN_GPIO_Port = GPIOB,
  .EN_GPIO_Pin = GPIO_PIN_12,

  .KEY_GPIO_Port = GPIOE,
  .KEY_GPIO_Pin = GPIO_PIN_6,

  .currentSense_adc = &hadc1,
  .adc_resolution = 4096,
  .pwm_tim = &htim1,
  .pwm_frequency = CONFIG_PWM_FREQUENCY,
  // .update_t = 0.0001f,

};

Motor_t Motor_ODRIVE_3508 = {
  .isODrive = true,
  .pole_pairs = 7,
  // .controllerType = ControllerType_velocityOpenLoop,
  // .controllerType = ControllerType_velocityClosedLoop,
  .controllerType = CONFIG_CONTROLLER_TYPE,
  .isEnabled = 0,
  .voltage_alignSensor = 0.5f,
  .freq_currentLoop = CONFIG_PWM_FREQUENCY,
  .freq_velocityLoop = CONFIG_PWM_FREQUENCY / 5,
  .freq_positionLoop = CONFIG_PWM_FREQUENCY / 10,

};
CurrentSense_t CurrentSense_ODrive = {
  .gain = 40,
  .r_sample = 0.001f,
  .tim_channel = TIM_CHANNEL_4,
  .adc_type = ADCType_2ADC,
  .ia_phase = 1, // ia_phase is rank1's current
  .ib_phase = 1, // ib_phase is rank2's current
  .phase = CurrentSensePhase_BC,

};

// [NOTE] need to adjust
PID_t PID_ODrive_MOTOR3508 = {
  .PID_Id = {
    .P = 0.1f,
    .I = 1,
    .D = 0,
    .output_ramp = 1,
    .limit = 2, //v
    .update_t = (float)(1/CONFIG_PWM_FREQUENCY)
  },
  .PID_Iq = {
    .P = 0.6f,
    .I = 50.f,
    .D = 0,
    .output_ramp = 1,
    .limit = 2, //v
    .update_t = (float)(1/CONFIG_PWM_FREQUENCY)
  },
  .PID_iVelocity = {
    .P = 0.01f,// 0.01
    .I = 0.5f, // 0.5
    .D = 0,
    .output_ramp = 0,
    .limit = 10, //A
    .update_t = (float)(1/CONFIG_PWM_FREQUENCY),
    },

  .PID_iVelDegree = {
    .P = 10,
    .I = 0,
    .D = 0,
    .output_ramp = 0,
    .limit = 360*20,
    .update_t = (float)(1/CONFIG_PWM_FREQUENCY),
    },

  .PID_Velocity = {
    .P = 0.6f,
    .I = 1.5f,
    .D = 0,
    .output_ramp = 0,
    .limit = 2 ,//v
    .update_t = (float)(1/CONFIG_PWM_FREQUENCY),
    },

  .PID_Angle = {
    .P = 0,
    .I = 0,
    .D = 0,
    .output_ramp = 0,
    .limit = 0,
    .update_t = (float)(1/CONFIG_PWM_FREQUENCY),
    }

};

Motor_t Motor_ODRIVE_2804 = {
  .isODrive = true,
  .pole_pairs = 7,
  // .controllerType = ControllerType_velocityOpenLoop,
  // .controllerType = ControllerType_velocityClosedLoop,
  .controllerType = CONFIG_CONTROLLER_TYPE,
  .isEnabled = 0,
  .voltage_alignSensor = 2.f,
  .freq_currentLoop = CONFIG_PWM_FREQUENCY,
  .freq_velocityLoop = CONFIG_PWM_FREQUENCY / 5,
  .freq_positionLoop = CONFIG_PWM_FREQUENCY / 10,

};
Filter_t Filter_MOTOR2804 = {
  .lowPassFilter_ia = {
    .Tf = 0.01f
  },
  .lowPassFilter_ib = {
    .Tf = 0.01f
  },
  .lowPassFilter_ic = {
    .Tf = 0.001f
  },
  .lowPassFilter_velocity = {
    .Tf = 0.01f
  },
  .lowPassFilter_degree = {
    .Tf = 0.001f
  },
};

PID_t PID_ODrive_MOTOR2804 = {
  .PID_Id = {
    .P = 0.1f,
    .I = 10,
    .D = 0,
    .output_ramp = 1,
    .limit = 1, //v
    .update_t = (float)(1/CONFIG_PWM_FREQUENCY)
  },
  .PID_Iq = {
    .P = 1.f,
    .I = 50.f,
    .D = 0,
    .output_ramp = 1,
    .limit = 3, //v
    .update_t = (float)(1/CONFIG_PWM_FREQUENCY)
  },
  .PID_iVelocity = {
    .P = 0.0001f,// 0.01
    .I = 0.0f, // 30
    .D = 0,
    .output_ramp = 0,
    .limit = 2, //A
    .update_t = (float)(1/CONFIG_PWM_FREQUENCY),
    },

  .PID_iVelDegree = {
    .P = 0.1f,
    .I = 0,
    .D = 0,
    .output_ramp = 0,
    .limit = 360*20,
    .update_t = (float)(1/CONFIG_PWM_FREQUENCY),
    },

  .PID_Velocity = {
    .P = 1.f,
    .I = 2.f,
    .D = 0,
    .output_ramp = 0,
    .limit = 4 ,//v
    .update_t = (float)(1/CONFIG_PWM_FREQUENCY),
    },
  .PID_VelocityAngle = {
    .P = 0.1f,
    .I = 0.f,
    .D = 0,
    .output_ramp = 0,
    .limit = 360*20 ,//v
    .update_t = (float)(1/CONFIG_PWM_FREQUENCY),
    },

  .PID_Angle = {
    .P = 0,
    .I = 0,
    .D = 0,
    .output_ramp = 0,
    .limit = 0,
    .update_t = (float)(1/CONFIG_PWM_FREQUENCY),
    }

};

FOCDriver_t FOCDriver_MOTOR2804 = {
  .FOCModulationType = FOCModulationType_SinePWM,
  .voltage_power_supply = 12,
  .voltage_limit = 10,
  .pwm = {
    // .frequency = 10e3,
    .resolution = 4096,
    .pwm_type = _6PWM
  },
};
// DRV8301_CONFIG_t DRV830xConfig = {
//   .gain = 20,
// };

// static void ConfigMotor_DRV830X(Motor_t* motor, DRV8301_CONFIG_t* pDRV830xConfig, CurrentSense_t* pCurrentSense) {
//   motor->drv830xConfig = pDRV830xConfig;
//   pCurrentSense->gain = pDRV830xConfig->gain;
//
// }
static void ConfigMotor_DRV830X(Motor_t* motor, CurrentSense_t* pCurrentSense) {
  motor->drv830xConfig.gain = pCurrentSense->gain;
  // pCurrentSense->gain = pDRV830xConfig->gain;

}

static void ConfigMotor_Check(Motor_t* motor) {
  switch (motor->controllerType) {
  case ControllerType_velocityOpenLoop:
    printf("[INFO] VelocityOpenLoop!\n");
    break;
  case ControllerType_velocityClosedLoop:
    printf("[INFO] VelocityClosedLoop!\n");
    break;
  case ControllerType_currentClosedLoop:
    printf("[INFO] CurrentClosedLoop!\n");
    break;
  case ControllerType_currentVelocityClosedLoop:
    printf("[INFO] CurrentVelocityClosedLoop!\n");
    break;
  case ControllerType_currentVelocityAngleClosedLoop:
    printf("[INFO] CurrentVelocityAngleClosedLoop!\n");
    break;
    default:
    printf("[ERROR] Check controllerType!\n");
    break;
  }
}

void ConfigMotor_ODrive(Motor_t* motor) {
  // motor = &Motor_3508;
  Motor_t* pMotor = &Motor_ODRIVE_3508;
  FOCDriver_t* pFOCDriver = &FOCDriver_3508;
  PID_t* pPID = &PID_ODrive_MOTOR3508;
  Filter_t* pFilter = &Filter_MOTOR3508;

  // Motor_t* pMotor = &Motor_ODRIVE_2804;
  // FOCDriver_t* pFOCDriver = &FOCDriver_MOTOR2804;
  // Filter_t* pFilter = &Filter_MOTOR2804;
  // PID_t* pPID = &PID_ODrive_MOTOR2804;

  ConfigSTM32_t* pConfigSTM32 = &configSTM32F405VGT6;
  CurrentSense_t* pCurrentSense = &CurrentSense; // [NOTE] can't be changed!!!!!
  CurrentSense_t* pCurrentSenseVar = &CurrentSense_ODrive;
  // DRV8301_CONFIG_t* pDRV830xConfig = &DRV830xConfig;  //if NULL, It doesn't set drv830x


  ConfigMotor_Motor(motor, pMotor, pConfigSTM32);

  ConfigMotor_Init(motor, pCurrentSense, pFOCDriver, pConfigSTM32);
  ConfigMotor_CurrentSense(motor,pCurrentSense, pCurrentSenseVar, pConfigSTM32);
  if (motor->isODrive) {
    ConfigMotor_DRV830X(motor, pCurrentSense);
  }
  // ConfigMotor_CurrentSense(motor, pCurrentSense, pDRV830xConfig, pConfigSTM32);
  // ConfigMotor_Init(motor, &CurrentSense, &FOCDriver_3508, &configSTM32F407ZGT6);
  // ConfigMotor_KEY(motor, &key_enableMotor, &configSTM32F407ZGT6);
  ConfigMotor_KEY(motor, &key_enableMotor, pConfigSTM32);
  ConfigMotor_Filter(motor, pFilter, pConfigSTM32);
  ConfigMotor_PID(motor, pPID, pConfigSTM32);

  ConfigMotor_Check(motor);
  // motor->pole_pairs = Motor_3508.pole_pairs;
  // motor->controllerType = Motor_3508.controllerType;

}
