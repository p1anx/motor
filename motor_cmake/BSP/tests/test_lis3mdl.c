//
// Created by xwj on 10/15/25.
//

#include "lis3mdl.h"
#include "uart.h"

extern LIS3MDL_t lis3mdl;
extern UART_t uart_printf;
extern uint8_t rxBuffer;
void test_lis3mdl_0(void)
{
    LIS3MDL_Init();
    while (1)
    {
        LIS3MDL_DataRead();
    }
}

extern int RxCplt_flag;
extern char str[1024];
extern int sync_flag;

void test_lis3mdl_update(void)
{
    UART_PrintfInit(&uart_printf);
    LIS3MDL_InitStruct(&lis3mdl);
    int isFirst = 1;
    char *start_flag = "start";
    // HAL_Delay(10000);
    while (1)
    {
        // if (isFirst)
        // {
        //     isFirst = 0;
        //     printf("%s\n", start_flag);
        // }
        if (sync_flag == 1)
        {

            LIS3MDL_DataUpdate(&lis3mdl);
            printf("%.3f,%.3f,%.3f\n", lis3mdl.data_x, lis3mdl.data_y, lis3mdl.data_z);
            // HAL_Delay(10);
        }
    }
}

void test_lis3mdl_measure_speed(void)
{
    UART_PrintfInit(&uart_printf);
    LIS3MDL_InitStruct(&lis3mdl);
    int isFirst = 1;
    char *start_flag = "start";
    float data[3];
    // HAL_Delay(10000);
    while (1)
    {
        // if (isFirst)
        // {
        //     isFirst = 0;
        //     printf("%s\n", start_flag);
        // }

        // int current_t = HAL_GetTick();
        LIS3MDL_DataUpdate(&lis3mdl);
        // 1.
        printf("%.3f,%.3f,%.3f\n", lis3mdl.data_x, lis3mdl.data_y, lis3mdl.data_z);
        // 2.
        //  data[0] = lis3mdl.data_x;
        //  data[1] = lis3mdl.data_y;
        //  data[2] = lis3mdl.data_z;
        //  HAL_UART_Transmit(&huart1, data, 3, 1000);
        //  int delta_t = HAL_GetTick() - current_t;
        //  printf("t = %d\n", delta_t);
    }
}
void test_lis3mdl(void)
{

    // 1.
    // test_lis3mdl_update();
    // 2.
    test_lis3mdl_measure_speed();
}
