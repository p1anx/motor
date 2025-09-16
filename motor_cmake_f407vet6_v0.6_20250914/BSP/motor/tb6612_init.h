
#ifndef __TB6612_INIT_H
#define __TB6612_INIT_H
#include "pwm.h"
#include "stm32_hal.h"
#include "encoder.h"
#include "types.h"

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

#define CLOCKWISE 0     // 0, 1
#define NOT_CLOCKWISE 1 // 0, 1

extern TIM_HandleTypeDef htim4;
#define pid_htim htim4
// #define PID_UPDATE_TIM TIM4

// encoder timer
extern TIM_HandleTypeDef htim8;
#define encoder_htim htim8
// #define ENCODER1_TIM TIM8

#define TB6612_PWM1_GPIO_PIN GPIO_PIN_9
#define TB6612_PWM1_GPIO_PORT GPIOE
extern TIM_HandleTypeDef htim1;
#define tb6612_pwm1_htim htim1
#define TB6612_PWM1_CHANNEL_x TIM_CHANNEL_1
#define TB6612_PWM1_PERIOD 1000

#define TB6612_AIN1_GPIO_PIN GPIO_PIN_8
#define TB6612_AIN1_GPIO_PORT GPIOB

#define TB6612_AIN2_GPIO_PIN GPIO_PIN_9
#define TB6612_AIN2_GPIO_PORT GPIOB

#define TB6612_STBY_GPIO_PIN GPIO_PIN_11
#define TB6612_STBY_GPIO_PORT GPIOB

#define TB6612_ENABLE() HAL_GPIO_WritePin(TB6612_STBY_GPIO_PORT, TB6612_STBY_GPIO_PIN, SET)
#define TB6612_DISABLE() HAL_GPIO_WritePin(TB6612_STBY_GPIO_PORT, TB6612_STBY_GPIO_PIN, RESET)
struct TB6612_t
{

    GPIO_TypeDef *IN1_GPIO_Port;
    uint16_t IN1_GPIO_Pin;
    GPIO_TypeDef *IN2_GPIO_Port;
    uint16_t IN2_GPIO_Pin;
    GPIO_TypeDef *STBYx_GPIO_Port;
    uint16_t STBYx_GPIO_Pin;
    Encoder_t *encoder;
    PWM_t *pwm;
    int direction;
};

int tb6612_init(TB6612_t *tb6612);

int tb6612_setDirection(TB6612_t *tb6612, int direction);
int tb6612_enable(TB6612_t *tb6612);
int tb6612_disable(TB6612_t *tb6612);
#endif // !__MOTOR_H
//
