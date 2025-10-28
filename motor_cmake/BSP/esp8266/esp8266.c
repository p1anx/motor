#include "esp8266.h"

#include <stdarg.h>
#include "common.h"

ESP8266_HandleTypeDef g_esp8266;
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_tx;

// 内部函数声明
static void ESP8266_Delay(uint32_t ms);
static uint8_t ESP8266_ReceiveResponse(char *response, uint32_t timeout);
static uint8_t ESP8266_WaitForResponse(const char *expected, uint32_t timeout);

/**
 * @brief 初始化ESP8266
 * @param huart: UART句柄
 * @retval ESP8266状态
 */
void printCmdInfo(void)
{

    printf("%s\n", g_esp8266.rxString);
}

void ESP8266_ClientInit(UART_HandleTypeDef *huart)
{
    printf("[INFO] Starting INIT ESP8266 Client\n");
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
    char msg[] = "Hello ESP8266! --> Client is Ready!\r\n";
    char *cmd;
    // cmd = cmd_linkWifi;
    HAL_StatusTypeDef status;
    HAL_StatusTypeDef hal_status;
    ESP8266_Status_t esp_status;
    UART_HandleTypeDef *esp_uart = huart;
    g_esp8266.huart = huart;

    int timeout = 5000;
    int long_timeout = 10000;
    ESP8266_SendCmd_NotResponse(esp_uart, cmd_exit, timeout);
    ESP8266_SendCmd_NotResponse(esp_uart, cmd_workMode, timeout);
    ESP8266_SendCmd_NotResponse(esp_uart, cmd_rst, long_timeout);
    ESP8266_SendCmd_NotResponse(esp_uart, cmd_mux, timeout);
    ESP8266_SendCmd(esp_uart, cmd_linkSever, "CONNECT", long_timeout);
    // ESP8266_SendCmd_NotResponse(esp_uart, cmd_linkSever, long_timeout);
    ESP8266_SendCmd_NotResponse(esp_uart, cmd_transparentModeOn, timeout);
    ESP8266_SendCmd_NotResponse(esp_uart, cmd_transparentOnSendCmd, timeout);
    // ESP8266_SendCmd_NotResponse(&esp_uart, cmd_transparentModeOff, timeout);
    // ESP8266_SendCmd_NotResponse(&esp_uart, cmd_transparentOffSendCmd, timeout);
    HAL_UART_Transmit(esp_uart, (uint8_t *)msg, strlen(msg), 1000);
    // while (1)
    // {
    //     // ESP8266_SendCmd_NotResponse(&esp_uart, cmd_transparentOffSendCmd, timeout);
    //     HAL_UART_Transmit(&esp_uart, (uint8_t *)msg, strlen(msg), 1000);
    //     // ESP8266_Print("hello ESP\n");
    //     HAL_Delay(1000);
    // }
}
ESP8266_Status_t ESP8266_InitAP(UART_HandleTypeDef *huart)
{
    g_esp8266.huart = huart;
    g_esp8266.connected = 0;
    char cmd_mode[] = "AT+CWMODE=1\r\n";
    char cmd_rst[] = "AT+RST\r\n";
    char cmd_getIp[] = "AT+CIFSR\r\n";
    char cmd_mux[] = "AT+CIPMUX=1\r\n";
    char cmd_send[] = "AT+CIPMODE=0\r\n";
    char cmd_server[] = "AT+CIPSERVER=1,8080\r\n";

    // 启动UART中断接收
    // HAL_UART_Receive_IT(g_esp8266.huart, &g_esp8266.rx_buffer, 1);

    // 重置ESP8266
    // ESP8266_Reset();
    if (ESP8266_SendCommand("AT\r\n", "OK", ESP8266_SHORT_TIMEOUT) != ESP8266_OK)
    {
        printf("[ERROR] AT\n");
        return ESP8266_ERROR;
    }
    if (ESP8266_SendCommand(cmd_mode, "OK", ESP8266_SHORT_TIMEOUT) != ESP8266_OK)
    {
        printf("[ERROR] CWMODE\n");
        return ESP8266_ERROR;
    }
    HAL_Delay(1000);
    if (ESP8266_SendCommand(cmd_rst, "CONNECTED", ESP8266_SHORT_TIMEOUT) != ESP8266_OK)
    {
        printf("[ERROR] RST\n");
        return ESP8266_ERROR;
    }
    HAL_Delay(5000);

    // 测试AT命令
    if (ESP8266_SendCommand(cmd_getIp, "OK", ESP8266_SHORT_TIMEOUT) != ESP8266_OK)
    {
        printf("[ERROR] GET_IP\n");
        return ESP8266_ERROR;
    }
    HAL_Delay(100);
    if (ESP8266_SendCommand(cmd_mux, "OK", ESP8266_SHORT_TIMEOUT) != ESP8266_OK)
    {
        printf("[ERROR] MUX\n");
        return ESP8266_ERROR;
    }
    if (ESP8266_SendCommand(cmd_send, "OK", ESP8266_SHORT_TIMEOUT) != ESP8266_OK)
    {
        printf("[ERROR] SEND\n");
        return ESP8266_ERROR;
    }
    if (ESP8266_SendCommand(cmd_server, "OK", ESP8266_SHORT_TIMEOUT) != ESP8266_OK)
    {
        printf("[ERROR] SERVER\n");
        return ESP8266_ERROR;
    }
    return ESP8266_OK;
}
ESP8266_Status_t ESP8266_Init(UART_HandleTypeDef *huart)
{
    g_esp8266.huart = huart;
    g_esp8266.connected = 0;
    char cmd_mode[] = "AT+CWMODE=1\r\n";
    char cmd_rst[] = "AT+RST\r\n";
    char cmd_getIp[] = "AT+CIFSR\r\n";
    char cmd_mux[] = "AT+CIPMUX=0\r\n";
    char cmd_send[] = "AT+CIPMODE=0\r\n";

    // 启动UART中断接收
    // HAL_UART_Receive_IT(g_esp8266.huart, &g_esp8266.rx_buffer, 1);

    // 重置ESP8266
    // ESP8266_Reset();
    if (ESP8266_SendCommand("+++", "OK", ESP8266_SHORT_TIMEOUT) != ESP8266_OK)
    {
        printf("[ERROR] AT\n");
        return ESP8266_ERROR;
    }
    if (ESP8266_SendCommand(cmd_mode, "OK", ESP8266_SHORT_TIMEOUT) != ESP8266_OK)
    {
        printf("[ERROR] CWMODE\n");
        return ESP8266_ERROR;
    }
    HAL_Delay(100);
    ESP8266_Reset();
    // if (ESP8266_SendCommand(cmd_rst, "CONNECTED", ESP8266_SHORT_TIMEOUT) != ESP8266_OK)
    // {
    //     printf("[ERROR] RST\n");
    //     return ESP8266_ERROR;
    // }
    // HAL_Delay(5000);

    // 测试AT命令
    if (ESP8266_SendCommand(cmd_getIp, "OK", ESP8266_SHORT_TIMEOUT) != ESP8266_OK)
    {
        printf("[ERROR] GET_IP\n");
        return ESP8266_ERROR;
    }
    HAL_Delay(100);
    if (ESP8266_SendCommand(cmd_mux, "OK", ESP8266_SHORT_TIMEOUT) != ESP8266_OK)
    {
        printf("[ERROR] MUX\n");
        return ESP8266_ERROR;
    }
    if (ESP8266_SendCommand(cmd_send, "OK", ESP8266_SHORT_TIMEOUT) != ESP8266_OK)
    {
        printf("[ERROR] SEND\n");
        return ESP8266_ERROR;
    }
    return ESP8266_OK;
}
ESP8266_Status_t ESP8266_Init_v0(UART_HandleTypeDef *huart)
{
    g_esp8266.huart = huart;
    g_esp8266.connected = 0;

    // 启动UART中断接收
    HAL_UART_Receive_IT(g_esp8266.huart, &g_esp8266.rx_buffer, 1);

    // 重置ESP8266
    ESP8266_Reset();

    // 测试AT命令
    if (ESP8266_SendCommand("AT\r\n", "OK", ESP8266_SHORT_TIMEOUT) != ESP8266_OK)
    {
        return ESP8266_ERROR;
    }

    // 设置为station模式
    if (ESP8266_SetMode(1) != ESP8266_OK)
    {
        return ESP8266_ERROR;
    }

    return ESP8266_OK;
}

/**
 * @brief 重置ESP8266
 * @retval ESP8266状态
 */
ESP8266_Status_t ESP8266_Reset(void)
{
    HAL_GPIO_WritePin(ESP8266_RST_GPIO_Port, ESP8266_RST_Pin, GPIO_PIN_RESET);
    ESP8266_Delay(500);
    HAL_GPIO_WritePin(ESP8266_RST_GPIO_Port, ESP8266_RST_Pin, GPIO_PIN_SET);
    ESP8266_Delay(2000); // 等待模块启动

    return ESP8266_OK;
}

/**
 * @brief 发送AT命令并检查响应
 * @param cmd: 命令字符串
 * @param expected: 期望的响应字符串
 * @param timeout: 超时时间
 * @retval ESP8266状态
 */
// ESP8266_Status_t ESP8266_SendCommand(const char *cmd, const char *expected,
// uint32_t timeout)
// {
//     // 清空缓冲区
//     memset(g_esp8266.rxString, 0, sizeof(g_esp8266.rxString));
//
//     // 发送命令
//     if (HAL_UART_Transmit(g_esp8266.huart, (uint8_t *)cmd, strlen(cmd),
//     HAL_MAX_DELAY) !=HAL_OK)
//     {
//         printf("failed to send cmd\n");
//     }
//     // 等待响应
//     if (ESP8266_WaitForResponse(g_esp8266.rxString, timeout))
//     {
//         if (strstr(g_esp8266.rxString, expected) != NULL)
//         {
//             return ESP8266_OK;
//         }
//         else
//         {
//             return ESP8266_ERROR;
//         }
//     }
//     else
//     {
//         return ESP8266_TIMEOUT_ERROR;
//     }
// }
uint8_t UART_Polling_Receive_Data(UART_HandleTypeDef *huart, char *expected, int timeout)
{
    uint8_t buffer[2048];
    uint8_t data;
    uint16_t index = 0;
    HAL_StatusTypeDef status;
    int start_time = HAL_GetTick();

    printf("[INFO] waitting data...\r\n");

    while (HAL_GetTick() - start_time < timeout)
    {
        // 轮询接收一个字节，超时时间为100ms
        status = HAL_UART_Receive(huart, &data, 1, 1000);

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
                if (strstr(buffer, expected) != NULL)
                {
                    printf("matched\n");
                    return 1;
                }

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
    return 0;
}

void UART_Polling_Receive_Data_NotResponse(UART_HandleTypeDef *huart, int timeout)
{
    uint8_t buffer[2048];
    uint8_t data;
    uint16_t index = 0;
    HAL_StatusTypeDef status;
    int start_time = HAL_GetTick();

    printf("[INFO] waitting data...\r\n");

    while (HAL_GetTick() - start_time < timeout)
    {
        // 轮询接收一个字节，超时时间为100ms
        status = HAL_UART_Receive(huart, &data, 1, 1000);

        if (status == HAL_OK)
        {
            // 接收到数据
            buffer[index++] = data;
            // printf("%c", data);

            // 如果接收到回车符，结束接收
            if (data == '\r' || data == '\n')
            {
                buffer[index - 1] = '\0'; // 添加字符串结束符
                printf("%s\r\n", buffer);
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
            // printf("UART Recieve timeout\r\n");
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
ESP8266_Status_t ESP8266_SendCmd(UART_HandleTypeDef *huart, const char *cmd, const char *expected,
                                 uint32_t timeout)
{
    HAL_StatusTypeDef status;
    status = HAL_UART_Transmit(huart, cmd, strlen(cmd), timeout);
    if (status != HAL_OK)
    {
        printf("[ERROR] SendCmd\r\n");
        return ESP8266_ERROR;
    }
    if (UART_Polling_Receive_Data(huart, expected, timeout) != 1)
    {
        printf("[ERROR] SEND: %s\r\n", cmd);
        return ESP8266_ERROR;
    }
    return ESP8266_OK;
}
ESP8266_Status_t ESP8266_SendCmd_NotResponse(UART_HandleTypeDef *huart, const char *cmd,
                                             uint32_t timeout)
{
    HAL_StatusTypeDef status;
    status = HAL_UART_Transmit(huart, cmd, strlen(cmd), timeout);
    if (status != HAL_OK)
    {
        printf("[ERROR] UART SendCmd\r\n");
        return ESP8266_ERROR;
    }
    UART_Polling_Receive_Data_NotResponse(huart, timeout);
    return ESP8266_OK;
}
ESP8266_Status_t ESP8266_SendCommand(const char *cmd, const char *expected, uint32_t timeout)
{
    // 清空缓冲区
    memset(g_esp8266.rxString, 0, sizeof(g_esp8266.rxString));

    // 发送命令
    // if (HAL_UART_Transmit(g_esp8266.huart, (uint8_t *)cmd, strlen(cmd), HAL_MAX_DELAY) != HAL_OK)
    if (HAL_UART_Transmit(g_esp8266.huart, (uint8_t *)cmd, strlen(cmd), 1000) != HAL_OK)
    {
        printf("failed to send cmd\n");
    }
    if (ESP8266_WaitForResponse(expected, timeout) == 1)
    {
        // printCmdInfo();
        return ESP8266_OK;
    }
    else
    {
        // printCmdInfo();
        return ESP8266_ERROR;
    }

    // 1.
    //  // 等待响应
    //  if (ESP8266_ReceiveResponse(g_esp8266.rxString, timeout))
    //  {
    //      if (strstr(g_esp8266.rxString, expected) != NULL)
    //      {
    //          return ESP8266_OK;
    //      }
    //      else
    //      {
    //          return ESP8266_ERROR;
    //      }
    //  }
    //  else
    //  {
    //      return ESP8266_TIMEOUT_ERROR;
    //  }
}
ESP8266_Status_t ESP8266_SendCommand_v0(const char *cmd, const char *expected, uint32_t timeout)
{
    // 清空缓冲区
    memset(g_esp8266.rxString, 0, sizeof(g_esp8266.rxString));

    // 发送命令
    if (HAL_UART_Transmit(g_esp8266.huart, (uint8_t *)cmd, strlen(cmd), HAL_MAX_DELAY) != HAL_OK)
    {
        printf("failed to send cmd\n");
    }
    // 等待响应
    if (ESP8266_ReceiveResponse(g_esp8266.rxString, timeout))
    {
        if (strstr(g_esp8266.rxString, expected) != NULL)
        {
            return ESP8266_OK;
        }
        else
        {
            return ESP8266_ERROR;
        }
    }
    else
    {
        return ESP8266_TIMEOUT_ERROR;
    }
}
/**
 * @brief 设置ESP8266工作模式
 * @param mode: 0-Station模式, 1-SoftAP模式, 2-Station+SoftAP模式
 * @retval ESP8266状态
 */
ESP8266_Status_t ESP8266_SetMode(uint8_t mode)
{
    char cmd[32];
    sprintf(cmd, "AT+CWMODE=%d\r\n", mode);

    return ESP8266_SendCommand(cmd, "OK", ESP8266_SHORT_TIMEOUT);
}

/**
 * @brief 连接WiFi
 * @param ssid: 网络名称
 * @param password: 密码
 * @retval ESP8266状态
 */
ESP8266_Status_t ESP8266_ConnectWiFi(const char *ssid, const char *password)
{
    char cmd[128];
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);

    // 连接WiFi可能需要较长时间
    ESP8266_Status_t status = ESP8266_SendCommand(cmd, "OK", ESP8266_TIMEOUT);

    if (status == ESP8266_OK)
    {
        g_esp8266.connected = 1;
        return ESP8266_OK;
    }
    else
    {
        g_esp8266.connected = 0;
        return ESP8266_CONNECT_ERROR;
    }
}

/**
 * @brief 启动TCP连接
 * @param server_ip: 服务器IP地址
 * @param port: 端口号
 * @retval ESP8266状态
 */
ESP8266_Status_t ESP8266_StartTCP(const char *server_ip, uint16_t port)
{
    char cmd[64];
    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", server_ip, port);

    return ESP8266_SendCommand(cmd, "OK", ESP8266_SHORT_TIMEOUT);
}

/**
 * @brief 发送数据
 * @param data: 要发送的数据
 * @retval ESP8266状态
 */
ESP8266_Status_t ESP8266_SendData(const char *data)
{
    char cmd[32];
    sprintf(cmd, "AT+CIPSEND\r\n", strlen(data));

    // 发送发送命令
    if (ESP8266_SendCommand(cmd, ">", ESP8266_SHORT_TIMEOUT) != ESP8266_OK)
    {
        printf("failed to send data\n");
        // return ESP8266_ERROR;
    }

    // 清空缓冲区
    memset(g_esp8266.rxString, 0, sizeof(g_esp8266.rxString));

    // 发送数据
    // HAL_UART_Transmit(g_esp8266.huart, (uint8_t *)data, strlen(data), HAL_MAX_DELAY);
    HAL_UART_Transmit_DMA(g_esp8266.huart, (uint8_t *)data, strlen(data));

    // 等待发送完成响应
    // if (ESP8266_WaitForResponse("SEND OK", ESP8266_SHORT_TIMEOUT))
    // {
    //         return ESP8266_OK;
    // }

    return ESP8266_ERROR;
}
ESP8266_Status_t ESP8266_Print(const char *data)
{
    char cmd[64];
    sprintf(cmd, "AT+CIPSEND=%d\r\n", strlen(data));

    // 发送数据
    ESP8266_SendCmd_NotResponse(g_esp8266.huart, cmd, ESP8266_SHORT_TIMEOUT);

    // HAL_UART_Transmit_DMA(g_esp8266.huart, (uint8_t *)cmd, strlen(cmd));
    // HAL_Delay(100);
    HAL_UART_Transmit_DMA(g_esp8266.huart, (uint8_t *)data, strlen(data));

    return ESP8266_ERROR;
}
ESP8266_Status_t ESP8266_PrintStr(const char data[])
{
    HAL_StatusTypeDef status;
    status = HAL_UART_Transmit(g_esp8266.huart, (uint8_t *)data, strlen(data), 1000);
    if (status != HAL_OK)
    {
        printf("[ERROR] UART DMA Transmit\n");
    }

    return ESP8266_OK;
}

ESP8266_Status_t ESP8266_PrintFloat(const char string[], const float floatValue)
{
    HAL_StatusTypeDef status;
    char msg[64];
    sprintf(msg, "%s%.3f\n", string, floatValue);
    // status = UART_DMA_Send(g_esp8266.huart, &hdma_usart3_tx, (uint8_t *)msg, strlen(msg));
    status = HAL_UART_Transmit(g_esp8266.huart, (uint8_t *)msg, strlen(msg), 1000);
    if (status != HAL_OK)
    {
        printf("[ERROR] UART DMA Transmit Float\n");
        return ESP8266_ERROR;
    }

    return ESP8266_OK;
}

ESP8266_Status_t ESP8266_PrintInt(const char string[], const int intValue)
{
    HAL_StatusTypeDef status;
    char msg[64];
    // memset(msg, 0, sizeof(msg));
    sprintf(msg, "%s%d\r\n", string, intValue);
    // printf("%s", msg);
    // status = UART_DMA_Send(g_esp8266.huart, &hdma_usart3_tx, (uint8_t *)msg, strlen(msg));
    status = HAL_UART_Transmit(g_esp8266.huart, (uint8_t *)msg, strlen(msg), 1000);
    if (status != HAL_OK)
    {
        printf("[ERROR] UART DMA Transmit Int, status = %d\n", status);
        return ESP8266_ERROR;
    }

    return ESP8266_OK;
}
// ESP8266_Status_t ESP8266_PrintDataFloat(const int data_num, const int intValue, , )
// {
//     HAL_StatusTypeDef status;
//     // char msg[64];
//     // memset(msg, 0, sizeof(msg));
//     sprintf(msg, "%s%d\r\n", string, intValue);
//     // printf("%s", msg);
//     // status = UART_DMA_Send(g_esp8266.huart, &hdma_usart3_tx, (uint8_t *)msg, strlen(msg));
//     status = HAL_UART_Transmit(g_esp8266.huart, (uint8_t *)msg, strlen(msg), 1000);
//     if (status != HAL_OK)
//     {
//         printf("[ERROR] UART DMA Transmit Int, status = %d\n", status);
//         return ESP8266_ERROR;
//     }
//
//     return ESP8266_OK;
// }

// 支持多个float数据打印的函数
ESP8266_Status_t ESP8266_PrintDataFloat(int data_num, ...)
{
    HAL_StatusTypeDef status;
    char msg[512]; // 增大缓冲区以支持多个float数据
    memset(msg, 0, sizeof(msg));

    va_list args;
    va_start(args, data_num);

    // 构建消息字符串
    strcpy(msg, "");
    for (int i = 0; i < data_num; i++)
    {
        float value = va_arg(args, double); // float参数会被提升为double
        if (i == 0)
        {
            sprintf(msg, "%.2f", value); // 保留2位小数，可根据需要调整
        }
        else
        {
            int len = strlen(msg);
            sprintf(msg + len, ",%.2f", value);
        }
    }

    strcat(msg, "\r\n");

    va_end(args);

    status = HAL_UART_Transmit(g_esp8266.huart, (uint8_t *)msg, strlen(msg), 1000);
    if (status != HAL_OK)
    {
        printf("[ERROR] UART Transmit, status = %d\n", status);
        return ESP8266_ERROR;
    }

    return ESP8266_OK;
}

// 支持多个数据打印的函数
ESP8266_Status_t ESP8266_PrintDataInt(int data_num, ...)
{
    HAL_StatusTypeDef status;
    char msg[256]; // 增大缓冲区以支持多个数据
    memset(msg, 0, sizeof(msg));

    va_list args;
    va_start(args, data_num);

    // 构建消息字符串
    strcpy(msg, "");
    for (int i = 0; i < data_num; i++)
    {
        int value = va_arg(args, int);
        if (i == 0)
        {
            sprintf(msg, "%d", value);
        }
        else
        {
            int len = strlen(msg);
            sprintf(msg + len, ",%d", value);
        }
    }

    strcat(msg, "\r\n");

    va_end(args);

    // 使用轮询方式发送（如果DMA不稳定）
    status = HAL_UART_Transmit(g_esp8266.huart, (uint8_t *)msg, strlen(msg), 1000);
    if (status != HAL_OK)
    {
        printf("[ERROR] UART Transmit, status = %d\n", status);
        return ESP8266_ERROR;
    }

    return ESP8266_OK;
}

/**
 * @brief 关闭TCP连接
 * @retval ESP8266状态
 */
ESP8266_Status_t ESP8266_CloseTCP(void)
{
    return ESP8266_SendCommand("AT+CIPCLOSE\r\n", "OK", ESP8266_SHORT_TIMEOUT);
}

/**
 * @brief 获取本地IP地址
 * @retval ESP8266状态
 */
ESP8266_Status_t ESP8266_GetLocalIP(void)
{
    if (ESP8266_SendCommand("AT+CIFSR\r\n", "OK", ESP8266_SHORT_TIMEOUT) == ESP8266_OK)
    {
        // 解析IP地址（这里简化处理，实际应用中需要更复杂的解析）
        char *ip_start = strstr(g_esp8266.buffer, "\"");
        if (ip_start != NULL)
        {
            ip_start++;
            char *ip_end = strstr(ip_start, "\"");
            if (ip_end != NULL)
            {
                *ip_end = '\0';
                strncpy(g_esp8266.ip, ip_start, sizeof(g_esp8266.ip) - 1);
                g_esp8266.ip[sizeof(g_esp8266.ip) - 1] = '\0';
                return ESP8266_OK;
            }
        }
    }
    return ESP8266_ERROR;
}

/**
 * @brief 检查是否连接到WiFi
 * @retval 1-已连接, 0-未连接
 */
uint8_t ESP8266_IsConnected(void)
{
    return g_esp8266.connected;
}

/**
 * @brief 延时函数
 * @param ms: 延时毫秒数
 */
static void ESP8266_Delay(uint32_t ms)
{
    HAL_Delay(ms);
}
/**
 * @brief 等待特定响应
 * @param expected: 期望的响应字符串
 * @param timeout: 超时时间
 * @retval 1-成功等待到, 0-超时
 */
static uint8_t ESP8266_WaitForResponse_v0(const char *expected, uint32_t timeout)
{
    uint32_t start_time = HAL_GetTick();
    uint16_t last_index = 0;

    while (HAL_GetTick() - start_time < timeout)
    {
        // 检查是否收到期望的响应
        // printf("waiting for response...\n");
        if (strstr(g_esp8266.rxString, expected) != NULL)
        {
            // g_esp8266.rx_index = 0;
            return 1;
        }

        // // 如果接收到的数据没有变化，继续等待
        // if (last_index != g_esp8266.rx_index)
        // {
        //     last_index = g_esp8266.rx_index;
        //     // printf("index = %d\n", g_esp8266.rx_index);
        //     start_time = HAL_GetTick(); // 重置超时计时器，因为收到了新数据
        // }

        HAL_Delay(1); // 短暂延时，避免CPU占用过高
    }

    return 0; // 超时
}
/**
 * @brief polling 等待特定响应
 * @param expected: 期望的响应字符串
 * @param timeout: 超时时间
 * @retval 1-成功等待到, 0-超时
 */
static uint8_t ESP8266_WaitForResponse(const char *expected, uint32_t timeout)
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
        status = HAL_UART_Receive(g_esp8266.huart, &data, 1, 100);

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
                if (strstr(buffer, expected) != NULL)
                {
                    return 1;
                }
                // memset(rxData, 0, strlen(rxData));

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
/**
 * @brief 接收ESP8266响应 - 使用轮询方式
 * @param response: 存储响应的缓冲区
 * @param timeout: 超时时间
 * @retval 1-成功接收, 0-超时
 */
static uint8_t ESP8266_ReceiveResponse(char *response, uint32_t timeout)
{
    uint8_t ch;
    HAL_StatusTypeDef status;

    response = g_esp8266.rxString;
    // 检查是否收到完整响应
    if (strstr(response, "OK") != NULL)
    {
        return 1;
    }
    if (strstr(response, "ERROR") != NULL)
    {
        return 1;
    }
    if (strstr(response, "FAIL") != NULL)
    {
        return 1;
    }
    if (strstr(response, "CONNECT") != NULL)
    {
        return 1;
    }
    // 如果没有数据，继续循环

    return 0; // 超时
}
static uint8_t ESP8266_ReceiveResponse_v0(char *response, uint32_t timeout)
{
    uint32_t start_time = HAL_GetTick();
    uint16_t len = 0;
    uint8_t ch;
    HAL_StatusTypeDef status;

    while (HAL_GetTick() - start_time < timeout)
    {
        // 尝试接收一个字节（非阻塞）
        status = HAL_UART_Receive(g_esp8266.huart, &ch, 1, 10);

        if (status == HAL_OK)
        {
            if (len < sizeof(g_esp8266.rxString) - 1)
            {
                response[len++] = ch;
                response[len] = '\0';

                printf("response: %s\n", response);

                // 检查是否收到完整响应
                if (len > 2 && strstr(response, "OK") != NULL)
                {
                    return 1;
                }
                if (len > 5 && strstr(response, "ERROR") != NULL)
                {
                    return 1;
                }
                if (len > 7 && strstr(response, "FAIL") != NULL)
                {
                    return 1;
                }
                if (len > 7 && strstr(response, "CONNECT") != NULL)
                {
                    return 1;
                }
            }
        }
        // 如果没有数据，继续循环
    }

    return 0; // 超时
}

// extern ESP8266_HandleTypeDef g_esp8266;
void Example_ESP8266_0(void)
{
    char str[] = "let's go\n";
    float floatV = 100.1f;
    int n = 10;
    // g_esp8266.huart = &huart3;
    ESP8266_ClientInit(&huart3);
    while (1)
    {
        ESP8266_PrintStr(str);
        ESP8266_PrintInt("n:", n);
        ESP8266_PrintFloat("float=", floatV);
        ESP8266_PrintDataFloat(3, floatV, 2.1, 3.2);
        delay_ms(1000);
    }
}

/**
 * @brief UART接收完成回调函数
 * @param huart: UART句柄
 */
void ESP8266_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == g_esp8266.huart)
    {
        // 将接收到的数据存储到缓冲区或进行其他处理
        // 这里简单地重新启动接收
        HAL_UART_Receive_IT(g_esp8266.huart, &g_esp8266.rx_buffer, 1);
    }
}

// // 如果使用回调注册，需要在main函数中注册
// #ifdef USE_HAL_UART_REGISTER_CALLBACKS
// void ESP8266_RegisterCallbacks(void)
// {
//     HAL_UART_RegisterRxEventCallback(g_esp8266.huart,
//     ESP8266_UART_RxCpltCallback);
// }
// #endif