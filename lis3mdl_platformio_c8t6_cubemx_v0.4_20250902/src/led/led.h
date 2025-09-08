#ifndef __LED_H
#define __LED_H
#include "stm32f1xx_hal.h"

#define LED1_GPIO_PIN  GPIO_PIN_13
#define LED1_GPIO_PORT GPIOC

#define LED1_TOGGLE() HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_GPIO_PIN)




#endif // ! __LED_H
