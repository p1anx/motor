#include "tim.h"
#include "types.h"
#include <stdio.h>
#include "encoder.h"
#include "pid.h"

int encoder_init(Encoder_t *encoder)
{
    encoder->htim = &encoder_htim;
    encoder->count_period = 65535;
    encoder->gear_ratio = MOTOR_REDUCTION_RATIO;
    encoder->multiple = 4;
    encoder->ppr = ENCODER_RESOLUTION;
    encoder->pulses = 0;
    encoder->overflow_count = 0;
    encoder->last_count = 0;
    // encoder->dt = PID_UPDATE_TIME;

    if (HAL_TIM_Encoder_Start(encoder->htim, TIM_CHANNEL_ALL) != HAL_OK)
    {
        printf("[error] failed to start encoder\n");
        return 1;
    }
    else
    {
        printf("[ok] started encoder\n");
    }
    if (HAL_TIM_Base_Start_IT(encoder->htim) != HAL_OK)
    {
        printf("[error] failed to start encoder base it\n");
        return 1;
    }
    else
    {

        printf("[ok] started encoder base it\n");
    }
    __HAL_TIM_ENABLE_IT(encoder->htim, TIM_IT_UPDATE);
    __HAL_TIM_SetAutoreload(encoder->htim, encoder->count_period);
    __HAL_TIM_SET_COUNTER(encoder->htim, 0);
    return 0;
}

int encoder_getOverflowCount(Encoder_t *encoder)
{

    if (__HAL_TIM_IS_TIM_COUNTING_DOWN(encoder->htim))
    {
        encoder->overflow_count--;
    }
    else
    {
        encoder->overflow_count++;
    }
    return encoder->overflow_count;
}

int encoder_getPulses(Encoder_t *encoder)
{
    encoder->pulses =
        __HAL_TIM_GET_COUNTER(encoder->htim) + (encoder->overflow_count) * (encoder->count_period);
    return encoder->pulses;
}

float encoder_getVelocity(Encoder_t *encoder)
{

    static float motor_totalCount, motor_lastCount = 0, delta_count;
    motor_totalCount = encoder_getPulses(encoder);
    delta_count = motor_totalCount - motor_lastCount;
    encoder->velocity = (float)((delta_count) / (4 * MOTOR_REDUCTION_RATIO * ENCODER_RESOLUTION *
                                                 encoder->pid->dt));
    motor_lastCount = motor_totalCount;
    return encoder->velocity;
}

float encoder_getPosition(Encoder_t *encoder)
{
    encoder->position = (float)encoder_getPulses(encoder);
    return encoder->position;
}

void Encoder_AngleUpdate(Encoder_t *enc)
{
    // 读取编码器当前值
    int32_t current_count = __HAL_TIM_GET_COUNTER(enc->htim);

    // 计算计数差值
    int32_t count_diff = current_count - enc->last_count;

    // 处理16位定时器溢出 (0-65535)
    if (count_diff > 32767)
    {
        count_diff -= 65536;
    }
    else if (count_diff < -32768)
    {
        count_diff += 65536;
    }

    enc->count += count_diff;
    enc->last_count = current_count;

    // 计算圈数
    enc->revolution_count = enc->count / enc->ppr;

    // 计算角度
    enc->current_angle = ((float)enc->count / enc->ppr) * 360.0f / enc->gear_ratio / enc->multiple;
}

float IncrementalEncoder_GetAngle(Encoder_t *enc)
{
    Encoder_AngleUpdate(enc);
    return enc->current_angle -
           (enc->zero_offset * 360.0f / (enc->ppr * enc->gear_ratio * enc->multiple));
}
float Encoder_GetAngle(Encoder_t *enc)
{
    return IncrementalEncoder_GetAngle(enc);
}
