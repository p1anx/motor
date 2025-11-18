//
// Created by xwj on 11/2/25.
//

#include "currentSense.h"
#include "pwm.h"
#include "usart.h"

#include <stdio.h>

extern  CurrentSense_t currentSense;

void test_adc_00(void)
{
    CurrentSense_Init(&currentSense);
    HAL_Delay(1000);
    while(1)
    {
        // float v = CurrentSense_readCurrent(&currentSense);
        CurrentSense_read3Current(&currentSense);
        printf("v:%.2f, %.2f, %.2f\n", currentSense.i_a, currentSense.i_b, currentSense.i_c);
    }

}
void test_adc_01(void)
{
    CurrentSense_Init(&currentSense);
    HAL_Delay(1000);
    while(1)
    {
        // float v = CurrentSense_readCurrent(&currentSense);
        int t1 = HAL_GetTick();
        uint16_t rawValue = CurrentSense_getRawValue(&currentSense_adc1);
        // HAL_UART_Transmit(&huart2, (uint8_t *)(&rawValue), 2, 1000);
        printf("v:%d\n", rawValue);
        int t2 = HAL_GetTick();
        printf("t: %d\n", t2 - t1);
    }

}

void test_adc_02(void)
{
    CurrentSense_Init(&currentSense);
    const int data_len = 10000;
    float raw[data_len];
    HAL_Delay(1000);
    while(1)
    {
        printf("sampling\n");
        for(int i=0;i<data_len;i++)
        {
            float v = CurrentSense_readCurrent(&currentSense);
            raw[i] = v;

        }
        for(int i=0;i<data_len;i++)
        {
            printf("%f\n", raw[i]);
        }
    }

}
void test_adc_03(void)
{
    CurrentSense_Init(&currentSense);
    const int data_len = 10000;
    float raw[data_len];
    HAL_Delay(1000);
    while(1)
    {
        printf("sampling\n");
        for(int i=0;i<data_len;i++)
        {
            CurrentSense_read3Current(&currentSense);
            raw[i] = currentSense.i_a;

        }
        for(int i=0;i<data_len;i++)
        {
            printf("%f\n", raw[i]);
        }
    }

}
void test_adc_get_raw_voltage(void)
{
    CurrentSense_Init(&currentSense);
    const int data_len = 10000;
    float raw[data_len];
    HAL_Delay(1000);
    while(1)
    {
        printf("sampling\n");
        for(int i=0;i<data_len;i++)
        {
            float v = CurrentSense_getRawVoltage(&currentSense);
            raw[i] = v;
            printf("v:%f\n", raw[i]);

        }
        // for(int i=0;i<data_len;i++)
        // {
        // }
    }

}

extern float fTest_adc_buffer[100];

extern int adc_ok;
void test_adc_inject(void)
{
    int frequency = 1000;
    int resolution = 409;
    __HAL_TIM_SET_PRESCALER(&pwm_tim, SystemCoreClock / (frequency*2 * resolution) - 1);
    __HAL_TIM_SET_AUTORELOAD(&pwm_tim, resolution);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 1);
    __HAL_ADC_ENABLE_IT(&hadc1, ADC_IT_JEOC);
    if (HAL_ADCEx_InjectedStart_IT(&hadc1) != HAL_OK)
    {
        printf("[ERROR] FAILED TO START ADC\n");
        return;
    }
    printf("adc injected\n");
    int i = 0;
    while (1)
    {
        if (adc_ok)
        {
            // UART_SendFloat_DMA(1, fTest_adc_buffer[1]);
            printf("%f\n", fTest_adc_buffer[1]);
            // printf("%d\n", i++);
            //===============
            // float i0 = (float) i++;
            // UART_SendFloat_DMA(1, i0);
            // if (i ==1000)
            // {
            //     i = 0;
            // }
            //
            // float v = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1)*3.3/4096;
            // UART_SendFloat_DMA(1, fTest_adc_buffer[1]);
            adc_ok = 0;

        }

    }
}
void test_adc_0()
{
    //1.
    // test_adc();
    //2.
    // test_adc_00();
    //3.
    // test_adc_01();
    // test_adc_02();
    // test_adc_03();
    //4.
    test_adc_get_raw_voltage();
}