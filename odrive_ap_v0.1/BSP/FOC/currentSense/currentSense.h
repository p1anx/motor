//
// Created by xwj on 11/2/25.
//

#ifndef __CURRENTSENSE_H
#define __CURRENTSENSE_H

// #include "BLDCMotor.h"
#include "stm32_hal.h"
#include "motor_config.h"
#include <stdbool.h>

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;
#define currentSense_adc1 CONFIG_STM32_ADC
#define currentSense_adc2 hadc1
#define currentSense_adc3 hadc1
#define ADC_RESOLUTION CONFIG_STM32_ADC_RESOLUTION

#define R_SAMPLE           CONFIG_DRIVER_R_SAMPLE //0.02
#define AMPLIFIER_GAIN     CONFIG_DRIVER_GAIN       //6
// ADC_HandleTypeDef adc1;
// ADC_HandleTypeDef adc2;
// ADC_HandleTypeDef adc3;
typedef enum {
    ADCType_2ADC,
    ADCType_3ADC,
}CurrentSense_ADC_Type;

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
    float i_alpha, i_beta;
    float i_d, i_q;
    float v_a, v_b, v_c;
    float R_sample;
    float gain;
    int adc_resolution;
    int isCalibrating;
    int adc_flag;
    volatile bool isReady;
    bool isSetIc;
    CurrentSense_ADC_Type adc_type;
    // LowPassFilter_t lpf_d;
    // LowPassFilter_t lpf_q;
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

uint16_t CurrentSense_Sampling(CurrentSense_t *currentSense);
int  CurrentSense_Scan(CurrentSense_t *pCurrentSense, const int channel_num, int *rawValue);
// int currentSense_get2PhaseCurrent(CurrentSense_t *pCurrentSense);
int currentSense_get2PhaseVoltage(CurrentSense_t *pCurrentSense);
int currentSense_get3PhaseVoltage(CurrentSense_t *pCurrentSense);
void CurrentSense_Scan_Init(CurrentSense_t *currentSense);
int currentSense_get3RawPhaseVoltage(CurrentSense_t *pCurrentSense, float *rawVoltage);

int currentSense_2PhaseCurrentCalibrate(CurrentSense_t *pCurrentSense);

int currentSense_3PhaseCurrentCalibrate(CurrentSense_t *pCurrentSense);
int currentSense_get2RawPhaseCurrent(CurrentSense_t *pCurrentSense);
int currentSense_get2PhaseCurrent(CurrentSense_t *pCurrentSense);
int currentSense_get3PhaseCurrent(CurrentSense_t *pCurrentSense);

void CurrentSense_InitADC(CurrentSense_t *currentSense);
void currentSense_getCurrentDQ_NotFilter(CurrentSense_t *currentSense);
void currentSense_getCurrentDQ(CurrentSense_t *currentSense);
void currentSense_3currentCalibrationVoltage(CurrentSense_t *currentSense);
void currentSense_3currentVoltage(CurrentSense_t *currentSense);
void currentSense_clarkParkTransform(CurrentSense_t *currentSense, float e_angle);
uint32_t currentSense_pollingADCRawValue(ADC_HandleTypeDef *adc);
void currentSense_pollingCalibrate(CurrentSense_t *pCurrentSense);
void CurrentSense_InitADC_Regular(CurrentSense_t *currentSense);
void currentSense_pollingGetCurrent(CurrentSense_t *pCurrentSense);
float currentSense_pollingGetVoltage(CurrentSense_t *pCurrent_sense);
void currentSense_Clark_Park_IaIb(float Ia, float Ib, float *Id, float *Iq, float e_angle);
void currentSense_Clark_Park_IbIc(float Ia, float Ib, float *Id, float *Iq, float e_angle);

void CurrentSense_2ADC_Init(CurrentSense_t *currentSense);
void currentSense_2currentVoltage(CurrentSense_t *currentSense);
void currentSense_2currentCalibrationVoltage(CurrentSense_t *currentSense);
void currentSense_currentCalibrationVoltage(CurrentSense_t *currentSense);

#endif // !__CURRENTSENSE_H