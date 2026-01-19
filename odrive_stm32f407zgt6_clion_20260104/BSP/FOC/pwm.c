#include "stm32_hal.h"
#include "pwm.h"
#include "tim.h"

#include <stdio.h>
#include "motor_config.h"

void pwm_init(int frequency, int resolution)
{
    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&pwm_tim, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&pwm_tim, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&pwm_tim, TIM_CHANNEL_3);
    __HAL_TIM_SET_PRESCALER(&pwm_tim, SystemCoreClock / (frequency * resolution) - 1);
    __HAL_TIM_SET_AUTORELOAD(&pwm_tim, resolution - 1);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_1, 500);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_2, 400);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_3, 300);
}
int PWM3_Init(int frequency, int resolution)
{

    if (HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_1) != HAL_OK)
    {
        printf("PWM6_Init: TIM_CHANNEL_1 failed\n");
        return -1;
    }
    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_3);

    resolution = 4096;
    int default_resolution;
    int arr = SystemCoreClock / 2 /frequency;
    if (pwm_tim.Init.CounterMode == TIM_COUNTERMODE_CENTERALIGNED1) {
        arr = arr / 2;
    }
    if (arr > 65535) {
        __HAL_TIM_SET_PRESCALER(&pwm_tim, SystemCoreClock / (frequency * 4 * resolution) - 1);
        __HAL_TIM_SET_AUTORELOAD(&pwm_tim, resolution - 1);
        // __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_1, resolution-1 - 0);
        // __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_2, resolution-1 - 0);
        // __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_3, resolution-1 - 0);
        default_resolution = resolution;
    }
    else {
        __HAL_TIM_SET_AUTORELOAD(&pwm_tim, arr - 1);
        // __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_1, arr - 1 - 0);
        // __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_2, arr - 1 - 0);
        // __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_3, arr - 1 - 0);
        default_resolution = arr;
    }
#if CONFIG_CURRENT_SAMPLE_MODE
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_1, default_resolution-1 - 0);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_2, default_resolution-1 - 0);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_3, default_resolution-1 - 0);
#else
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_2, 0);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_3, 0);
#endif

    // __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_1, resolution*0.2);
    // __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_2, resolution*0.4);
    // __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_3, resolution*0.7);

    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_4);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_4, 1);
    printf("[OK] PWM Init\n");
}
// for 6 pwm
int PWM6_Init(int frequency, int resolution)
{
    if (HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_1) != HAL_OK)
    {
        printf("PWM6_Init: TIM_CHANNEL_1 failed\n");
        return -1;
    }
    if (HAL_TIMEx_PWMN_Start(&pwm_tim, TIM_CHANNEL_1) != HAL_OK)
    {
        printf("PWM6_Init: TIM_CHANNEL_2 failed\n");
        return -1;
    }
    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&pwm_tim, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&pwm_tim, TIM_CHANNEL_3);
    __HAL_TIM_SET_PRESCALER(&pwm_tim, SystemCoreClock / (frequency*2 * resolution) - 1);
    __HAL_TIM_SET_AUTORELOAD(&pwm_tim, resolution - 1);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_1, resolution - 0);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_2, resolution - 0);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_3, resolution - 0);

    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_4);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_4, 1);
    printf("[OK] PWM Init\n");
}
int test_PWM(int frequency, int resolution)
{
    if (HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_1) != HAL_OK)
    {
        printf("PWM6_Init: TIM_CHANNEL_1 failed\n");
        return -1;
    }
    if (HAL_TIMEx_PWMN_Start(&pwm_tim, TIM_CHANNEL_1) != HAL_OK)
    {
        printf("PWM6_Init: TIM_CHANNEL_2 failed\n");
        return -1;
    }
    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&pwm_tim, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&pwm_tim, TIM_CHANNEL_3);
    __HAL_TIM_SET_PRESCALER(&pwm_tim, SystemCoreClock / (frequency*2 * resolution) - 1);
    __HAL_TIM_SET_AUTORELOAD(&pwm_tim, resolution - 1);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_1, resolution *0.4);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_2, resolution *0.2);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_3, resolution *0.1);

    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_4);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_4, 1);
    printf("[OK] PWM Init\n");
}
int test_6PWM(void) {
    int frequency = 10e3;
    int resolution = 4096;
    float duty[3] = {0.2f, 0.4f, 0.5f};

    PWM6_Init(frequency, resolution);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_1, (1-duty[0]) * resolution - 1);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_2, (1-duty[1]) * resolution - 1);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_3, (1-duty[2]) * resolution - 1);

}
int test_PWM3_Init(int frequency, int resolution)
{
    if (HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_1) != HAL_OK)
    {
        printf("PWM6_Init: TIM_CHANNEL_1 failed\n");
        return -1;
    }
    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_3);
    __HAL_TIM_SET_PRESCALER(&pwm_tim, SystemCoreClock / (frequency * 4 * resolution) - 1);
    __HAL_TIM_SET_AUTORELOAD(&pwm_tim, resolution - 1);
    // __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_1, resolution - 0);
    // __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_2, resolution - 0);
    // __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_3, resolution - 0);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_1, resolution*0.2);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_2, resolution*0.4);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_3, resolution*0.7);

    printf("[OK] PWM Init\n");
}
