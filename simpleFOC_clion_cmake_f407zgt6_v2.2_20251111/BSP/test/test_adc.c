//
// Created by xwj on 11/2/25.
//

#include "currentSense.h"
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