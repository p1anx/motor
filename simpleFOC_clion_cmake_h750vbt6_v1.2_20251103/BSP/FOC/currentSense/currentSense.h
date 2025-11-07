//
// Created by xwj on 11/2/25.
//

#ifndef __CURRENTSENSE_H
#define __CURRENTSENSE_H

#include "BLDCMotor.h"
#include "stm32_hal.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;
#define currentSense_adc1 hadc1
#define currentSense_adc2 hadc2
#define currentSense_adc3 hadc3

#define R_SAMPLE 0.02
#define GAIN 6
// ADC_HandleTypeDef adc1;
// ADC_HandleTypeDef adc2;
// ADC_HandleTypeDef adc3;

typedef struct CurrentSense_t CurrentSense_t;

struct CurrentSense_t
{
    ADC_HandleTypeDef *adc1;
    ADC_HandleTypeDef *adc2;
    ADC_HandleTypeDef *adc3;
    ADC_HandleTypeDef *adc;
    float offset;
    float offset_a;
    float offset_b;
    float offset_c;
    float offset3[3];
    float i3[3];
    float i_a, i_b, i_c;
    float R_sample;
    float gain;
    int adc_resolution;
};

void test_adc(void);
void CurrentSense_Init(CurrentSense_t *currentSense);
float CurrentSense_getOffset(CurrentSense_t *currentSense);
float CurrentSense_getRawCurrent(CurrentSense_t *currentSense);
float CurrentSense_getCurrent(CurrentSense_t *currentSense);
float CurrentSense_getRawVoltage(CurrentSense_t *currentSense);
float CurrentSense_get3Current(CurrentSense_t *currentSense);
float CurrentSense_get3Offset(CurrentSense_t *currentSense);

float CurrentSense_read3Current(CurrentSense_t *pCurrentSense);

float CurrentSense_readCurrent(CurrentSense_t *pCurrentSense);

void CurrentSense_calibrate(CurrentSense_t *pCurrentSense);

float CurrentSense_readRawCurrent(ADC_HandleTypeDef *pADC);
// float CurrentSense_getCurrentDQ(BLDCMotor_t *motor, CurrentSense_t *currentSense);
uint16_t CurrentSense_getRawValue(ADC_HandleTypeDef *pADC);
#endif // !__CURRENTSENSE_H