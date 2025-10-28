#include "key.h"
#include "stm32f4xx_hal_gpio.h"
#include "types.h"
#include <stdio.h>
#include "motor.h"

int gKey_flag = 0;
int gKey_calibrate = 0;
extern Motor_t motor;

uint8_t Key_Scan(int key)
{
    switch (key)
    {
    case 1:
        if (HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
        {
            /*等待按键释放 */
            while (HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
            //     ;
            return KEY_ON;
        }
        else
            return KEY_OFF;
        /* code */
        break;
    case 2:
        /*检测是否有按键按下 */
        if (HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
        {
            /*等待按键释放 */
            while (HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
                ;
            return KEY_ON;
        }
        else
            return KEY_OFF;
    case 3:
        /*检测是否有按键按下 */
        if (HAL_GPIO_ReadPin(KEY3_GPIO_PORT, KEY3_GPIO_PIN) == KEY_ON)
        {
            /*等待按键释放 */
            while (HAL_GPIO_ReadPin(KEY3_GPIO_PORT, KEY3_GPIO_PIN) == KEY_ON)
                ;
            return KEY_ON;
        }
        else
            return KEY_OFF;

    default:
        return 1;
        break;
    }
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == KEY1_GPIO_PIN)
    {
        // motor.target -= 5;
        if (gKey_flag == 0)
        {
            gKey_flag = 1;
        }
        else
        {
            gKey_flag = 0;
        }
        printf("key 1 is pressed\n");
    }
    else if (GPIO_Pin == KEY2_GPIO_PIN) // can't be used for PA0
    {
        // gKey_calibrate = 1;
        motor.target += 1.1;

        // printf("key 2 is pressed\n");
    }
}
