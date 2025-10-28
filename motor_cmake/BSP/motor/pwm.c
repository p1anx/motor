#include "pwm.h"
#include "stm32_hal.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_tim.h"
#include "system_stm32f4xx.h"
#include "types.h"
#include <stdio.h>

int pwm_init(PWM_t *pwm)
{
    // int arr;

    pwm->tim = &pwm_htim;
    pwm->channel = PWM_CHANNEL;
    pwm->resolution = PWM_RESOLUTION; // 12bit -> 4096
    pwm->frequency = PWM_FREQUENCY;   // Hz
    pwm->tim_psc = SystemCoreClock / (pwm->resolution * pwm->frequency);
    pwm->duty = 0.2;
    // HAL_TIM_Base_Start_IT(pwm->tim);
    if (HAL_TIM_PWM_Start(pwm->tim, pwm->channel) != HAL_OK)
    {
        printf("failed to start pwm\n");
        return 1;
    }
    else
    {

        printf("[ok] start pwm\n");
    }
    __HAL_TIM_SET_PRESCALER(pwm->tim, pwm->tim_psc - 1);
    __HAL_TIM_SET_AUTORELOAD(pwm->tim, pwm->resolution - 1);
    // arr = __HAL_TIM_GET_AUTORELOAD(pwm->tim);
    __HAL_TIM_SET_COMPARE(pwm->tim, pwm->channel, pwm->resolution * pwm->duty);
    printf("[ok] pwm init\n");
    return 0;
}

int pwm_calculatePSC(PWM_t *pwm)
{
    return SystemCoreClock / (pwm->resolution * pwm->frequency);
}

int pwm_setPWM(PWM_t *pwm)
{
    __HAL_TIM_SET_PRESCALER(pwm->tim, pwm->tim_psc - 1);
    __HAL_TIM_SetAutoreload(pwm->tim, pwm->resolution - 1);
    __HAL_TIM_SetCompare(pwm->tim, pwm->channel, pwm->duty * (pwm->resolution - 1));

    // DEBUG_PRINT("pwm duty = %f", pwm->duty);
    return 0;
}
int pwm_setDuty(PWM_t *pwm, float duty)
{
    __HAL_TIM_SET_PRESCALER(pwm->tim, pwm->tim_psc - 1);
    __HAL_TIM_SetAutoreload(pwm->tim, pwm->resolution - 1);
    __HAL_TIM_SetCompare(pwm->tim, pwm->channel, duty * (pwm->resolution - 1));

    // DEBUG_PRINT("pwm duty = %f", pwm->duty);
    return 0;
}
int pwm_setCompare(PWM_t *pwm, float compare)
{
    __HAL_TIM_SET_PRESCALER(pwm->tim, pwm->tim_psc - 1);
    __HAL_TIM_SetAutoreload(pwm->tim, pwm->resolution - 1);
    __HAL_TIM_SetCompare(pwm->tim, pwm->channel, compare - 1);

    // DEBUG_PRINT("pwm duty = %f", pwm->duty);
    return 0;
}
