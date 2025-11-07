#include "BLDCMotor.h"
#include "encoder_as5600.h"
#include "foc_motor.h"
#include "tim.h"
#include "stm32_hal.h"

#include <stdio.h>
extern BLDCMotor_t motor;
// extern AS5600_t as5600;
void timer_init(TIM_HandleTypeDef *tim, int update_ms)
{
    int arr = 999;
    if (HAL_TIM_Base_Start_IT(tim) != HAL_OK)
    {
        printf("failed to start pid update timer\n");
    }
    __HAL_TIM_SET_PRESCALER(tim, 23999);
    __HAL_TIM_SET_AUTORELOAD(tim, update_ms*10 - 1);
}

void pid_update_timer(void)
{
    timer_init(&pid_tim, motor.pid_dt);
}

// // 获取定时器时钟频率（单位：Hz）
// uint32_t get_timer_clock_freq(TIM_HandleTypeDef *htim) {
//     uint32_t timer_clock;
//
//     // 判断定时器挂载在哪个总线
//     if (htim->Instance == TIM1 || htim->Instance == TIM8) {
//         // if (htim->Instance == TIM1 || htim->Instance == TIM8
//         //     || htim->Instance == TIM9 || htim->Instance == TIM10
//         //     || htim->Instance == TIM11) {
//             // APB2 总线上的定时器
//         timer_clock = HAL_RCC_GetPCLK2Freq();
//
//         // 检查 APB2 预分频器，如果分频系数不为1，则定时器时钟频率加倍
//         if ((RCC->CFGR & RCC_CFGR_PPRE2) != RCC_CFGR_PPRE2_DIV1) {
//             timer_clock *= 2;
//         }
//         } else {
//             // APB1 总线上的定时器
//             timer_clock = HAL_RCC_GetPCLK1Freq();
//
//             // 检查 APB1 预分频器，如果分频系数不为1，则定时器时钟频率加倍
//             if ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1) {
//                 timer_clock *= 2;
//             }
//         }
//
//     return timer_clock;
// }
uint32_t get_timer_clock_freq_simple(TIM_TypeDef *TIMx) {
    // 获取系统时钟频率
    uint32_t sysclk = HAL_RCC_GetSysClockFreq();

    // 简化的频率获取（适用于大多数情况）
    if (TIMx == TIM1 || TIMx == TIM8) {
        return HAL_RCC_GetPCLK2Freq() * 2;  // APB2 定时器通常时钟加倍
        } else {
            return HAL_RCC_GetPCLK1Freq() * 2;  // APB1 定时器通常时钟加倍
        }
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        // motor.angle = BLDCMotor_getAngle(&motor);
        // motor.angle = FOCMotor_shaftAngle(&motor.foc_motor);
        // motor.velocity = BLDCMotor_getVelocity(&motor);
        // motor.angle = AS5600_ReadAngleRadians(&as5600);
        // printf("angle = %f\n", motor.angle);

        //1.
        // static int first_run = 1;
        // if (first_run) {
        //     printf("First PID run: angle=%.2f, velocity=%.2f\n",
        //            motor.angle, motor.velocity);
        //     first_run = 0;
        // }
        // BLDCMotor_run(&motor);
        // AS5600_GetAngularSpeed(&as5600, , bool update)
        //2.
        // static  int i = 0;
        // if (i == 10)
        // {
        //     int apb1 = get_timer_clock_freq_simple(TIM3);
        //     int apb2 = HAL_RCC_GetPCLK2Freq();
        //     printf("hello timer, %d, %d\n", apb1, apb2);
        //     i=0;
        //
        // }
        // i++;
    }
}
