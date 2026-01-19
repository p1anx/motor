
#include "key.h"

#include <stdio.h>
#include "Motor.h"

// #include "BLDCMotor.h"
KEY_t key_enableMotor;

extern Motor_t motor;

uint8_t Key_Scan(int key_num)
{
    switch (key_num)
    {
        case 0:
            if(HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_GPIO_Pin) == KEY_ON )
            {
                while(HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_GPIO_Pin) == KEY_ON)
                {
                    return 	KEY_ON;
                }
            }
            else
                return KEY_OFF;
    case 1:
        if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_GPIO_Pin) == KEY_ON )
        {
            while(HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_GPIO_Pin) == KEY_ON)
            {
                return 	KEY_ON;
            }
        }
        else
            return KEY_OFF;
    default:
            return KEY_OFF;
    }
}
/*uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{
    if(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin) == KEY_ON )
    {
        while(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin) == KEY_ON);
        return 	KEY_ON;
    }
    else
        return KEY_OFF;
}*/
volatile int g_keyFlag = 0;
void KEY_Switch(void) {
    if (g_keyFlag == 0) {
        g_keyFlag = 1;
    }
    else if (g_keyFlag == 1) {
        g_keyFlag = 0;
    }

}
void KEY_Motor(void) {
    if (g_keyFlag == 0) {
        g_keyFlag = 1;
        // motor.isEnable = 1;

    }
    else if (g_keyFlag == 1) {
        g_keyFlag = 0;
        // motor.isEnable = 0;
    }

}
void KEY_enableMotor(KEY_t* key, uint16_t GPIO_Pin) {
    if (GPIO_Pin == key->GPIO_Pin) {
        // key->isPressed = 1;
        if (key->flag == 0) {
            key->flag= 1;
            Motor_enable(&motor);
        }
        else {
            key->flag= 0;
            Motor_disable(&motor);
        }
    }
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    // if (GPIO_Pin == KEY0_GPIO_Pin)
    // {
    //     KEY_Motor();
    // }
    KEY_enableMotor(&key_enableMotor, GPIO_Pin);
}
