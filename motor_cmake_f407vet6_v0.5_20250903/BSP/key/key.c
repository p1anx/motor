#include "key.h"

uint8_t Key_Scan(int key)
{			
    switch (key)
    {
    case 1:
        if(HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON )  
        {	 
            /*等待按键释放 */
            while(HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON);   
            return 	KEY_ON;	 
        }
        else
            return KEY_OFF;
        /* code */
        break;
    case 2:
        /*检测是否有按键按下 */
        if(HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON )  
        {	 
            /*等待按键释放 */
            while(HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON);   
            return 	KEY_ON;	 
        }
        else
            return KEY_OFF;
    case 3:
        /*检测是否有按键按下 */
        if(HAL_GPIO_ReadPin(KEY3_GPIO_PORT, KEY3_GPIO_PIN) == KEY_ON )  
        {	 
            /*等待按键释放 */
            while(HAL_GPIO_ReadPin(KEY3_GPIO_PORT, KEY3_GPIO_PIN) == KEY_ON);   
            return 	KEY_ON;	 
        }
        else
            return KEY_OFF;
    
    default:
        return 1;
        break;
    }
}

