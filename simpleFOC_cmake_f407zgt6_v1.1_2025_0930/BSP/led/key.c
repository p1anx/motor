#include "BLDCMotor.h"
#include "led.h"
#include "stm32_hal.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal_gpio.h"

extern BLDCMotor_t motor;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_4)
    {
        if (motor.isEnable)
        {
            BLDCMotor_disable(&motor);
        }
        else
        {
            BLDCMotor_enable(&motor);
        }
        // HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_9);
        LED1_TOGGLE();
    }
}
