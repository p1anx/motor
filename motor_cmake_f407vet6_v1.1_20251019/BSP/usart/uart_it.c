#include "uart_it.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_uart.h"
#include "usart.h"
#include "usart_mx.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "esp8266.h"
extern uint8_t rxBuffer;
extern GlobalVar_t g_var;
char str[RXBUFFER];
uint8_t rx;
extern UART_HandleTypeDef huart1;
extern Motor_t motor;
extern PIDController_t pid;
extern ESP8266_HandleTypeDef g_esp8266;
char *sync_cmd = "SYNC_COMMAND";
int sync_flag = 0;
int t_ms = 4000;
float angle = 10.0f;
float qam4_list[4] = {180.0f, 0, 0, 0};
float qam16_list[16] = {9.8, 162.9, 192.6, 352.7,23.0, 151.0, 203.7, 338.7,35.7, 136.3, 217.8, 326.4,55.7, 118.1, 233.8, 308.8};

UART_t uart1 = {.huart = &huart1, .i = 0, .rxBuffer = &rxBuffer, .rxSize = RXBUFFER_SIZE};
char *getString(UART_t *uart)
{
    printf("rx =  %c\n", rx);
    uart->str[uart->i++] = *uart->rxBuffer;
    if (uart->rxBuffer == '\n')
    {
        printf("str = %s\n", uart->str);
        memset(uart->str, 0, uart->rxSize);
        uart->i = 0;
    }
    HAL_UART_Receive_IT(uart->huart, uart->rxBuffer, uart->rxSize);

    return uart->str;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {

        if (strcpyFromUART(str, rx) ==0 )
        {
            printf("[UART] received: %s\n", str);
        }
        HAL_UART_Receive_IT(&MY_UART_1, &rx, RXBUFFER_SIZE);
    }
    if (huart->Instance == g_esp8266.huart->Instance)
    {
        if (strcpyFromUART(g_esp8266.rxString, rx)==0)
        {
            // printf("[FROM ESP8266] %s\n", g_esp8266.rxString);
            float p_data;
            if ( getFloatFromCmd(g_esp8266.rxString, "p=", &p_data) == 0)
            {
                pid.Kp = p_data;
                printf("pdata = %f\n", pid.Kp);

            }
            float i_data;
            if ( getFloatFromCmd(g_esp8266.rxString, "i=", &i_data) == 0)
            {
                pid.Ki = i_data;
                printf("idata = %f\n",pid.Ki);

            }
            float m_enable;
            if ( getFloatFromCmd(g_esp8266.rxString, "m=", &m_enable) == 0)
            {
                motor.isEnable = (int)m_enable;
                printf("motor = %d\n",motor.isEnable);

            }
            float t_data;
            if ( getFloatFromCmd(g_esp8266.rxString, "t=", &t_data) == 0)
            {
                t_ms = (int)t_data;
                printf("t = %d\n",t_ms);

            }
            float angle_data;
            if ( getFloatFromCmd(g_esp8266.rxString, "a=", &angle_data) == 0)
            {
                angle = angle_data;
                printf("delta angle = %.2f\n", angle);

            }
            if (getFloatArrayFromCmd(g_esp8266.rxString, "q", 4, qam4_list) == 4)
            {
                for (int i = 0; i < 4; i++)
                {
                    printf("qam4_list[%d] = %f\n", i, qam4_list[i]);
                }
            }
            if (getFloatArrayFromCmd(g_esp8266.rxString, "k", 16, qam16_list) == 16)
            {
                for (int i = 0; i < 16; i++)
                {
                    printf("qam16_list[%d] = %f\n", i, qam16_list[i]);
                }
            }
            if (strcmp(g_esp8266.rxString, sync_cmd) == 0)
            {
                sync_flag = 1;

            }
        }
        HAL_UART_Receive_IT(g_esp8266.huart, &rx, RXBUFFER_SIZE);
    }
}
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     if (huart->Instance == USART1)
//     {
//         static int i = 0;
//         int result;
//         float f_result;
//         str[i++] = rx;
//         if (rx == '\n')
//         {
//             printf("str: %s\n", str);
//             // result = getInt_from_str(str, 2);
//             // if (str[0] == 'p')
//             // {
//             //     printf("p = %d\n", result);
//             // }
//             // else if (str[0] == 'i')
//             // {
//             //     printf("i = %d\n", result);
//             // }
//             f_result = getFloat_from_str(str, 2);
//             g_var.B_avg = f_result;
//             if (str[0] == 'p')
//             {
//                 printf("p = %f\n", f_result);
//             }
//             else if (str[0] == 'i')
//             {
//                 printf("i = %f\n", f_result);
//             }
//             g_var.pwm_duty = f_result;
//             // printf("result = %d\n", result);
//             i = 0;
//             memset(str, 0, RXBUFFER);
//         }
//         HAL_UART_Receive_IT(&MY_UART_1, &rx, RXBUFFER_SIZE);
//     }
// }

// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     if (huart->Instance == USART1)
//     {
//         static int i = 0;
//         printf("uart rx\n");
//         // PID_Controller* pid_init = motor1.inc_pid_vel_init;
//         str[i++] = rxBuffer;
//         printf("rx = %c\n", rxBuffer);
//         if (rxBuffer == '\n')
//         {
//
//             printf("str = %s", str);
//             memset(str, 0x00, RXBUFFER);
//             i = 0;
//         }
//
//         HAL_UART_Receive_IT(&MY_UART_1, &rxBuffer, 1);
//     }
// }
