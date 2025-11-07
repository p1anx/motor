//
// Created by xwj on 11/2/25.
//

#include "currentSense.h"
// #include "BLDCMotor.h"
#include "lowpass_filter.h"
#include "stm32_hal.h"
#include <stdio.h>

CurrentSense_t currentSense;

void test_adc(void)
{
    // HAL_ADCEx_Calibration_Start(&currentSense_adc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
    uint16_t ADC_Value;
    static int i = 0;
    while (1)
    {
        int t1 = HAL_GetTick();
        HAL_ADC_Start(&currentSense_adc1);
        HAL_ADC_PollForConversion(&currentSense_adc1, 20);
        if (HAL_IS_BIT_SET(HAL_ADC_GetState(&currentSense_adc1), HAL_ADC_STATE_REG_EOC))
        {
            ADC_Value = HAL_ADC_GetValue(&currentSense_adc1); // 获取AD值

            printf("%.4f\n", ADC_Value * 3.3f / 65535);
            int t2 = HAL_GetTick();
            printf("t = %d\n", t2 - t1);

            // if (i > 1)
            // {
                //1.
                // printf("ADC1 Reading : %d \r\n", ADC_Value);
                // printf("True Voltage value : %.4f \r\n", ADC_Value * 3.3f / 65535);
                //2.
                /*
                printf("%.4f\n", ADC_Value * 3.3f / 65535);
                i = 0;
            }
            */
            i++;
        }
    }
}
void CurrentSense_Init(CurrentSense_t *currentSense)
{
    currentSense->gain = GAIN;
    currentSense->R_sample = R_SAMPLE;
    currentSense->adc1 = &currentSense_adc1;
    currentSense->adc2 = &currentSense_adc2;
    currentSense->adc3 = &currentSense_adc3;
    currentSense->adc = &currentSense_adc3;
    currentSense->adc_resolution = 4096; //16bit
    // 1.
    //  CurrentSense_get3Offset(currentSense);
    //  2.
    CurrentSense_calibrate(currentSense);
}
void CurrentSense_Scan_Init(CurrentSense_t *currentSense)
{
    currentSense->gain = GAIN;
    currentSense->R_sample = R_SAMPLE;
    currentSense->adc = &currentSense_adc1;
    currentSense->adc_resolution = 4096; //16bit
    currentSense_3PhaseCurrentCalibrate(currentSense);
}

void CurrentSense_InitADC(CurrentSense_t *currentSense)
{
    currentSense->gain = GAIN;
    currentSense->R_sample = R_SAMPLE;
    currentSense->adc = &currentSense_adc1;
    currentSense->adc_resolution = 4096; //16bit
    currentSense->isCalibrating = 1;
    currentSense->adc_flag = 0;
    __HAL_ADC_ENABLE_IT(currentSense->adc, ADC_IT_JEOC);
    HAL_ADCEx_InjectedStart_IT(currentSense->adc);
}
uint16_t CurrentSense_Sampling(CurrentSense_t *currentSense)
{
    uint16_t adc_value;

    HAL_ADC_Start(currentSense->adc);
    HAL_ADC_PollForConversion(currentSense->adc, 20);
    if (HAL_IS_BIT_SET(HAL_ADC_GetState(currentSense->adc), HAL_ADC_STATE_REG_EOC))
    {

        // ADC_Value = HAL_ADC_GetValue(&hadc1); // 获取AD值
        // printf("ADC1 Reading : %d \r\n", ADC_Value);
        adc_value = HAL_ADC_GetValue(currentSense->adc);
        // printf("%.4f,", adc_value * 3.3f / 4096);
    }
    return adc_value;
}
int  CurrentSense_Scan(CurrentSense_t *pCurrentSense, const int channel_num, int *rawValue)
{
    int adc_value[channel_num];
    for (int i = 0; i < channel_num; i++)
    {
        rawValue[i] = CurrentSense_Sampling(pCurrentSense);
        // printf("raw: %d\n", adc_value[i]);
    }
    return 0;
}

int currentSense_get2PhaseCurrent(CurrentSense_t *pCurrentSense)
{
    int adc_value[2];
    CurrentSense_Scan(pCurrentSense, 2, adc_value);
    float scale = 3.3f/((float)pCurrentSense->adc_resolution*pCurrentSense->R_sample*pCurrentSense->gain);
    pCurrentSense->i_a = adc_value[0]*scale - pCurrentSense->offset_a;
    pCurrentSense->i_b = adc_value[1]*scale - pCurrentSense->offset_b;
    return 0;
}
int currentSense_get3PhaseCurrent(CurrentSense_t *pCurrentSense)
{
    int adc_value[3];
    CurrentSense_Scan(pCurrentSense, 3, adc_value);
    float scale = 3.3f/((float)pCurrentSense->adc_resolution*pCurrentSense->R_sample*pCurrentSense->gain);
    pCurrentSense->i_a = adc_value[0]*scale - pCurrentSense->offset_a;
    pCurrentSense->i_b = adc_value[1]*scale - pCurrentSense->offset_b;
    pCurrentSense->i_c = adc_value[2]*scale - pCurrentSense->offset_c;
    return 0;
}
int currentSense_get2RawPhaseCurrent(CurrentSense_t *pCurrentSense)
{
    int adc_value[2];
    CurrentSense_Scan(pCurrentSense, 2, adc_value);
    float scale = 3.3f/((float)pCurrentSense->adc_resolution*pCurrentSense->R_sample*pCurrentSense->gain);
    pCurrentSense->i_a = adc_value[0]*scale;
    pCurrentSense->i_b = adc_value[1]*scale;
    return 0;
}
int currentSense_get2PhaseVoltage(CurrentSense_t *pCurrentSense)
{
    int adc_value[2];
    CurrentSense_Scan(pCurrentSense, 2, adc_value);
    const float scale = 3.3f/((float)pCurrentSense->adc_resolution*pCurrentSense->gain);
    pCurrentSense->i_a = (float)adc_value[0]*scale;
    pCurrentSense->i_b = (float)adc_value[1]*scale;
    return 0;
}

int currentSense_get3PhaseVoltage(CurrentSense_t *pCurrentSense)
{
    int adc_value[3];
    CurrentSense_Scan(pCurrentSense, 3, adc_value);
    const float scale = 3.3f/((float)pCurrentSense->adc_resolution*pCurrentSense->gain);
    pCurrentSense->i_a = (float)adc_value[0]*scale;
    pCurrentSense->i_b = (float)adc_value[1]*scale;
    pCurrentSense->i_c = (float)adc_value[2]*scale;
    return 0;
}

int currentSense_3PhaseCurrentCalibrate(CurrentSense_t *pCurrentSense)
{
    int adc_value[3];
    float ia_sum = 0.0f, ib_sum = 0.0f, ic_sum = 0.0f;
    const float scale = 3.3f/((float)pCurrentSense->adc_resolution*pCurrentSense->gain*pCurrentSense->R_sample);
    const int sample_n = 1000;
    printf("[INFO] Calibrating Current\n");
    for (int i = 0; i < sample_n; i++)
    {
        CurrentSense_Scan(pCurrentSense, 3, adc_value);
        ia_sum += (float)adc_value[0]*scale;
        ib_sum += (float)adc_value[1]*scale;
        ic_sum += (float)adc_value[2]*scale;
    }
    pCurrentSense->offset_a = ia_sum/sample_n;
    pCurrentSense->offset_b = ib_sum/sample_n;
    pCurrentSense->offset_c = ic_sum/sample_n;
    return 0;
}
int currentSense_2PhaseCurrentCalibrate(CurrentSense_t *pCurrentSense)
{
    const int channel_num = 2;
    int adc_value[channel_num];
    float ia_sum = 0.0f, ib_sum = 0.0f;
    const float scale = 3.3f/((float)pCurrentSense->adc_resolution*pCurrentSense->gain*pCurrentSense->R_sample);
    const int sample_n = 1000;
    for (int i = 0; i < sample_n; i++)
    {
        CurrentSense_Scan(pCurrentSense, channel_num, adc_value);
        ia_sum += (float)adc_value[0]*scale;
        ib_sum += (float)adc_value[1]*scale;
    }
    pCurrentSense->offset_a = ia_sum/sample_n;
    pCurrentSense->offset_b = ib_sum/sample_n;
    return 0;
}

int currentSense_get3RawPhaseVoltage(CurrentSense_t *pCurrentSense, float *rawVoltage)
{
    int adc_value[3];
    CurrentSense_Scan(pCurrentSense, 3, adc_value);
    const float scale = 3.3f/((float)pCurrentSense->adc_resolution);
    rawVoltage[0] = (float)adc_value[0]*scale;
    rawVoltage[1] = (float)adc_value[1]*scale;
    rawVoltage[2] = (float)adc_value[2]*scale;
    return 0;
}
float CurrentSense_getOffset_v0(CurrentSense_t *currentSense)
{
    float current_sum = 0;
    for (int i = 0; i < 1000; i++)
    {
        current_sum += CurrentSense_getRawCurrent(currentSense);
    }
    currentSense->offset = current_sum / 1000;
    return currentSense->offset;
}
float CurrentSense_get3Offset_v0(CurrentSense_t *currentSense)
{
    currentSense->adc = currentSense->adc1;
    currentSense->offset3[0] = CurrentSense_getOffset(currentSense);
    currentSense->adc = currentSense->adc2;
    currentSense->offset3[1] = CurrentSense_getOffset(currentSense);
    currentSense->adc = currentSense->adc3;
    currentSense->offset3[2] = CurrentSense_getOffset(currentSense);
    return 0;
}
float CurrentSense_getOffset(CurrentSense_t *currentSense)
{
    float current_sum = 0;
    for (int i = 0; i < 1000; i++)
    {
        current_sum += CurrentSense_getRawCurrent(currentSense);
    }
    currentSense->offset = current_sum / 1000;
    return currentSense->offset;
}
float CurrentSense_get3Offset(CurrentSense_t *currentSense)
{
    currentSense->adc = currentSense->adc1;
    currentSense->offset3[0] = CurrentSense_getOffset(currentSense);
    currentSense->adc = currentSense->adc2;
    currentSense->offset3[1] = CurrentSense_getOffset(currentSense);
    currentSense->adc = currentSense->adc3;
    currentSense->offset3[2] = CurrentSense_getOffset(currentSense);
    return 0;
}
float CurrentSense_getRawCurrent_v0(CurrentSense_t *currentSense)
{

    uint16_t adc_value;
    float current;

    HAL_ADC_Start(currentSense->adc);
    HAL_ADC_PollForConversion(currentSense->adc, 20);
    if (HAL_IS_BIT_SET(HAL_ADC_GetState(currentSense->adc), HAL_ADC_STATE_REG_EOC))
    {

        // ADC_Value = HAL_ADC_GetValue(&hadc1); // 获取AD值
        // printf("ADC1 Reading : %d \r\n", ADC_Value);
        // printf("True Voltage value : %.4f \r\n", ADC_Value * 3.3f / 4096);
        adc_value = HAL_ADC_GetValue(currentSense->adc);
    }
    current = adc_value * 3.3 / (4096 * currentSense->R_sample * currentSense->gain);
    return current;
}
float CurrentSense_readRawCurrent(ADC_HandleTypeDef *pADC)
{
    uint16_t adc_value;

    HAL_ADC_Start(pADC);
    HAL_ADC_PollForConversion(pADC, 20);
    if (HAL_IS_BIT_SET(HAL_ADC_GetState(pADC), HAL_ADC_STATE_REG_EOC))
    {

        // ADC_Value = HAL_ADC_GetValue(&hadc1); // 获取AD值
        // printf("ADC1 Reading : %d \r\n", ADC_Value);
        adc_value = HAL_ADC_GetValue(pADC);
        // printf("%.4f,", adc_value * 3.3f / 4096);
    }
    return adc_value;
}

uint16_t CurrentSense_getRawValue(ADC_HandleTypeDef *pADC)
{
    uint16_t adc_value;

    HAL_ADC_Start(pADC);
    HAL_ADC_PollForConversion(pADC, 20);
    if (HAL_IS_BIT_SET(HAL_ADC_GetState(pADC), HAL_ADC_STATE_REG_EOC))
    {

        // ADC_Value = HAL_ADC_GetValue(&hadc1); // 获取AD值
        // printf("ADC1 Reading : %d \r\n", ADC_Value);
        adc_value = HAL_ADC_GetValue(pADC);
        // printf("%.4f,", adc_value * 3.3f / 4096);
    }
    return adc_value;
}
void CurrentSense_calibrate(CurrentSense_t *pCurrentSense)
{
    float sum_a = 0, sum_b = 0, sum_c = 0;
    int samples = 100;
    for (int i = 0; i < samples; i++)
    {
        sum_a += CurrentSense_readRawCurrent(pCurrentSense->adc1);
        sum_b += CurrentSense_readRawCurrent(pCurrentSense->adc2);
        sum_c += CurrentSense_readRawCurrent(pCurrentSense->adc3);
        delay_ms(1);
    }
    pCurrentSense->offset_a = sum_a / samples;
    pCurrentSense->offset_b = sum_b / samples;
    pCurrentSense->offset_c = sum_c / samples;
    printf("current offset: a=%.2f v, b=%.2f v, c=%.2f v\n", pCurrentSense->offset_a / pCurrentSense->adc_resolution * 3.3, pCurrentSense->offset_b/ pCurrentSense->adc_resolution * 3.3, pCurrentSense->offset_c / pCurrentSense->adc_resolution * 3.3);
}

float CurrentSense_readCurrent(CurrentSense_t *pCurrentSense)
{
    float adc_rawValue = CurrentSense_readRawCurrent(pCurrentSense->adc);
    float adc_value = adc_rawValue * 3.3 / (pCurrentSense->adc_resolution * pCurrentSense->R_sample * pCurrentSense->gain);
    return adc_value;
}
float CurrentSense_read3Current(CurrentSense_t *pCurrentSense)
{
    float scales = 3.3 / (pCurrentSense->adc_resolution * pCurrentSense->R_sample * pCurrentSense->gain);
    pCurrentSense->i_a = (CurrentSense_readRawCurrent(pCurrentSense->adc1) - pCurrentSense->offset_a) * scales;
    pCurrentSense->i_b = (CurrentSense_readRawCurrent(pCurrentSense->adc2) - pCurrentSense->offset_a) * scales;
    pCurrentSense->i_c = (CurrentSense_readRawCurrent(pCurrentSense->adc3) - pCurrentSense->offset_a) * scales;
    return 0;
}

float CurrentSense_getRawCurrent(CurrentSense_t *currentSense)
{

    uint16_t adc_value;
    float current;

    HAL_ADC_Start(currentSense->adc);
    HAL_ADC_PollForConversion(currentSense->adc, 20);
    if (HAL_IS_BIT_SET(HAL_ADC_GetState(currentSense->adc), HAL_ADC_STATE_REG_EOC))
    {

        // ADC_Value = HAL_ADC_GetValue(&hadc1); // 获取AD值
        // printf("ADC1 Reading : %d \r\n", ADC_Value);
        // printf("True Voltage value : %.4f \r\n", ADC_Value * 3.3f / 4096);
        adc_value = HAL_ADC_GetValue(currentSense->adc);
    }
    current = adc_value * 3.3 / (currentSense->adc_resolution * currentSense->R_sample * currentSense->gain);
    return current;
}
float CurrentSense_getRawVoltage(CurrentSense_t *currentSense)
{

    uint16_t adc_value;
    float v;

    HAL_ADC_Start(currentSense->adc);
    HAL_ADC_PollForConversion(currentSense->adc, 20);
    if (HAL_IS_BIT_SET(HAL_ADC_GetState(currentSense->adc), HAL_ADC_STATE_REG_EOC))
    {

        // ADC_Value = HAL_ADC_GetValue(&hadc1); // 获取AD值
        // printf("ADC1 Reading : %d \r\n", ADC_Value);
        // printf("True Voltage value : %.4f \r\n", ADC_Value * 3.3f / 4096);
        adc_value = HAL_ADC_GetValue(currentSense->adc);
    }
    v = adc_value * 3.3 / (currentSense->adc_resolution);
    return v;
}

float CurrentSense_getCurrent(CurrentSense_t *currentSense)
{

    uint16_t adc_value;
    float current;

    HAL_ADC_Start(currentSense->adc);
    HAL_ADC_PollForConversion(currentSense->adc, 20);
    if (HAL_IS_BIT_SET(HAL_ADC_GetState(currentSense->adc), HAL_ADC_STATE_REG_EOC))
    {

        // ADC_Value = HAL_ADC_GetValue(&hadc1); // 获取AD值
        // printf("ADC1 Reading : %d \r\n", ADC_Value);
        // printf("True Voltage value : %.4f \r\n", ADC_Value * 3.3f / 4096);
        adc_value = HAL_ADC_GetValue(currentSense->adc);
    }
    current = adc_value * 3.3 / (currentSense->adc_resolution * currentSense->R_sample * currentSense->gain) - currentSense->offset;
    return current;
}

float CurrentSense_get3Current(CurrentSense_t *currentSense)
{
    currentSense->adc = currentSense->adc1;
    currentSense->offset = currentSense->offset3[0];
    currentSense->i3[0] = CurrentSense_getCurrent(currentSense);
    currentSense->adc = currentSense->adc2;
    currentSense->offset = currentSense->offset3[1];
    currentSense->i3[1] = CurrentSense_getCurrent(currentSense);
    currentSense->adc = currentSense->adc3;
    currentSense->offset = currentSense->offset3[2];
    currentSense->i3[2] = CurrentSense_getCurrent(currentSense);
    return 0;
}

// float CurrentSense_getCurrentDQ(BLDCMotor_t *motor, CurrentSense_t *currentSense)
// {
//     LowPassFilter_t lpf_d;
//     LowPassFilter_t lpf_q;
//
//     LowPassFilter_init(&lpf_d, 1);
//     LowPassFilter_init(&lpf_q, 1);
//     float i_a, i_b, i_alpha, i_beta, i_q, i_d;
//
//     CurrentSense_get3Current(currentSense);
//     i_a = currentSense->i3[0];
//     i_b = currentSense->i3[1];
//     // i_a = LowPassFilter(&lpf_a, i_a);
//     // i_b = LowPassFilter(&lpf_b, i_b);
//     // 7.1 test angle
//     // float angle = _normalizeAngle(_electricalAngle(motor.angle, motor.foc_motor.pole_pairs));
//     // 7.2
//     // printf("angle = %f\n", motor.e_angle);
//     // delay_ms(20);
//     // printf("%f, %f\n", i_a, i_b);
//     //
//     // motor.angle = BLDCMotor_getAngle(&motor);
//     Clarke_Transform(i_a, i_b, &i_alpha, &i_beta);
//     Park_Transform(i_alpha, i_beta, motor->e_angle, &i_d, &i_q);
//     i_d = LowPassFilter(&lpf_d, i_d);
//     i_q = LowPassFilter(&lpf_q, i_q);
// }

void currentSense_3currentCalibration(CurrentSense_t *currentSense)
{

    if (currentSense->isCalibrating)
    {
        static uint32_t ia_sum = 0, ib_sum = 0, ic_sum = 0;
        const int sample_num = 1000;
        static  int sample_i = 0;
        if (sample_i < sample_num)
        {
            ia_sum += HAL_ADCEx_InjectedGetValue(currentSense->adc, ADC_INJECTED_RANK_1);
            ib_sum += HAL_ADCEx_InjectedGetValue(currentSense->adc, ADC_INJECTED_RANK_2);
            ic_sum += HAL_ADCEx_InjectedGetValue(currentSense->adc, ADC_INJECTED_RANK_3);
            // ia_sum += HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
            // ib_sum += HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);
            // ic_sum += HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3);
        }
        sample_i++;
        if (sample_i == sample_num)
        {
            currentSense->offset_a = (float)ia_sum / sample_num *3.3/(currentSense->R_sample*currentSense->gain*currentSense->adc_resolution);
            currentSense->offset_b = (float)ib_sum / sample_num *3.3/(currentSense->R_sample*currentSense->gain*currentSense->adc_resolution);
            currentSense->offset_c = (float)ic_sum / sample_num *3.3/(currentSense->R_sample*currentSense->gain*currentSense->adc_resolution);
            // currentSense->offset_a = (float)ia_sum / sample_num *3.3/(currentSense->R_sample*currentSense->gain*currentSense->adc_resolution);
            // currentSense->offset_b = (float)ib_sum / sample_num *3.3/(currentSense->R_sample*currentSense->gain*currentSense->adc_resolution);
            // currentSense->offset_c = (float)ic_sum / sample_num *3.3/(currentSense->R_sample*currentSense->gain*currentSense->adc_resolution);
            currentSense->isCalibrating = 0;
            return ;
        }
    }
    return ;
}
void currentSense_3current(CurrentSense_t *currentSense)
{
    const float scales = 3.3f /((float)currentSense->adc_resolution*currentSense->R_sample*currentSense->gain);

    currentSense->i_a = HAL_ADCEx_InjectedGetValue(currentSense->adc, ADC_INJECTED_RANK_1)*scales - currentSense->offset_a;
    currentSense->i_b = HAL_ADCEx_InjectedGetValue(currentSense->adc, ADC_INJECTED_RANK_2)*scales - currentSense->offset_b;
    currentSense->i_c = HAL_ADCEx_InjectedGetValue(currentSense->adc, ADC_INJECTED_RANK_3)*scales - currentSense->offset_c;

}
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
        currentSense_3currentCalibration(&currentSense);
        if (currentSense.adc_flag == 0)
        {
            currentSense_3current(&currentSense);
            currentSense.adc_flag = 1;
        }


        //2.
        // current_rawValue[0] = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
        // current_rawValue[1] = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);
        // current_rawValue[2] = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3);
        // adc_cmp_flag = 1;
        //3.


    }
}
