#ifndef __ENCODER1_H
#define __ENCODER1_H
#include "stm32_hal.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal_tim.h"
#include "types.h"

#define ENCODER1_TIM TIM3
#define ENCODER2_TIM TIM8

#define ENCODER_RESOLUTION 500
#define MOTOR_REDUCTION_RATIO 10

// encoder timer
extern TIM_HandleTypeDef htim3;
#define encoder_htim htim3
// #define ENCODER1_TIM TIM8

// typedef struct Encoder_t Encoder_t;

struct Encoder_t
{
    int overflow_count;
    int direction;
    TIM_HandleTypeDef *htim;
    int count_period;
    int pulses;
    float velocity;
    float position;
    float dt;
    float *pdt;
    PIDController_t *pid;

    int32_t count;
    int32_t last_count;
    int32_t zero_offset;
    int32_t ppr; // 编码器线数 × 4 (AB相正交)
    float gear_ratio;
    float current_angle;
    int32_t revolution_count; // 圈数计数
    int multiple;
};

int encoder_init(Encoder_t *encoder);
int encoder_getOverflowCount(Encoder_t *encoder);
int encoder_getPulses(Encoder_t *encoder);
float encoder_getVelocity(Encoder_t *encoder);
float encoder_getPosition(Encoder_t *encoder);

void Encoder_AngleUpdate(Encoder_t *enc);
float IncrementalEncoder_GetAngle(Encoder_t *enc);
float Encoder_GetAngle(Encoder_t *enc);
#endif // !__ENCODER1_H
