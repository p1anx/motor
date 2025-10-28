//
// Created by xwj on 10/20/25.
//

#include "common.h"
#include <stdio.h>
#include "esp8266/esp8266.h"
#include "uart/uart.h"
#include "uart_dma.h"
#include "uart_polling.h"
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart2;
extern UART_t uart_printf;
extern UART_t uart_esp8266;
extern ESP8266_HandleTypeDef g_esp8266;
extern uint8_t rx0;

// void test_init_esp(void)
// {
//
//     // 初始化ESP8266
//     if (ESP8266_Init(&huart3) == ESP8266_OK)
//     {
//         // 连接WiFi
//         if (ESP8266_ConnectWiFi("BlackWind", "19950402") == ESP8266_OK)
//         {
//             // 获取本地IP
//             ESP8266_GetLocalIP();
//
//             // 启动TCP连接
//             if (ESP8266_StartTCP("192.168.1.11", 1123) == ESP8266_OK)
//             {
//                 // 发送数据
//                 ESP8266_SendData("Hello ESP8266!");
//
//                 // 关闭连接
//                 ESP8266_CloseTCP();
//             }
//         }
//     }
//
//     while (1)
//     {
//         HAL_Delay(1000);
//     }
// }
//
void test_0(void)
{
    uart_printf.huart = &huart2;
    uart_esp8266.huart = &huart3;
    UART_init(&uart_printf);
    UART_init(&uart_esp8266);
    char cmd_at_ok[] = "AT\r\n";
    char at_mode[] = "AT+CWMODE=1\r\n";
    char at_ip[] = "AT+CIFSR\r\n";
    char at_rst[] = "AT+RST\r\n";
    char at_cwjap[] = "AT+CWJAP=\"TCP\",\"192.168.1.11\",1123\r\n";
    char rxBuffer[128];
    char *cmd;
    cmd = at_ip;

    // ESP8266_UART_Init();
    printf("test esp66\n");
    // g_esp8266.huart = &huart3;
    // HAL_UART_Receive_IT(&huart3, &g_esp8266.rx_buffer, 1);
    // 1.
    if (ESP8266_Init(&ESP8266_USART) == ESP8266_OK)
    {

        printf("[OK] ESP8266 init\n");
        // 连接WiFi
        if (ESP8266_ConnectWiFi("BlackWind", "19950402") == ESP8266_OK)
        {
            // 获取本地IP
            delay_ms(5000);
            ESP8266_GetLocalIP();

            // 启动TCP连接
            if (ESP8266_StartTCP("192.168.1.11", 1123) == ESP8266_OK)
            {
                // 发送数据
                printf("start to send data \n");
                ESP8266_SendData("Hello ESP8266!");

                // 关闭连接
                // ESP8266_CloseTCP();
            }
        }
    }
    delay_ms(5000);
    // 2.
    //  ESP8266_InitAP(&huart3);
    //
    //  printf("start rx\n");
    while (1)
    {
        // delay_ms(10000);
        // printf("%s\n", g_esp8266.rxString);
        // 1.
        // if (HAL_UART_Transmit(&huart3, cmd, strlen(cmd), 1000) == HAL_OK)
        // {
        //     printf("[ok] uart tx\n");
        // }
        // delay_ms(2000);

        // 2.
        // if (ESP8266_SendCommand(cmd, "OK", 1000) == ESP8266_OK)
        // {
        //     printf("esp8266 send commond ok\n");
        // }
        // delay_ms(5000);
        // 3.
        //  HAL_UART_Transmit(&huart3, at_mode, strlen(at_mode), 1000);
        //  delay_ms(2000);
        //  HAL_UART_Transmit(&huart3, at_ip, strlen(at_ip), 1000);
        //  delay_ms(2000);
        //  HAL_UART_Transmit(&huart3, at_cwjap, strlen(at_cwjap), 1000);
        //  delay_ms(2000);
    }
}

extern uint8_t uart_buffer[256];
void test_1(void)
{
    printf("starting polling\n");
    char cmd_rst[] = "AT+RST\r\n";
    char cmd_linkWifi[] = "AT+CIPSTART=\"TCP\",\"192.168.1.11\",1123\r\n";
    char cmd_mode[] = "AT+CIPMODE=1\r\n";
    char cmd_send[] = "AT+CIPSEND\r\n";
    char cmd_exit[] = "+++\r\n";
    char rxData[1024];
    char msg[] = "Hello ESP8266!\n";
    char *cmd;
    cmd = cmd_linkWifi;
    HAL_StatusTypeDef status;
    int timeout = 6000;
    ESP8266_Reset();
    status = HAL_UART_Transmit(&huart3, (uint8_t *)cmd_exit, strlen(cmd_exit), 1000);
    status = HAL_UART_Transmit(&huart3, (uint8_t *)cmd_rst, strlen(cmd), 1000);
    if (status != HAL_OK)
    {
        printf("failed to transmit\n");
    }
    UART_Polling_Receive_Update(&huart3, rxData, timeout);
    delay_ms(5000);
    status = HAL_UART_Transmit(&huart3, (uint8_t *)cmd_linkWifi, strlen(cmd_linkWifi), 1000);
    if (status != HAL_OK)
    {
        printf("failed to transmit\n");
    }
    UART_Polling_Receive_Update(&huart3, rxData, timeout);
    status = HAL_UART_Transmit(&huart3, (uint8_t *)cmd_mode, strlen(cmd_mode), 1000);
    if (status != HAL_OK)
    {
        printf("failed to transmit\n");
    }
    UART_Polling_Receive_Update(&huart3, rxData, timeout);
    // status = HAL_UART_Transmit(&huart3, (uint8_t *)cmd_send, strlen(cmd_send), 1000);
    // if (status != HAL_OK)
    // {
    //     printf("failed to transmit\n");
    // }
    // UART_Polling_Receive_Update(&huart3, rxData, timeout);
    // status = HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), 1000);
    // if (status != HAL_OK)
    // {
    //     printf("failed to transmit\n");
    // }
    // UART_Polling_Receive_Update(&huart3, rxData, timeout);

    // while (1)
    // {
    //     HAL_UART_Transmit_DMA(&huart3, msg, strlen(msg));
    //     delay_ms(1000);
    // }
    // 2.
    //  printf("rxData: %s\n", rxData);
    //  UART_Polling_Receive();
    //  printf("uart recieved: %s\n", uart_buffer);
}
void ESP8266_InitType(void)
{
    printf("[INFO] Starting INIT ESP8266\n");
    char cmd_workMode[] = "AT+CWMODE=1\r\n"; // STA mode
    char cmd_rst[] = "AT+RST\r\n";
    char cmd_linkSever[] = "AT+CIPSTART=\"TCP\",\"192.168.1.11\",1123\r\n";
    char cmd_mux[] = "AT+CIPMUX=0\r\n"; // 0: single link path
    char cmd_sendMode[] = "AT+CIPMODE=0\r\n";
    char cmd_transparentModeOn[] = "AT+CIPMODE=1\r\n";
    char cmd_transparentModeOff[] = "AT+CIPMODE=0\r\n";
    char cmd_transparentOffSendCmd[] = "AT+CIPSEND=10\r\n";
    char cmd_transparentOnSendCmd[] = "AT+CIPSEND\r\n";
    char cmd_exit[] = "+++";
    char rxData[1024];
    char msg[] = "Hello ESP8266!\n";
    char *cmd;
    // cmd = cmd_linkWifi;
    HAL_StatusTypeDef status;
    HAL_StatusTypeDef hal_status;
    ESP8266_Status_t esp_status;
    UART_HandleTypeDef esp_uart = huart3;
    g_esp8266.huart = &esp_uart;

    int timeout = 5000;
    // 1.
    //  ESP8266_SendCmd(&esp_uart, cmd_rst, "CONNECTED", timeout);
    //  ESP8266_SendCmd(&esp_uart, cmd_linkSever, "CONNECT", timeout);
    //  ESP8266_SendCmd(&esp_uart, cmd_sendMode, "OK", timeout);
    //  ESP8266_SendCmd(&esp_uart, cmd_sendCmd, ">", timeout);
    //  HAL_UART_Transmit(&esp_uart, (uint8_t *)msg, strlen(msg), 1000);
    //  delay_ms(5000);

    // 2.
    ESP8266_SendCmd_NotResponse(&esp_uart, cmd_workMode, timeout);
    ESP8266_SendCmd_NotResponse(&esp_uart, cmd_rst, 10000);
    ESP8266_SendCmd_NotResponse(&esp_uart, cmd_mux, timeout);
    ESP8266_SendCmd_NotResponse(&esp_uart, cmd_linkSever, timeout);
    ESP8266_SendCmd_NotResponse(&esp_uart, cmd_transparentModeOn, timeout);
    ESP8266_SendCmd_NotResponse(&esp_uart, cmd_transparentOnSendCmd, timeout);
    // ESP8266_SendCmd_NotResponse(&esp_uart, cmd_transparentModeOff, timeout);
    // ESP8266_SendCmd_NotResponse(&esp_uart, cmd_transparentOffSendCmd, timeout);
    HAL_UART_Transmit(&esp_uart, (uint8_t *)msg, strlen(msg), 1000);
    while (1)
    {
        // ESP8266_SendCmd_NotResponse(&esp_uart, cmd_transparentOffSendCmd, timeout);
        HAL_UART_Transmit(&esp_uart, (uint8_t *)msg, strlen(msg), 1000);
        // ESP8266_Print("hello ESP\n");
        HAL_Delay(1000);
    }

    // ESP8266_Reset();
    // status = HAL_UART_Transmit(&esp_uart, (uint8_t *)cmd_exit, strlen(cmd_exit), 1000);
    // status = HAL_UART_Transmit(&esp_uart, (uint8_t *)cmd_rst, strlen(cmd), 1000);
    // if (status != HAL_OK)
    // {
    //     printf("failed to transmit\n");
    // }
    // UART_Polling_Receive_Data(&esp_uart, "CONNECTED", timeout);

    // ESP8266_SendCmd(&esp_uart, cmd_workMode, "OK", timeout);
    // status = HAL_UART_Transmit(&esp_uart, (uint8_t *)cmd_linkWifi, strlen(cmd_linkWifi), 1000);
    // if (status != HAL_OK)
    // {
    //     printf("failed to transmit\n");
    // }
    // UART_Polling_Receive_Update(&esp_uart, rxData, timeout);
    // status = HAL_UART_Transmit(&esp_uart, (uint8_t *)cmd_mode, strlen(cmd_mode), 1000);
    // if (status != HAL_OK)
    // {
    //     printf("failed to transmit\n");
    // }
    // UART_Polling_Receive_Update(&esp_uart, rxData, timeout);
    // status = HAL_UART_Transmit(&esp_uart, (uint8_t *)cmd_send, strlen(cmd_send), 1000);
    // if (status != HAL_OK)
    // {
    //     printf("failed to transmit\n");
    // }
    // UART_Polling_Receive_Update(&esp_uart, rxData, timeout);
    // status = HAL_UART_Transmit(&esp_uart, (uint8_t *)msg, strlen(msg), 1000);
    // if (status != HAL_OK)
    // {
    //     printf("failed to transmit\n");
    // }
    // UART_Polling_Receive_Update(&esp_uart, rxData, timeout);

    // while (1)
    // {
    //     HAL_UART_Transmit_DMA(&esp_uart, msg, strlen(msg));
    //     delay_ms(1000);
    // }
    // 2.
    //  printf("rxData: %s\n", rxData);
    //  UART_Polling_Receive();
    //  printf("uart recieved: %s\n", uart_buffer);
}
void test_esp_print(void)
{
    char str[] = "let's go\n";
    float floatV = 100.1f;
    int n = 10;
    ESP8266_ClientInit(&huart3);
    // g_esp8266.huart = &huart3;
    while (1)
    {
        ESP8266_PrintStr(str);
        // HAL_Delay(100);
        ESP8266_PrintInt("n:", n);
        ESP8266_PrintFloat("float=", floatV);
        ESP8266_PrintDataFloat(3, floatV, 2.1, 3.2);
        delay_ms(1);
    }
}
void test_esp8266()
{
    // 1.
    //  test_init_esp();
    // 2.
    // test_0();
    // 3.
    // test_1();
    // 4.
    // ESP8266_InitType();
    // 5.
    test_esp_print();
}