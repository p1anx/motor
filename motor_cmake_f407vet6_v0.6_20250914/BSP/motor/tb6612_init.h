
#ifndef __TB6612_INIT_H
#define __TB6612_INIT_H
#include "pwm.h"
#include "stm32_hal.h"
#include "encoder.h"
#include "types.h"

struct TB6612_t
{

    GPIO_TypeDef *AIN1_GPIO_Port;
    uint16_t AIN1_GPIO_Pin;
    GPIO_TypeDef *AIN2_GPIO_Port;
    uint16_t AIN2_GPIO_Pin;
    GPIO_TypeDef *STBY_GPIO_Port;
    uint16_t STBY_GPIO_Pin;
    Encoder_t *encoder;
    PWM_t *pwm;
};

int tb6612_init(TB6612_t *tb6612);
#endif // !__MOTOR_H
//
