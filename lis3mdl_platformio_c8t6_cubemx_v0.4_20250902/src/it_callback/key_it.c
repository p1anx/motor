#include "key_it.h"
#include "mymain.h"

void key_it(uint16_t GPIO_pin)
{
    if(GPIO_pin == KEY1_GPIO_PIN){
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
    }
}