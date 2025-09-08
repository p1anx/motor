#include "main.h"
#include "mymain.h"
#include "key_it.h"

int i = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    key_it(GPIO_Pin);
}