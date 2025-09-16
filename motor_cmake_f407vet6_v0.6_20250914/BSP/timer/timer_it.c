#include "encoder.h"
#inclde "motor.h"
#include "mymain.h"
#include <stdio.h>
#include "stm32_hal.h"
#include "stm32f4xx_hal.h"
#include "types.h"

extern Encoder_t encoder;
extern Motor_t motor;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* USER CODE BEGIN Callback 1 */
    if (htim->Instance == ENCODER1_TIM)
    {
        // printf("encoder timer1 is ok\n");
        encoder_getOverflowCount(&encoder);
    }
    else if (htim->Instance == ENCODER2_TIM)
    {

        // printf("encoder timer2 is ok\n");

        // DEBUG_PRINT("debug");
        encoder_getOverflowCount(&encoder);
    }
    else if (htim->Instance == PID_UPDATE_TIM)
    {
        printf("pid update tim is ok\n");
        static int last_t = 0;
        int t = HAL_GetTick();
        int delta_t = t - last_t;
        printf("delta = %d\n", delta_t);

        motor_pid(&motor);
        // pid();
        // gTime.time_flag++;
        //
        // if (g_tim_flag == 1) {
        //   tim_control_position();
        // }
    }
}
