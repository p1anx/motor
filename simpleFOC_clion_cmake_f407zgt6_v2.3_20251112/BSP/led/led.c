#include "led.h"

void led_blink(int time_ms){
    LED1_TOGGLE();
    HAL_Delay(time_ms);
}