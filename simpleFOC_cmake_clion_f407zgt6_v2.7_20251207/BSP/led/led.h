#ifndef __LED_H
#define __LED_H

#include  "stm32_hal.h"

#define LED1_GPIO_PIN GPIO_PIN_2
#define LED1_GPIO_PORT GPIOB

#define LED1_TOGGLE() HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_GPIO_PIN)

void led_blink(int time_ms);

#endif // ! __LED_H
