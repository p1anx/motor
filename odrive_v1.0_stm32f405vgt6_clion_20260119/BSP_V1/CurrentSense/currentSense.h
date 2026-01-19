//
// Created by xwj on 1/14/26.
//

#ifndef ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_CURRENTSENSE_H
#define ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_CURRENTSENSE_H

#include "adc_sampling.h"
typedef enum {
  ADCType_2ADC,
  ADCType_3ADC,

}ADCType;

typedef struct CurrentSense_t CurrentSense_t;
struct CurrentSense_t{
  ADC_t* pADC;
  ADC_t adc;
  ADCType adc_type;
  TIM_HandleTypeDef* htim;
  uint32_t tim_channel;
  float i_a, i_b, i_c, i_d, i_q;
  int ia_phase, ib_phase, ic_phase;
  float v_a, v_b, v_c;
  float offset_va, offset_vb, offset_vc;
  float i_scales;
  float r_sample;
  int gain;
  int isCalibrating, isReady;


};

void CurrentSense_Init(CurrentSense_t* pCurrentSense);
void CurrentSense_Get2RawVoltage(CurrentSense_t* pCurrentSense);
void CurrentSense_Get3RawVoltage(CurrentSense_t* pCurrentSense);
void CurrentSense_Get2RawCurrent(CurrentSense_t* pCurrentSense);
void CurrentSense_Get3RawCurrent(CurrentSense_t* pCurrentSense);
void CurrentSense_Get2Current(CurrentSense_t* pCurrentSense);
void CurrentSense_Get3Current(CurrentSense_t* pCurrentSense);
void CurrentSense_GetCurrent(CurrentSense_t* pCurrentSense);
void CurrentSense_Calibrate(CurrentSense_t* pCurrentSense);

void CurrentSense_Example(void);

#endif // ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_CURRENTSENSE_H
