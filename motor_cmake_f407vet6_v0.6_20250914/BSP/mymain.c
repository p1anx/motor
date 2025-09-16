#include "mymain.h"
// #include "stm32f4xx_hal.h"
// #include "stm32f4xx_hal_tim.h"
// #include "stm32f4xx_hal_uart.h"
// #include "tim.h"
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
    // HAL_UART_Receive_IT(&MY_UART_1, (uint8_t *)&rxBuffer, 1);

    // motor_main();

    test_main();
    return 0;
}
