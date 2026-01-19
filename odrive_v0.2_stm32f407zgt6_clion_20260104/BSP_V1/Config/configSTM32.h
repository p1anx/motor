//
// Created by xwj on 1/13/26.
//

#ifndef ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_CONFIGSTM32_H
#define ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_CONFIGSTM32_H
// #include "stm32_hal.h"
#include "spi.h"


typedef struct ConfigSTM32_t ConfigSTM32_t;

struct ConfigSTM32_t {
  SPI_HandleTypeDef* sensor_spi;
  I2C_HandleTypeDef* sensor_i2c;
  ADC_HandleTypeDef* currentSense_adc;
  TIM_HandleTypeDef* pwm_tim;
  int pwm_frequency;
  int adc_resolution;
  float update_t;

  UART_HandleTypeDef* uart_printf;
  GPIO_TypeDef* EN_GPIO_Port;
  uint16_t     EN_GPIO_Pin;
  GPIO_TypeDef* CS_GPIO_Port;
  uint16_t     CS_GPIO_Pin;
  GPIO_TypeDef* KEY_GPIO_Port;
  uint16_t     KEY_GPIO_Pin;
  int sensor_direction;

};

#endif // ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_CONFIGSTM32_H
