//
// Created by xwj on 11/6/25.
//

#include "my_adc.h"
#include "stm32_hal.h"
#include "adc.h"

#include <stdio.h>
extern ADC_HandleTypeDef hadc1;
int adc1_value, adc2_value, adc3_value;
int adc_cmp_flag = 0;
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
        // adc1_value = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1);
        adc_cmp_flag = 1;
        // HAL_ADCEx_InjectedStart_IT(&hadc1);
        // HAL_ADC_Start_IT(&hadc1);
        // HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc1_value, 1);


        // adc1_value = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1);
        // printf("v:%f\r\n",(float)adc1_value*3.3/4096);
        // float v = (float)adc1_value*3.3/4096;
        // UART_SendDataFloat(&v, 1);
        // HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc1_value, 1);


    }

}
