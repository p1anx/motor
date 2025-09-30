#ifndef __CURRENTSENSE_H
#define __CURRENTSENSE_H

#include "mymain.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal_adc.h"
#include "adc.h"
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
    float offset3[3];
    float i3[3];
    float i_a, i_b, i_c;
    float R_sample;
    float gain;
};

void test_adc(void);
void CurrentSense_Init(CurrentSense_t *currentSense);
float CurrentSense_getOffset(CurrentSense_t *currentSense);
float CurrentSense_getRawCurrent(CurrentSense_t *currentSense);
float CurrentSense_getCurrent(CurrentSense_t *currentSense);
float CurrentSense_getRawVoltage(CurrentSense_t *currentSense);
float CurrentSense_get3Current(CurrentSense_t *currentSense);
float CurrentSense_get3Offset(CurrentSense_t *currentSense);
#endif // !__CURRENTSENSE_H
