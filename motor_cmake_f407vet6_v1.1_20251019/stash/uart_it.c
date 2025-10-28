#include "uart_it.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_uart.h"
#include "usart.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// extern char rxBuffer[RXBUFFER];
extern uint8_t rxBuffer;
// extern char *rxBuffer;
uint8_t str[RXBUFFER];
// extern uint8_t str[RXBUFFER];
extern Motor_InitStruct motor1;
float get_pid_value_from_uart(Motor_InitStruct *motor)
{
    char *pid_str;
    float pid_value;
    char *endptr;
    // PID_Controller* pid_init = motor->inc_pid_vel_init;
    PID_Controller *pid_init = motor->inc_pid_pos_init;
    pid_str = str + 2;
    if (str[0] == 'p' && str[1] == '=')
    {
        pid_value = strtof(pid_str, &endptr);
        pid_init->Kp = pid_value;
        printf("get p = %.2f\n", pid_value);
    }
    else if (str[0] == 'i' && str[1] == '=')
    {
        pid_value = strtof(pid_str, &endptr);
        pid_init->Ki = pid_value;
    }
    else if (str[0] == 'd' && str[1] == '=')
    {
        pid_value = strtof(pid_str, &endptr);
        pid_init->Kd = pid_value;
    }
    else
    {
        printf("plese input according to the format, p=[num] or i=[num] or d=[num]\n");
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        static int i = 0;
        // PID_Controller* pid_init = motor1.inc_pid_vel_init;
        PID_Controller *pid_init = motor1.inc_pid_pos_init;
        // printf("%c\n",rxBuffer);
        // if (rxBuffer == 'a') {
        //   printf("hello uart\n");
        // }
        str[i++] = rxBuffer;
        if (rxBuffer == '\n')
        {

            get_pid_value_from_uart(&motor1);
            printf("str = %s", str);
            // printf("p = %.2f, i = %.2f, d = %.2f\n", motor1.pid_vel_init->Kp, motor1.pid_vel_init->Ki,
            // motor1.pid_vel_init->Kd);
            // printf("p = %.2f, i = %.2f, d = %.2f\n", pid_init->Kp, pid_init->Ki, pid_init->Kd);
            // HAL_UART_Transmit(&MY_UART_1, str, RXBUFFER, HAL_MAX_DELAY);
            memset(str, 0x00, RXBUFFER);
            i = 0;
        }

        // static int i = 0;
        // while (*rxBuffer != '\0') {
        //     *(prxBuffer+i) = rxBuffer[RXBUFFER];
        //     i++;

        // }
        // printf("%s\n",prxBuffer);
        // memset(pRxBuffer, 0x00, RXBUFFER);
        HAL_UART_Receive_IT(&MY_UART_1, &rxBuffer, 1);
    }
}
