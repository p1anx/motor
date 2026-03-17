//
// Created by xwj on 1/14/26.
//

#include "pwm.h"

#include "tim.h"

#include <stdio.h>
extern TIM_HandleTypeDef htim1;
int PWM_Example(void) {
    PWM_t pwm;
    pwm.htim = &htim1;
    pwm.resolution = 4096;
    pwm.frequency = 500*4;
    pwm.pwm_type = _6PWM;
    // PWM3_Init(&pwm);
    PWM_Init(&pwm);
    int arr = pwm.htim->Instance->ARR;
    __HAL_TIM_SET_COMPARE(pwm.htim, TIM_CHANNEL_1, 0.3 * arr -1 - 0);
    __HAL_TIM_SET_COMPARE(pwm.htim, TIM_CHANNEL_2, 0.4 * arr -1 - 0);
    __HAL_TIM_SET_COMPARE(pwm.htim, TIM_CHANNEL_3, 0.5 * arr -1 - 0);
    return 0;

}

int PWM6_StartInit(PWM_t* pwm) {
    printf("[INFO] PWM TYPE is 6PWM\n");
    if (HAL_TIM_PWM_Start(pwm->htim, TIM_CHANNEL_1) != HAL_OK)
    {
        printf("[ERROR] PWM6_Init: TIM_CHANNEL_1 failed\n");
        return -1;
    }
    if (HAL_TIM_PWM_Start(pwm->htim, TIM_CHANNEL_2) != HAL_OK)
    {
        printf("[ERROR] PWM6_Init: TIM_CHANNEL_2 failed\n");
        return -1;
    }
    if (HAL_TIM_PWM_Start(pwm->htim, TIM_CHANNEL_3) != HAL_OK)
    {
        printf("[ERROR] PWM6_Init: TIM_CHANNEL_3 failed\n");
        return -1;
    }
    HAL_TIMEx_PWMN_Start(pwm->htim, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(pwm->htim, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(pwm->htim, TIM_CHANNEL_3);
    return 0;
}

int PWM3_StartInit(PWM_t* pwm) {
    printf("[INFO] PWM TYPE is 3PWM\n");
    if (HAL_TIM_PWM_Start(pwm->htim, TIM_CHANNEL_1) != HAL_OK)
    {
        printf("[ERROR] PWM3_Init: TIM_CHANNEL_1 failed\n");
        return -1;
    }
    if (HAL_TIM_PWM_Start(pwm->htim, TIM_CHANNEL_2) != HAL_OK)
    {
        printf("[ERROR] PWM3_Init: TIM_CHANNEL_2 failed\n");
        return -1;
    }
    if (HAL_TIM_PWM_Start(pwm->htim, TIM_CHANNEL_3) != HAL_OK)
    {
        printf("[ERROR] PWM3_Init: TIM_CHANNEL_3 failed\n");
        return -1;
    }
    return 0;
}

int PWM_Init(PWM_t* pwm) {

    if (!pwm->htim) {
        printf("[ERROR] PWM_Init: htim is NULL\n");
        return -1;
    }
    int pwm_tim_clk_2;
    int autoreload;
    int resolution = pwm->resolution;
    const int frequency = pwm->frequency;
    int default_resolution;
    int psc = 0;
    int arr = 0;
    if (pwm->pwm_type == _3PWM) {
        PWM3_StartInit(pwm);
    }
    else if (pwm->pwm_type == _6PWM) {
        PWM6_StartInit(pwm);
    }
    else {
        printf("[ERROR] Check the PWM Type\n");
    }

    int pwm_tim_clk;
    if (pwm->htim->Instance == TIM1 || pwm->htim->Instance == TIM8) {
        pwm_tim_clk = HAL_RCC_GetPCLK2Freq()*2;
    }
    else {
        pwm_tim_clk = HAL_RCC_GetPCLK2Freq();
    }

#if defined(STM32F407xx) || defined(STM32F405xx)
    autoreload = (int)(pwm_tim_clk / frequency);
#elif defined(STM32H743xx) || defined(STM32H750xx)
    autoreload = (int)(SystemCoreClock / 2 / frequency);
#endif

    pwm->autoreload = autoreload;
    if (pwm->htim->Init.CounterMode == TIM_COUNTERMODE_CENTERALIGNED1) {
        autoreload = autoreload / 2;
        pwm_tim_clk_2 = pwm_tim_clk / 2;
    }
    else {
        pwm_tim_clk_2 = pwm_tim_clk;
    }
    if (autoreload >= 65536) {
        __HAL_TIM_SET_PRESCALER(pwm->htim,  pwm_tim_clk_2 / (frequency * resolution) - 1);
        __HAL_TIM_SET_AUTORELOAD(pwm->htim,  resolution - 1);
        default_resolution = resolution;
    }
    else {
        __HAL_TIM_SET_AUTORELOAD(pwm->htim, autoreload - 1);
        default_resolution = autoreload;
    }
    __HAL_TIM_SET_COMPARE(pwm->htim, TIM_CHANNEL_1, default_resolution);
    __HAL_TIM_SET_COMPARE(pwm->htim, TIM_CHANNEL_2, default_resolution);
    __HAL_TIM_SET_COMPARE(pwm->htim, TIM_CHANNEL_3, default_resolution);
// #if CONFIG_CURRENT_SAMPLE_MODE
//     __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_1, default_resolution-1 - 0);
//     __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_2, default_resolution-1 - 0);
//     __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_3, default_resolution-1 - 0);
// #else
//     __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_1, 0);
//     __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_2, 0);
//     __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_3, 0);
// #endif

    HAL_TIM_PWM_Start(pwm->htim, TIM_CHANNEL_4);
    __HAL_TIM_SET_COMPARE(pwm->htim, TIM_CHANNEL_4, 1);

    if (pwm->htim->Instance == TIM1) {
        psc = TIM1->PSC;
        arr = TIM1->ARR;
    }
    else {
        printf("[INFO] check the tim\n");
    }
    printf("[INFO] PWM Freq = %d Hz, PSC = %d, ARR = %d(%d)\n", frequency, psc, arr, autoreload);
    printf("[INFO] PWM CLK = %d MHz\n", pwm_tim_clk / 1000000);
    return 0;
}

