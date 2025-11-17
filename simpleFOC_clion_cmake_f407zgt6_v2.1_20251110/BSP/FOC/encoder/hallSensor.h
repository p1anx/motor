#ifndef __HALLSENSOR_H
#define __HALLSENSOR_H
#include "stm32_hal.h"

#define hall_tim htim4

#define HALL1_GPIO_PORT GPIOD
#define HALL1_GPIO_PIN GPIO_PIN_12
#define HALL2_GPIO_PORT GPIOD
#define HALL2_GPIO_PIN GPIO_PIN_13
#define HALL3_GPIO_PORT GPIOD
#define HALL3_GPIO_PIN GPIO_PIN_14

uint8_t Read_Hall_State(void);
uint32_t Calculate_Speed(void);
void HallSensor_init(void);
#endif // !__HALLSENSOR_H
