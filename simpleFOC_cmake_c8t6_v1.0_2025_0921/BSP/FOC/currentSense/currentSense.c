#include "currentSense.h"
#include "adc.h"
#include "stm32f1xx_hal_adc.h"
#include "stm32f1xx_hal_adc_ex.h"

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
    currentSense->gain = 6;
    currentSense->R_sample = 0.02;
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
