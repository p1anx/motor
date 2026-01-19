//
// Created by xwj on 1/13/26.
//

#include "config.h"
#include "configSTM32.h"
#include "configMotor.h"
#include "configDriver.h"
#include "LowpassFilter.h"

extern ADC_HandleTypeDef hadc1;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim1;

extern KEY_t key_enableMotor;

// ConfigSTM32_t stm32;
// ConfigMotor_t motor;
// ConfigDriver_t driver;
#define CONFIG_PWM_FREQUENCY 10e3
// #define CONFIG_CONTROLLER_TYPE ControllerType_currentClosedLoop
// #define CONFIG_CONTROLLER_TYPE ControllerType_currentVelocityClosedLoop
#define CONFIG_CONTROLLER_TYPE ControllerType_currentVelocityAngleClosedLoop
// #define CONFIG_CONTROLLER_TYPE ControllerType_velocityClosedLoop
// #define CONFIG_CONTROLLER_TYPE ControllerType_velocityOpenLoop

ConfigSTM32_t configSTM32F407ZGT6 = {
  .sensor_spi = &hspi1,
  .CS_GPIO_Port = GPIOA,
  .CS_GPIO_Pin = GPIO_PIN_4,
  .sensor_direction = 0,

  .EN_GPIO_Port = NULL,
  .EN_GPIO_Pin = 0,

  .KEY_GPIO_Port = GPIOE,
  .KEY_GPIO_Pin = GPIO_PIN_0,

  .currentSense_adc = &hadc1,
  .adc_resolution = 4096,
  .pwm_tim = &htim1,
  .pwm_frequency = CONFIG_PWM_FREQUENCY,
  // .update_t = 0.0001f,

};

CurrentSense_t CurrentSense = {
  .gain = 50,
  .r_sample = 0.001f,
  .tim_channel = TIM_CHANNEL_4,
  .adc_type = ADCType_2ADC,
  .ia_phase = -1,
  .ib_phase = -1,
  .ic_phase = -1,

};

FOCDriver_t FOCDriver_3508 = {
  .FOCModulationType = FOCModulationType_SinePWM,
  .voltage_power_supply = 15,
  .voltage_limit = 10,
  .pwm = {
    // .frequency = 10e3,
    .resolution = 4096,
    .pwm_type = _6PWM
  },
};


// Sensor_t Sensor_mt6835 = {
//   .sensorType = Sensor_MT6835,
//   .mt6835 = {
//     .direction = 0,
//    },
// };

Motor_t Motor_3508 = {
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

Filter_t Filter = {
  .lowPassFilter_ia = {
    .Tf = 0.001f
  },
  .lowPassFilter_ib = {
    .Tf = 0.001f
  },
  .lowPassFilter_ic = {
    .Tf = 0.001f
  },
  .lowPassFilter_velocity = {
    .Tf = 0.001f
  },
  .lowPassFilter_degree = {
    .Tf = 0.001f
  },

};

PID_t PID_3508 = {
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
    .P = 0.0052f,
    .I = 30.f,
    .D = 0,
    .output_ramp = 0,
    .limit = 10, //A
    .update_t = (float)(1/CONFIG_PWM_FREQUENCY),
    },

  .PID_iVelDegree = {
    .P = 60,
    .I = 0,
    .D = 0,
    .output_ramp = 0,
    .limit = 360*10,
    .update_t = (float)(1/CONFIG_PWM_FREQUENCY),
    },

  .PID_Velocity = {
    .P = 0.6f,
    .I = 1.f,
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

// Config_t config = {
//   .pCurrentSense = {
//
//   }
// };

// FOCDriver_t FOCDriver0 = {
//   .
//
// };


void Config_Init(Config_t* pConfig, ConfigSTM32_t* pConfigSTM32, CurrentSense_t* pCurrentSense, Sensor_t* pSensor) {
  pConfig->pCurrentSense = pCurrentSense;
  pConfig->pCurrentSense->adc.hadc = pConfigSTM32->currentSense_adc;
  pConfig->pCurrentSense->htim = pConfigSTM32->pwm_tim;
  pConfig->pCurrentSense->adc.resolution = pConfigSTM32->adc_resolution;
}

void ConfigMotor_Init(Motor_t* motor, CurrentSense_t* pCurrentSense, const FOCDriver_t* pFOCDriver, ConfigSTM32_t* pConfigSTM32) {
  motor->FOCDriver.FOCModulationType = pFOCDriver->FOCModulationType;
  motor->FOCDriver.voltage_power_supply = pFOCDriver->voltage_power_supply;
  motor->FOCDriver.voltage_limit = pFOCDriver->voltage_limit;

  motor->FOCDriver.pwm.htim = pConfigSTM32->pwm_tim;
  motor->FOCDriver.pwm.frequency = pConfigSTM32->pwm_frequency;
  // motor->FOCDriver.pwm.frequency = pFOCDriver->pwm.frequency;
  motor->FOCDriver.pwm.resolution = pFOCDriver->pwm.resolution;
  motor->FOCDriver.pwm.pwm_type = pFOCDriver->pwm.pwm_type;


  motor->pCurrentSense = pCurrentSense;
  motor->pCurrentSense->adc.hadc = pConfigSTM32->currentSense_adc;
  motor->pCurrentSense->htim = pConfigSTM32->pwm_tim;
  motor->pCurrentSense->adc.resolution = pConfigSTM32->adc_resolution;

  motor->Sensor.mt6835.hspi = pConfigSTM32->sensor_spi;
  motor->Sensor.mt6835.cs_port = pConfigSTM32->CS_GPIO_Port;
  motor->Sensor.mt6835.cs_pin = pConfigSTM32->CS_GPIO_Pin;
  // motor->Sensor.mt6835.delta_t = pConfigSTM32->update_t;
  motor->Sensor.mt6835.delta_t = 1.f/(float)pConfigSTM32->pwm_frequency;
  motor->Sensor.mt6835.direction = pConfigSTM32->sensor_direction;


}
void ConfigMotor_KEY(Motor_t* motor, KEY_t *key,ConfigSTM32_t* pConfigSTM32) {

  motor->pKEY = key;
  motor->pKEY->GPIO_Port = pConfigSTM32->KEY_GPIO_Port;
  motor->pKEY->GPIO_Pin = pConfigSTM32->KEY_GPIO_Pin;
  motor->pKEY->flag = 0;
}
void ConfigMotor_Motor(Motor_t* motor, const Motor_t* motor_u, ConfigSTM32_t* pConfigSTM32) {
  motor->pole_pairs = motor_u->pole_pairs;
  motor->controllerType = motor_u->controllerType;
  motor->isEnabled = motor_u->isEnabled; // motor is disabled
  motor->voltage_alignSensor = motor_u->voltage_alignSensor;
  motor->dt = 1.f / (float)pConfigSTM32->pwm_frequency;
  motor->freq_currentLoop = motor_u->freq_currentLoop;
  motor->freq_velocityLoop  = motor_u->freq_velocityLoop;
  motor->freq_positionLoop = motor_u->freq_positionLoop;
  motor->vel_period = motor->freq_currentLoop / motor->freq_velocityLoop;
  motor->pos_period = motor->freq_currentLoop / motor->freq_positionLoop;
}

void ConfigMotor_Filter(Motor_t* motor, const Filter_t* filter, ConfigSTM32_t* pConfigSTM32) {
  motor->lpf_ia.Tf = filter->lowPassFilter_ia.Tf;
  motor->lpf_ib.Tf = filter->lowPassFilter_ib.Tf;
  motor->lpf_ic.Tf = filter->lowPassFilter_ic.Tf;
  motor->lpf_id.Tf = filter->lowPassFilter_id.Tf;
  motor->lpf_iq.Tf = filter->lowPassFilter_iq.Tf;
  motor->lpf_velocity.Tf = filter->lowPassFilter_velocity.Tf;
  motor->lpf_degree.Tf = filter->lowPassFilter_degree.Tf;

}

void ConfigMotor_PID(Motor_t* motor, const PID_t* PID, ConfigSTM32_t* pConfigSTM32) {
  motor->PID_Velocity = PID->PID_Velocity;
  motor->PID_Id = PID->PID_Id;
  motor->PID_Iq = PID->PID_Iq;
  motor->PID_iVelocity = PID->PID_iVelocity;
  motor->PID_iVelDegree = PID->PID_iVelDegree;
  motor->PID_Current = PID->PID_Current;

}

void ConfigMotor(Motor_t* motor) {
  // motor = &Motor_3508;
  ConfigSTM32_t* pConfigSTM32 = &configSTM32F407ZGT6;
  Motor_t* pMotor = &Motor_3508;
  FOCDriver_t* pFOCDriver = &FOCDriver_3508;
  CurrentSense_t* pCurrentSense = &CurrentSense;
  Filter_t* pFilter = &Filter;
  PID_t* pPID = &PID_3508;

  ConfigMotor_Init(motor, pCurrentSense, pFOCDriver, pConfigSTM32);
  // ConfigMotor_Init(motor, &CurrentSense, &FOCDriver_3508, &configSTM32F407ZGT6);
  ConfigMotor_Motor(motor, pMotor, pConfigSTM32);
  // ConfigMotor_KEY(motor, &key_enableMotor, &configSTM32F407ZGT6);
  ConfigMotor_KEY(motor, &key_enableMotor, pConfigSTM32);
  ConfigMotor_Filter(motor, pFilter, pConfigSTM32);
  ConfigMotor_PID(motor, pPID, pConfigSTM32);
  // motor->pole_pairs = Motor_3508.pole_pairs;
  // motor->controllerType = Motor_3508.controllerType;

}




