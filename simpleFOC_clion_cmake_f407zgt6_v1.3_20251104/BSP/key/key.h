#ifndef  __KEY_H
#define __KEY_H
#include "stm32f4xx_hal.h"

#define KEY_ON	1
#define KEY_OFF	0

#define KEY0_GPIO_Port GPIOE
#define KEY0_GPIO_Pin GPIO_PIN_0

#define KEY1_GPIO_Port GPIOE
#define KEY1_GPIO_Pin GPIO_PIN_1
uint8_t Key_Scan(int key_num);
// uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);
#endif
