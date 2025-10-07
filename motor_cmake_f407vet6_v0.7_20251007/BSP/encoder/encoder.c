#include "types.h"
#include "encoder.h"

int encoder_init(Encoder_t *encoder)
{

    HAL_TIM_Encoder_Start(encoder->htim, TIM_CHANNEL_ALL);
    HAL_TIM_Base_Start_IT(encoder->htim);

    __HAL_TIM_ENABLE_IT(encoder->htim, TIM_IT_UPDATE);
    __HAL_TIM_SetAutoreload(encoder->htim, encoder->count_period);
    __HAL_TIM_SET_COUNTER(encoder->htim, 0);
    return 0;
}
