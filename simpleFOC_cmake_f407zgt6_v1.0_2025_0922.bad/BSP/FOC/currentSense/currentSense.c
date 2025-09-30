#include "currentSense.h"
#include "stm32f4xx.h"

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
    CurrentSense_get3Offset(currentSense);
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
