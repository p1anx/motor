#include "pwm.h"
#include "types.h"

int pwm_init(PWM_t *pwm)
{

    HAL_TIM_PWM_Start(pwm->tim, pwm->channel);
    __HAL_TIM_SET_PRESCALER(pwm->tim, pwm->tim_psc - 1);
    __HAL_TIM_SET_AUTORELOAD(pwm->tim, pwm->resolution - 1);
    return 0;
}
