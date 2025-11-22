
#include "key.h"
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
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    /*if (GPIO_Pin == GPIO_PIN_4)
    {
    }*/
}
