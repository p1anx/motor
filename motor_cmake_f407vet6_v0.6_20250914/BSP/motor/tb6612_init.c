#include "tb6612_init.h"
#include "encoder.h"
#include "pwm.h"
#include "stm32_hal.h"
#include "stm32f4xx_hal_def.h"
#include "types.h"
#include <stdio.h>

int tb6612_init(TB6612_t *tb6612)
{
    pwm_init(tb6612->pwm);
    encoder_init(tb6612->encoder);
    tb6612_enable(tb6612);

    tb6612_setDirection(tb6612, CLOCKWISE);

    return 0;
}

int tb6612_setDirection(TB6612_t *tb6612, int direction)
{
    switch (direction)
    {
    case CLOCKWISE:
        HAL_GPIO_WritePin(tb6612->IN1_GPIO_Port, tb6612->IN1_GPIO_Pin, SET);
        HAL_GPIO_WritePin(tb6612->IN2_GPIO_Port, tb6612->IN2_GPIO_Pin, RESET);
        // my_printf("counterclockwise\n");
        break;
    case NOT_CLOCKWISE:
        HAL_GPIO_WritePin(tb6612->IN1_GPIO_Port, tb6612->IN1_GPIO_Pin, RESET);
        HAL_GPIO_WritePin(tb6612->IN2_GPIO_Port, tb6612->IN2_GPIO_Pin, SET);
        // my_printf("clockwise\n");
        break;
    case 2:
        HAL_GPIO_WritePin(tb6612->IN1_GPIO_Port, tb6612->IN1_GPIO_Pin, RESET);
        HAL_GPIO_WritePin(tb6612->IN2_GPIO_Port, tb6612->IN2_GPIO_Pin, RESET);
        // my_printf("motor is stopped\n");
        break;
    default:
        HAL_GPIO_WritePin(tb6612->IN1_GPIO_Port, tb6612->IN1_GPIO_Pin, RESET);
        HAL_GPIO_WritePin(tb6612->IN2_GPIO_Port, tb6612->IN2_GPIO_Pin, RESET);
        break;
    }
    return 0;
}
int tb6612_enable(TB6612_t *tb6612)
{
    HAL_GPIO_WritePin(tb6612->STBYx_GPIO_Port, tb6612->STBYx_GPIO_Pin, SET);
    DEBUG_PRINT("[ok] tb6612 is enabled!");
    return 0;
}

int tb6612_disable(TB6612_t *tb6612)
{
    HAL_GPIO_WritePin(tb6612->STBYx_GPIO_Port, tb6612->STBYx_GPIO_Pin, RESET);
    DEBUG_PRINT("[ok] tb6612 is disabled!");
    return 0;
}
