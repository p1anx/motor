#include "currentSense.h"
// #include "BLDCMotor.h"
#include "lowpass_filter.h"
#include "stm32_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal_adc.h"

void test_adc(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1);
    uint16_t ADC_Value;
    static int i = 0;
    while (1)
    {
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 20);
        if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
        {
            ADC_Value = HAL_ADC_GetValue(&hadc1); // 获取AD值

            if (i > 1000)
            {
                printf("ADC1 Reading : %d \r\n", ADC_Value);
                printf("True Voltage value : %.4f \r\n", ADC_Value * 3.3f / 4096);
                i = 0;
            }
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
    // 1.
    //  CurrentSense_get3Offset(currentSense);
    //  2.
    CurrentSense_calibrate(currentSense);
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
        // printf("True Voltage value : %.4f \r\n", ADC_Value * 3.3f / 4096);
        adc_value = HAL_ADC_GetValue(pADC);
    }
    return adc_value;
}
void CurrentSense_calibrate(CurrentSense_t *pCurrentSense)
{
    float sum_a = 0, sum_b = 0, sum_c = 0;
    int samples = 1000;
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
    PRINT_INFO("current offset: a=%.2f, b=%.2f, c=%.2f\n", pCurrentSense->offset_a, pCurrentSense->offset_b, pCurrentSense->offset_c);
}

float CurrentSense_readCurrent(CurrentSense_t *pCurrentSense)
{
    float adc_rawValue = CurrentSense_readRawCurrent(pCurrentSense->adc);
    float adc_value = adc_rawValue * 3.3 / (4096 * pCurrentSense->R_sample * pCurrentSense->gain);
    return adc_value;
}
float CurrentSense_read3Current(CurrentSense_t *pCurrentSense)
{
    float scales = 3.3 / (4096 * pCurrentSense->R_sample * pCurrentSense->gain);
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
    current = adc_value * 3.3 / (4096 * currentSense->R_sample * currentSense->gain);
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
    v = adc_value * 3.3 / (4096);
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
    current = adc_value * 3.3 / (4096 * currentSense->R_sample * currentSense->gain) - currentSense->offset;
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
