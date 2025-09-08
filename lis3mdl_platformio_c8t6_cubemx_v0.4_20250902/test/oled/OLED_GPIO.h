#ifndef __OLED_GPIO_H
#define  __OLED_GPIO_H

// for stm32 hal
#include "stm32f1xx.h"

#define OLED_SCL_GPIO_PIN 			GPIO_PIN_8
#define OLED_SCL_GPIO_PORT 			GPIOB
#define OLED_SCL_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#define OLED_SDA_GPIO_PIN 			GPIO_PIN_9
#define OLED_SDA_GPIO_PORT 			GPIOB
#define OLED_SDA_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#define OLED_SCL_WritePin(BitValue) HAL_GPIO_WritePin(OLED_SCL_GPIO_PORT, OLED_SCL_GPIO_PIN, BitValue)
#define OLED_SDA_WritePin(BitValue) HAL_GPIO_WritePin(OLED_SDA_GPIO_PORT, OLED_SDA_GPIO_PIN, BitValue)

void OLED_HAL_GPIO_Init(void);
#endif