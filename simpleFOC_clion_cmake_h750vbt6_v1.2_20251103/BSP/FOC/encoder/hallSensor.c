#include "hallSensor.h"
#include <stdio.h>
#include "tim.h"

// HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)

// TIM_HandleTypeDef htim4; // 霍尔传感器定时器
// TIM_HandleTypeDef htim1; // PWM 生成定时器（用于电机控制）
void HallSensor_init(void)
{

    HAL_TIM_Base_Start_IT(&hall_tim);
    HAL_TIMEx_HallSensor_Start_IT(&hall_tim);
}

// 霍尔传感器状态
uint8_t hall_state = 0;
uint32_t hall_capture_val = 0, hall1, hall2, hall3;
/* 4. 霍尔传感器中断回调 */
// void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
// {
//     if (htim->Instance == TIM4)
//     {
//         // 1.
//         // 读取捕获值（用于计算速度）
//         hall_capture_val = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
//         // hall_capture_val = __HAL_TIM_GET_COUNTER(htim);
//         // 读取霍尔状态（GPIO 读取或通过寄存器）
//         hall_state = Read_Hall_State();
//         printf("hall val = %d\n", hall_capture_val);
//         printf("hall state = %d\n", hall_state);
//
//         // 2.
//         // hall1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
//         // printf("hall1 val = %d\n", hall1);
//
//         // 根据霍尔状态更新换相（6 步换相）
//         // Commutation_Update(hall_state);
//         float vel = Calculate_Speed();
//         printf("vel = %f rpm\n", vel);
//
//         // 重置计数器
//         __HAL_TIM_SET_COUNTER(htim, 0);
//     }
// }

/* 5. 读取霍尔传感器状态 */
uint8_t Read_Hall_State(void)
{
    uint8_t state = 0;

    // 假设霍尔传感器连接到 PA6, PA7, PB0
    if (HAL_GPIO_ReadPin(HALL1_GPIO_PORT, HALL1_GPIO_PIN))
        state |= 0x01; // H1
    if (HAL_GPIO_ReadPin(HALL2_GPIO_PORT, HALL2_GPIO_PIN))
        state |= 0x02; // H2
    if (HAL_GPIO_ReadPin(HALL3_GPIO_PORT, HALL3_GPIO_PIN))
        state |= 0x04; // H3

    return state;
}
/* 7. 计算速度（RPM）*/
uint32_t Calculate_Speed(void)
{
    // Speed (RPM) = (Timer_Clock / Capture_Value) * 60 / (Pole_Pairs * 6)
    // 例如：72MHz / (hall_capture_val * 极对数 * 6) * 60

    if (hall_capture_val > 0)
    {
        uint32_t speed_rpm = (84000000UL / hall_capture_val) * 60 / (2 * 6);
        return speed_rpm;
    }
    return 0;
}
/* ========== 带溢出处理和滤波的完整示例 ========== */

#define CAPTURE_BUFFER_SIZE 10
uint32_t capture_buffer[CAPTURE_BUFFER_SIZE];
uint8_t capture_index = 0;

uint32_t last_capture = 0;
uint32_t current_capture = 0;
uint32_t filtered_capture = 0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4)
    {
        // 1. 读取捕获值
        current_capture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

        // 2. 计算差值（处理溢出）
        uint32_t difference;
        if (current_capture >= last_capture)
        {
            difference = current_capture - last_capture;
        }
        else // 16位定时器溢出
        {
            difference = (0xFFFF - last_capture) + current_capture + 1;
        }

        // 3. 有效性检查（过滤异常值）
        if (difference > 100 && difference < 0xFFFF) // 合理范围
        {
            // 4. 存入滑动平均滤波缓冲区
            capture_buffer[capture_index] = difference;
            capture_index = (capture_index + 1) % CAPTURE_BUFFER_SIZE;

            // 5. 计算平均值（滤波）
            uint32_t sum = 0;
            for (int i = 0; i < CAPTURE_BUFFER_SIZE; i++)
            {
                sum += capture_buffer[i];
            }
            filtered_capture = sum / CAPTURE_BUFFER_SIZE;

            // 6. 计算速度
            if (filtered_capture > 0)
            {
                uint32_t rpm = (72000000UL / filtered_capture) * 60 / (2 * 6);
                printf("RPM: %lu (Raw: %lu, Filtered: %lu)\n", rpm, difference, filtered_capture);
            }
        }
        else
        {
            printf("Invalid capture: %lu (out of range)\n", difference);
        }

        // 7. 更新上次捕获值
        last_capture = current_capture;

        // 8. 可选：重置计数器（避免溢出）
        // __HAL_TIM_SET_COUNTER(htim, 0);
    }
}
