//
// Created by xwj on 1/14/26.
//

#include "adc_sampling.h"

#include "stm32_hal.h"
#include <stdio.h>

ADC_t adc;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim1;

void ADC_Injected_Init(ADC_t *adc, ADC_HandleTypeDef* hadc, int resolution) {

  if (adc->hadc == NULL) {
    printf("[EEROR] hadc in adc_t is needed\n");
  }
  adc->resolution = resolution;
  adc->hadc = hadc;
  adc->v_scales = (3.3f/(float)adc->resolution);


  __HAL_ADC_ENABLE_IT(adc->hadc, ADC_IT_JEOC);
  if (HAL_ADCEx_InjectedStart_IT(adc->hadc) != HAL_OK) {
    printf("[ERROR] FAILED TO START ADC\n");
    return;
  }
}

uint32_t ADC_Injected_GetValue(ADC_HandleTypeDef* ADCx, uint32_t ch) {

}
void ADC_Injected_Get2Voltage(ADC_t* adc) {
  // const float v_scales  = (float)(3.3f / adc->resolution);
  adc->v_a = (float)HAL_ADCEx_InjectedGetValue(adc->hadc, ADC_INJECTED_RANK_1) * adc->v_scales;
  adc->v_b = (float)HAL_ADCEx_InjectedGetValue(adc->hadc, ADC_INJECTED_RANK_2) * adc->v_scales;

}
void ADC_Injected_Get3Voltage(ADC_t* adc) {
  // const float v_scales  = (float)(3.3f / adc->resolution);
  adc->v_a = (float)HAL_ADCEx_InjectedGetValue(adc->hadc, ADC_INJECTED_RANK_1) * adc->v_scales;
  adc->v_b = (float)HAL_ADCEx_InjectedGetValue(adc->hadc, ADC_INJECTED_RANK_2) * adc->v_scales;
  adc->v_c = (float)HAL_ADCEx_InjectedGetValue(adc->hadc, ADC_INJECTED_RANK_3) * adc->v_scales;

}
void ADC_Example(void) {
  printf("adc test\n");

  // adc.hadc = &hadc1;
  const int resolution = 4096;
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 1);
  ADC_Injected_Init(&adc, &hadc1, resolution);
  while (1) {
    // ADC_Injected_Get2Voltage(&adc);
    // printf("adc: %f v, %f v\n", adc.v_a, adc.v_b);
    // HAL_Delay(1000);
  }

}