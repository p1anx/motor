//
// Created by xwj on 10/22/25.
//

#include "uart_polling.h"
#include "usart.h"

#include <stdio.h>
#include <string.h>

// 轮询接收指定长度的数据
uint8_t uart_buffer[256];

void UART_Polling_Receive(void)
{
    uint8_t data;
    uint16_t index = 0;
    HAL_StatusTypeDef status;

    printf("waitting data...\r\n");

    while (1)
    {
        // 轮询接收一个字节，超时时间为100ms
        status = HAL_UART_Receive(&huart2, &data, 1, 100);

        if (status == HAL_OK)
        {
            // 接收到数据
            uart_buffer[index++] = data;

            // 如果接收到回车符，结束接收
            if (data == '\r' || data == '\n')
            {
                uart_buffer[index - 1] = '\0'; // 添加字符串结束符
                printf("recieved data: %s\r\n", uart_buffer);

                // 清空缓冲区，准备下次接收
                index = 0;
                memset(uart_buffer, 0, sizeof(uart_buffer));
            }

            // 防止缓冲区溢出
            if (index >= sizeof(uart_buffer) - 1)
            {
                index = 0;
                memset(uart_buffer, 0, sizeof(uart_buffer));
            }
        }
        else if (status == HAL_TIMEOUT)
        {
            // 超时，可以继续其他操作
            // printf("timeout\r\n");
            continue;
        }
        else
        {
            // 其他错误
            printf("UART ERROR\r\n");
            break;
        }
    }
}
void UART_Polling_Receive_Update(UART_HandleTypeDef *huart, char *rxData, int timeout)
{
    uint8_t buffer[1024];
    uint8_t data;
    uint16_t index = 0;
    HAL_StatusTypeDef status;
    int start_time = HAL_GetTick();

    printf("waitting data...\r\n");

    while (HAL_GetTick() - start_time < timeout)
    {
        // 轮询接收一个字节，超时时间为100ms
        status = HAL_UART_Receive(huart, &data, 1, 100);

        if (status == HAL_OK)
        {
            // 接收到数据
            buffer[index++] = data;
            // printf("%c", data);

            // 如果接收到回车符，结束接收
            if (data == '\r' || data == '\n')
            {
                buffer[index - 1] = '\0'; // 添加字符串结束符
                // printf("recieved data: %s\r\n", buffer);
                printf("%s\r\n", buffer);
                // memset(rxData, 0, strlen(rxData));
                strcpy(rxData, buffer);

                // 清空缓冲区，准备下次接收
                index = 0;
                memset(buffer, 0, sizeof(buffer));
            }

            // 防止缓冲区溢出
            if (index >= sizeof(buffer) - 1)
            {
                index = 0;
                memset(buffer, 0, sizeof(buffer));
            }
        }
        else if (status == HAL_TIMEOUT)
        {
            // 超时，可以继续其他操作
            // printf("timeout\r\n");
            continue;
        }
        else
        {
            // 其他错误
            printf("UART ERROR\r\n");
            break;
        }
    }
}
// uint8_t UART_Polling_Receive_Data(UART_HandleTypeDef *huart, char *expected, int timeout)
// {
//     uint8_t buffer[1024];
//     uint8_t data;
//     uint16_t index = 0;
//     HAL_StatusTypeDef status;
//     int start_time = HAL_GetTick();
//
//     printf("waitting data...\r\n");
//
//     while (HAL_GetTick() - start_time < timeout) {
//         // 轮询接收一个字节，超时时间为100ms
//         status = HAL_UART_Receive(huart, &data, 1, 100);
//
//         if (status == HAL_OK) {
//             // 接收到数据
//             buffer[index++] = data;
//             // printf("%c", data);
//
//             // 如果接收到回车符，结束接收
//             if (data == '\r' || data == '\n') {
//                 buffer[index-1] = '\0'; // 添加字符串结束符
//                 // printf("recieved data: %s\r\n", buffer);
//                 printf("%s\r\n", buffer);
//                 // memset(rxData, 0, strlen(rxData));
//                 if (strstr(buffer, expected) != NULL)
//                 {
//                     printf("matched\n");
//                     return 1;
//                 }
//
//                 // 清空缓冲区，准备下次接收
//                 index = 0;
//                 memset(buffer, 0, sizeof(buffer));
//             }
//
//             // 防止缓冲区溢出
//             if (index >= sizeof(buffer)-1) {
//                 index = 0;
//                 memset(buffer, 0, sizeof(buffer));
//             }
//         }
//         else if (status == HAL_TIMEOUT) {
//             // 超时，可以继续其他操作
//             // printf("timeout\r\n");
//             continue;
//         }
//         else {
//             // 其他错误
//             printf("UART ERROR\r\n");
//             break;
//         }
//     }
// }
