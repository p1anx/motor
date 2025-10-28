#include "encoder.h"
#include "motor.h"
#include <stdio.h>
#include "stm32_hal.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include "tb6612_init.h"
#include "types.h"

extern Encoder_t encoder;
extern Motor_t motor;
void test_pid(Motor_t *motor);


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
        // printf("pid update tim is ok\n");
        /*

        static int last_t = 0;
        int t = HAL_GetTick();
        int position = motor_getPosition(&motor);
        static int last_position = 0;
        int delta_t = t - last_t;
        int delta_position = position - last_position;
        float vel = (float)delta_position / delta_t * 1000;
        last_t = t;
        last_position = position;
        printf("vel = %f, position = %d, delta_t = %d\n", vel, delta_position, delta_t);

        */

        // printf("delta = %d ms\n", delta_t);
        // printf("arr = %d\n", __HAL_TIM_GET_AUTORELOAD(&pid_htim));

        // motor_pid(&motor);

        test_pid(&motor);
        // pid();
        // gTime.time_flag++;
        //
        // if (g_tim_flag == 1) {
        //   tim_control_position();
        // }
    }
}


void test_pid(Motor_t *motor)
{
    // motor_pid_velocity(motor, 2);
    motor_move(motor, motor->target);
}
