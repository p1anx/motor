//
// Created by xwj on 10/15/25.
//

#include "key.h"

#include <stdio.h>
int gKey_flag_0 = 0;
int gKey_flag_1 = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_0) {
        if (gKey_flag_0 == 0) {
            gKey_flag_0 = 1;
        }
        else if (gKey_flag_0 == 1) {
            gKey_flag_0 = 0;
        }
        // printf("Button 0 pressed\r\n");
        HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_9);
    }
    if (GPIO_Pin == GPIO_PIN_1) {
        if (gKey_flag_1 == 0) {
            gKey_flag_1 = 1;
        }
        else if (gKey_flag_1 == 1) {
            gKey_flag_1 = 0;
        }
        // printf("Button 1 pressed\r\n");
        HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_10);
    }
}
