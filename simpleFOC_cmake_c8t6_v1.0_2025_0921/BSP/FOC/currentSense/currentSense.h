#ifndef __CURRENTSENSE_H
#define __CURRENTSENSE_H

#include "stm32f1xx.h"
#include "stm32f1xx_hal_adc.h"
void test_adc(void);

typedef struct CurrentSense_t CurrentSense_t;

struct CurrentSense_t
{
    ADC_HandleTypeDef *adc1;
    ADC_HandleTypeDef *adc2;
    ADC_HandleTypeDef *adc;
    float offset;
    float i_a, i_b, i_c;
    float R_sample;
    float gain;
};

void CurrentSense_Init(CurrentSense_t *currentSense);
float CurrentSense_getOffset(CurrentSense_t *currentSense);
float CurrentSense_getRawCurrent(CurrentSense_t *currentSense);
float CurrentSense_getCurrent(CurrentSense_t *currentSense);
float CurrentSense_getRawVoltage(CurrentSense_t *currentSense);
#endif // !__CURRENTSENSE_H
