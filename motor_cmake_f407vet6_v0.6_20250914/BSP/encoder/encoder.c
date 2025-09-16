#include "tim.h"
#include "types.h"
#include <stdio.h>
#include "encoder.h"
#include "pid.h"
int encoder_init(Encoder_t *encoder)
{

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
    //(float) is important for solution of
    // negative value
    // if (__HAL_TIM_IS_TIM_COUNTING_DOWN(encoder->htim))
    // {
    //     encoder->pulses = (float)(encoder->count_period - __HAL_TIM_GET_COUNTER(encoder->htim)) +
    //                       (encoder->overflow_count) * (encoder->count_period);
    // }
    // else
    // {
    //     encoder->pulses =
    //         -(float)__HAL_TIM_GET_COUNTER(encoder->htim) +
    //         (encoder->overflow_count + 1) *
    //             (encoder
    //                  ->count_period);
    //                  //(g_overflow_encoder_count+1)补偿启动时，由于--的起始值为-1,加1做补偿
    // }
    // if (__HAL_TIM_IS_TIM_COUNTING_DOWN(encoder->htim))
    // {
    //     DEBUG_PRINT("forward");
    //     encoder->pulses = __HAL_TIM_GET_COUNTER(encoder->htim) +
    //                       (encoder->overflow_count) * (encoder->count_period);
    // }
    // else
    // {
    //     DEBUG_PRINT("backward");
    //     encoder->pulses =
    //         __HAL_TIM_GET_COUNTER(encoder->htim) +
    //         (encoder->overflow_count + 0) *
    //             (encoder
    //                  ->count_period);
    //                  //(g_overflow_encoder_count+1)补偿启动时，由于--的起始值为-1,加1做补偿
    // }
    encoder->pulses =
        __HAL_TIM_GET_COUNTER(encoder->htim) + (encoder->overflow_count) * (encoder->count_period);
    return encoder->pulses;
}

float encoder_getVelocity(Encoder_t *encoder)
{

    static float motor_totalCount, motor_lastCount = 0, motor_speed, delta_count;
    motor_totalCount = encoder_getPulses(encoder);
    delta_count = motor_totalCount - motor_lastCount;
    encoder->velocity =
        (float)((delta_count) / (4 * MOTOR_REDUCTION_RATIO * ENCODER_RESOLUTION * PID_UPDATE_TIME));
    motor_lastCount = motor_totalCount;
    return encoder->velocity;
}
