//
// Created by xwj on 1/14/26.
//

#ifndef ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_ADC_SAMPLING_H
#define ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_ADC_SAMPLING_H
#include "stm32_hal.h"
typedef struct ADC_t ADC_t;

struct ADC_t {
  ADC_HandleTypeDef* hadc;
  float v_a, v_b, v_c;
  float v_scales;
  int resolution;


};

// void ADC_Injected_Init(ADC_t *adc);
void ADC_Injected_Init(ADC_t *adc, ADC_HandleTypeDef* hadc, int resolution);
void ADC_Injected_Get2Voltage(ADC_t* adc);
void ADC_Injected_Get3Voltage(ADC_t* adc);

void ADC_Example(void);

#endif // ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_ADC_SAMPLING_H
