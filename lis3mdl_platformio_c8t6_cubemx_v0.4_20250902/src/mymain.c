#include "mymain.h"

static void led_task(void *arg){
    while(1){
        LED1_TOGGLE();
        
        // my_printf("hello mx1\n");
        vTaskDelay(500);
    }
}

void task_lis3mdl(void *arg){
    LIS3MDL_Init();
    while(1){
        my_printf("hello mx1\n");
        LIS3MDL_DataRead();
        vTaskDelay(500);
    }

}
int mymain(void){

    // xTaskCreate(task_lis3mdl, "lis3mdl", 1024*10, NULL, 1, NULL);
    // xTaskCreate(led_task, "led", 128, NULL, 1, NULL);
    // vTaskStartScheduler();

    // while (1)
    // {
    //     // pwm_server_test(600);
        
    //     // pwm_led_brth(100);
        
    // }
    
    LIS3MDL_Init();
    while(1){
        // my_printf("hello mx1\n");
        // LED1_TOGGLE();
        // HAL_Delay(1000);
        LIS3MDL_DataRead();
        // HAL_Delay(500);
        // vTaskDelay(500);
    }

    return 0;
}