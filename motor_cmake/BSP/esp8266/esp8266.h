#ifndef __ESP8266_H
#define __ESP8266_H

#include "main.h"
#include <string.h>
#include <stdio.h>

// ESP8266配置
#define ESP8266_USART huart3
#define ESP8266_RST_GPIO_Port GPIOB
#define ESP8266_RST_Pin GPIO_PIN_11

// 超时定义
#define ESP8266_TIMEOUT 5000
#define ESP8266_SHORT_TIMEOUT 1000

// 返回值定义
typedef enum
{
    ESP8266_OK = 0,
    ESP8266_ERROR,
    ESP8266_TIMEOUT_ERROR,
    ESP8266_CONNECT_ERROR
} ESP8266_Status_t;

// ESP8266结构体
typedef struct
{
    UART_HandleTypeDef *huart;
    DMA_HandleTypeDef *hdma_usart_tx;
    char buffer[512];
    char ip[16];
    char mac[18];
    uint8_t connected;
    uint8_t rx_buffer;
    uint8_t rx_data;
    uint16_t rx_index;
    char rxString[1024];
} ESP8266_HandleTypeDef;

// 函数声明
ESP8266_Status_t ESP8266_Init(UART_HandleTypeDef *huart);
ESP8266_Status_t ESP8266_InitAP(UART_HandleTypeDef *huart);
ESP8266_Status_t ESP8266_Reset(void);
ESP8266_Status_t ESP8266_SendCommand(const char *cmd, const char *expected, uint32_t timeout);
// ESP8266_Status_t ESP8266_SendCmd(const char *cmd, const char *expected, uint32_t timeout);
ESP8266_Status_t ESP8266_SendCmd(UART_HandleTypeDef *huart, const char *cmd, const char *expected,
                                 uint32_t timeout);
ESP8266_Status_t ESP8266_SendCmd_NotResponse(UART_HandleTypeDef *huart, const char *cmd,
                                             uint32_t timeout);
ESP8266_Status_t ESP8266_SetMode(uint8_t mode);
ESP8266_Status_t ESP8266_ConnectWiFi(const char *ssid, const char *password);
ESP8266_Status_t ESP8266_StartTCP(const char *server_ip, uint16_t port);
ESP8266_Status_t ESP8266_SendData(const char *data);
ESP8266_Status_t ESP8266_CloseTCP(void);
ESP8266_Status_t ESP8266_GetLocalIP(void);
uint8_t ESP8266_IsConnected(void);
void printCmdInfo(void);
uint8_t UART_Polling_Receive_Data(UART_HandleTypeDef *huart, char *expected, int timeout);

void UART_Polling_Receive_Data_NotResponse(UART_HandleTypeDef *huart, int timeout);
// 中断接收回调函数
void ESP8266_UART_RxCpltCallback(UART_HandleTypeDef *huart);
ESP8266_Status_t ESP8266_Print(const char *data);
// void ESP8266_ClientInit(void);
void ESP8266_ClientInit(UART_HandleTypeDef *huart);
ESP8266_Status_t ESP8266_PrintStr(const char *data);
ESP8266_Status_t ESP8266_PrintFloat(const char string[], const float floatValue);
ESP8266_Status_t ESP8266_PrintInt(const char string[], const int intValue);
ESP8266_Status_t ESP8266_PrintDataFloat(int data_num, ...);
ESP8266_Status_t ESP8266_PrintDataInt(int data_num, ...);
void Example_ESP8266_0(void);

#endif