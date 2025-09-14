#include "mymain.h"
#include "motor.h"
#include "motor_config.h"
#include "pid_init.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_uart.h"
#include "tim.h"
#include "uart_it.h"
#include <stdint.h>

void test(void)
{
    static int last_time = 0;
    while (1)
    {
        int current_time = HAL_GetTick();
        int delta_time = current_time - last_time;
        last_time = current_time;
        printf("time = %d\n", delta_time);
        HAL_Delay(1000);
        // if(Key_Scan(1) ==KEY_ON){
        //     LED1_TOGGLE();
        // }
        // if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8)
        // == GPIO_PIN_RESET){
        //     // LED1_TOGGLE();
        //     LED1_ON();
        // }
        // else{
        //     LED1_OFF();
        // }
        // printf("hello motor\n");
        // HAL_Delay(1000);

        // HAL_Delay(100)
        // HAL_Delay(100);
    }
}

// char rxBuffer[RXBUFFER];
uint8_t rxBuffer;
// char *rxBuffer;
int mymain(void)
{
    // motor_run();
    // test();
    // time_test();
    HAL_UART_Receive_IT(&MY_UART_1, (uint8_t *)&rxBuffer, 1);
    // HAL_UART_Receive_IT(&MY_UART_1, (uint8_t
    // *)pRxBuffer, RXBUFFER);

    // HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    // __HAL_TIM_SET_PRESCALER(&htim1, PWM_TIM_PSC
    // - 1);
    // __HAL_TIM_SET_AUTORELOAD(&htim1,
    // PWM_RESOLUTION - 1);
    motor_main();
    return 0;
}
