//
// Created by xwj on 1/14/26.
//

#include "currentSense.h"
// #include "stm32_hal.h"
#include "adc.h"

#include <stdio.h>

CurrentSense_t CurrentSense;
extern ADC_t adc;
extern TIM_HandleTypeDef htim1;
#define IS_TESTING_ADC 0

void CurrentSense_Example(void) {
  printf("This is the current sense example\n");
  // CurrentSense.pADC = &adc;
  // CurrentSense.pADC->hadc = &hadc1;
  // CurrentSense.pADC->resolution = 4096;
  CurrentSense.adc.hadc = &hadc1;
  CurrentSense.adc.resolution = 4096;
  CurrentSense.gain = 50;
  CurrentSense.r_sample = 0.001f;

  CurrentSense.htim = &htim1;
  CurrentSense.tim_channel = TIM_CHANNEL_4;

  // HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
  // __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 1);
  CurrentSense_Init(&CurrentSense);

  HAL_TIM_PWM_Start(CurrentSense.htim, CurrentSense.tim_channel);

  int frequency = 10e3;
  int arr = SystemCoreClock / 2 /frequency;
  __HAL_TIM_SET_AUTORELOAD(CurrentSense.htim, arr - 1);
  __HAL_TIM_SET_COMPARE(CurrentSense.htim, CurrentSense.tim_channel, 1);
  printf("pwm = %d Hz, adc resolution = %d\n", frequency, CurrentSense.adc.resolution);

}

void CurrentSense_Init(CurrentSense_t* pCurrentSense) {
  if (!pCurrentSense) {
    printf("[ERROR] CurrentSense IS NOT SET\n");
    return;
  }
  pCurrentSense->i_scales = 1.f / ((float)pCurrentSense->gain * pCurrentSense->r_sample);
  pCurrentSense->isCalibrating = 1;
  pCurrentSense->isReady = 0;
  if (pCurrentSense->adc.hadc == NULL) {
    printf("[ERROR] CurrentSense ADC is WRONG\n");
    return;
  }
  ADC_Injected_Init(&pCurrentSense->adc, pCurrentSense->adc.hadc, pCurrentSense->adc.resolution);

}
void CurrentSense_Get2RawVoltage(CurrentSense_t* pCurrentSense) {
  ADC_Injected_Get2Voltage(&pCurrentSense->adc);
  pCurrentSense->v_a = pCurrentSense->adc.v_a;
  pCurrentSense->v_b = pCurrentSense->adc.v_b;
}
void CurrentSense_Get3RawVoltage(CurrentSense_t* pCurrentSense) {
  ADC_Injected_Get3Voltage(&pCurrentSense->adc);
  pCurrentSense->v_a = pCurrentSense->adc.v_a;
  pCurrentSense->v_b = pCurrentSense->adc.v_b;
  pCurrentSense->v_c = pCurrentSense->adc.v_c;
}
void CurrentSense_Get2RawCurrent(CurrentSense_t* pCurrentSense) {
  // const float i_scales = 1.f / ((float)pCurrentSense->gain * pCurrentSense->r_sample);
  // ADC_Injected_Get2Voltage(pCurrentSense->pADC);
  CurrentSense_Get2RawVoltage(pCurrentSense);
  pCurrentSense->i_a = pCurrentSense->adc.v_a * pCurrentSense->i_scales;
  pCurrentSense->i_b = pCurrentSense->adc.v_b * pCurrentSense->i_scales;

}
void CurrentSense_Get3RawCurrent(CurrentSense_t* pCurrentSense) {
  // const float i_scales = 1.f / ((float)pCurrentSense->gain * pCurrentSense->r_sample);
  // ADC_Injected_Get3Voltage(pCurrentSense->pADC);
  CurrentSense_Get3RawVoltage(pCurrentSense);
  pCurrentSense->i_a = pCurrentSense->adc.v_a * pCurrentSense->i_scales;
  pCurrentSense->i_b = pCurrentSense->adc.v_b * pCurrentSense->i_scales;
  pCurrentSense->i_c = pCurrentSense->adc.v_c * pCurrentSense->i_scales;

}
void CurrentSense_Get2Current(CurrentSense_t* pCurrentSense) {
  CurrentSense_Get2RawVoltage(pCurrentSense);
  pCurrentSense->i_a = pCurrentSense->ia_phase * (pCurrentSense->v_a - pCurrentSense->offset_va) * pCurrentSense->i_scales;
  pCurrentSense->i_b = pCurrentSense->ib_phase * (pCurrentSense->v_b - pCurrentSense->offset_vb) * pCurrentSense->i_scales;

}

void CurrentSense_Get3Current(CurrentSense_t* pCurrentSense) {
  CurrentSense_Get3RawVoltage(pCurrentSense);
  pCurrentSense->i_a = pCurrentSense->ia_phase * (pCurrentSense->v_a - pCurrentSense->offset_va) * pCurrentSense->i_scales;
  pCurrentSense->i_b = pCurrentSense->ib_phase * (pCurrentSense->v_b - pCurrentSense->offset_vb) * pCurrentSense->i_scales;
  pCurrentSense->i_c = pCurrentSense->ic_phase * (pCurrentSense->v_c - pCurrentSense->offset_vc) * pCurrentSense->i_scales;

}
void CurrentSense_GetCurrent(CurrentSense_t* pCurrentSense) {

  switch (pCurrentSense->adc_type) {
    case ADCType_2ADC:
      CurrentSense_Get2Current(pCurrentSense);
      break;
    case ADCType_3ADC:
      CurrentSense_Get3Current(pCurrentSense);
      break;
    default:
      printf("[ERROR] unknow current adc type\n");
      break;
  }

}

void CurrentSense_Calibrate2Voltage(CurrentSense_t* pCurrentSense) {
  const int sample_num = 1000;
  static int sample_index = 0;
  static float va_sum = 0, vb_sum = 0, vc_sum = 0;
  if (sample_index++ < sample_num) {
    CurrentSense_Get2RawVoltage(pCurrentSense);
    va_sum += pCurrentSense->v_a;
    vb_sum += pCurrentSense->v_b;
  }
  if (sample_index == sample_num) {
    pCurrentSense->offset_va = va_sum / (float)sample_num;
    pCurrentSense->offset_vb = vb_sum / (float)sample_num;
    printf("[INFO] Two Offsets: Va = %f v, Vb = %f v\n", pCurrentSense->offset_va, pCurrentSense->offset_vb);
    pCurrentSense->isCalibrating = 0;
    return;
  }
}
void CurrentSense_Calibrate3Voltage(CurrentSense_t* pCurrentSense) {
  const int sample_num = 1000;
  static int sample_index = 0;
  static float va_sum = 0, vb_sum = 0, vc_sum = 0;
  if (sample_index++ < sample_num) {
    CurrentSense_Get3RawVoltage(pCurrentSense);
    va_sum += pCurrentSense->v_a;
    vb_sum += pCurrentSense->v_b;
    vc_sum += pCurrentSense->v_c;
  }
  if (sample_index == sample_num) {
    pCurrentSense->offset_va = va_sum / (float)sample_num;
    pCurrentSense->offset_vb = vb_sum / (float)sample_num;
    pCurrentSense->offset_vc = vc_sum / (float)sample_num;
    printf("[INFO] Three Offsets: Va = %f v, Vb = %f v, Vc = %f\n", pCurrentSense->offset_va, pCurrentSense->offset_vb, pCurrentSense->offset_vc);
    pCurrentSense->isCalibrating = 0;
    return;
  }

}
void CurrentSense_Calibrate(CurrentSense_t* pCurrentSense) {
  switch (pCurrentSense->adc_type) {
    case ADCType_2ADC:
      CurrentSense_Calibrate2Voltage(pCurrentSense);
      break;
    case ADCType_3ADC:
      CurrentSense_Calibrate3Voltage(pCurrentSense);
      break;
    default:
      printf("[ERROR] Unknown ADC type\n");
    break;
  }
}

int i = 0;
void CurrentSense_ADC_Callback(ADC_HandleTypeDef *hadc) {

  if (hadc->Instance == ADC1) {
    if (CurrentSense.isCalibrating) {
      CurrentSense_Calibrate(&CurrentSense);
    }
    else {
      if (!CurrentSense.isReady) {
        // i++;
        if (i++ >= 10000) {
          i = 0;
        }
        CurrentSense_GetCurrent(&CurrentSense);
        // printf("%f,%f\n",CurrentSense.v_a,CurrentSense.v_b);
        CurrentSense.isReady = 1;

      }
    }
  }

}
void CurrentSense_ADC_Callback_Test(ADC_HandleTypeDef *hadc) {

  if (hadc->Instance == ADC1) {
    CurrentSense_Get2RawVoltage(&CurrentSense);
    printf("%f,%f\n",CurrentSense.v_a,CurrentSense.v_b);
  }
}

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc) {

#if IS_TESTING_ADC
  CurrentSense_ADC_Callback_Test(hadc);
#else
  CurrentSense_ADC_Callback(hadc);
#endif

  // 1.
  // float i = 1.1f;
  // if (hadc->Instance == ADC1) {
  //   ADC_Injected_Get2Voltage(&adc);
  //   printf("adc: %f v, %f v, %f\n", adc.v_a, adc.v_b, i);
  // }
}