#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 256

static char buffer[MAX_BUFFER_SIZE];
static int buffer_index = 0;

GlobalVar_t g_var;

int extract_integer(const char *str, int *result)
{
    if (str == NULL || result == NULL)
        return -1;

    while (*str && !isdigit(*str) && *str != '-' && *str != '+')
    {
        str++;
    }

    if (*str == '\0')
        return -1;

    char *endptr;
    long value = strtol(str, &endptr, 10);

    if (endptr == str)
        return -1;

    *result = (int)value;
    return 0;
}
int getInt_from_str(const char *pstr, int skip_char)
{
    char *endptr;
    char *pstrInt;
    int result;
    // PID_Controller* pid_init = motor->inc_pid_vel_init;
    pstrInt = pstr + skip_char;
    result = strtol(pstrInt, &endptr, 10);
    return result;
}
float getFloat_from_str(const char *pstr, int skip_char)
{
    char *endptr;
    char *pstrFloat;
    float result;
    // PID_Controller* pid_init = motor->inc_pid_vel_init;
    pstrFloat = pstr + skip_char;
    result = strtof(pstrFloat, &endptr);
    return result;
}
float getFloatFromCmdString(const char *pstr, const char cmd[])
{
    char *endptr;
    char *pstrFloat;
    float result;
    // const int cmd_n = sizeof(cmd)/sizeof(cmd[0]);
    const int cmd_n = strlen(cmd);
    // printf("cmd n = %d\n", cmd_n);
    // PID_Controller* pid_init = motor->inc_pid_vel_init;
    if (strncmp(pstr, cmd, cmd_n) == 0)
    {
        pstrFloat = pstr + cmd_n;
        result = strtof(pstrFloat, &endptr);
        return result;
    }
    printf("failed to get data from cmd\n");
    return -1;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <ctype.h>

int getFloatArrayFromCmd(const char *pstr, const char cmd[], int array_size, float *data)
{
    char *pstrStart;
    const int cmd_n = strlen(cmd);
    int count = 0;

    // 检查命令前缀是否匹配
    if (strncmp(pstr, cmd, cmd_n) != 0) {
        return -1; // 命令不匹配
    }

    // 找到等号后面的位置
    pstrStart = strchr(pstr + cmd_n, '=');
    if (pstrStart == NULL) {
        return -1; // 没有找到等号
    }
    pstrStart++; // 跳过等号

    // 解析逗号分隔的浮点数
    char *token = strtok(pstrStart, ",");
    while (token != NULL && count < array_size) {
        // 去除前后空格
        while (isspace((unsigned char)*token)) {
            token++;
        }
        int len = strlen(token);
        while (len > 0 && isspace((unsigned char)token[len - 1])) {
            token[--len] = '\0';
        }

        if (len > 0) {
            char *endptr;
            float value = strtof(token, &endptr);
            if (endptr == token) {
                // 转换失败
                return -1;
            }
            data[count] = value;
            count++;
        }

        token = strtok(NULL, ",");
    }

    return count; // 返回实际解析到的元素个数
}

// 使用示例
// int main() {
//     const char *cmd = "p";
//     const char *input = "p=1.0,2.0,3.0";
//     float data[10];
//     int array_size = 10;
//
//     int result = getFloatArrayFromCmd(input, cmd, array_size, data);
//
//     if (result >= 0) {
//         printf("成功解析到 %d 个元素:\n", result);
//         for (int i = 0; i < result; i++) {
//             printf("data[%d] = %.2f\n", i, data[i]);
//         }
//     } else {
//         printf("解析失败\n");
//     }
//
//     // 测试其他格式
//     const char *input2 = "p=1.5, 2.5, 3.5, 4.5";
//     int result2 = getFloatArrayFromCmd(input2, cmd, array_size, data);
//
//     if (result2 >= 0) {
//         printf("\n成功解析到 %d 个元素:\n", result2);
//         for (int i = 0; i < result2; i++) {
//             printf("data[%d] = %.2f\n", i, data[i]);
//         }
//     }
//
//     return 0;
// }
int getFloatFromCmd(const char *pstr, const char cmd[], float *data)
{
    char *endptr;
    char *pstrFloat;
    float result;
    // const int cmd_n = sizeof(cmd)/sizeof(cmd[0]);
    const int cmd_n = strlen(cmd);
    // printf("cmd n = %d\n", cmd_n);
    // PID_Controller* pid_init = motor->inc_pid_vel_init;
    if (strncmp(pstr, cmd, cmd_n) == 0)
    {
        pstrFloat = pstr + cmd_n;
        result = strtof(pstrFloat, &endptr);
        *data = result;
        return 0;
    }
    // printf("failed to get data from cmd\n");
    return -1;
}
/*
 * example of usage
 * function: getFloatFromCmd(const char *pstr, const char cmd[], float *data)
 */
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     if (huart->Instance == USART1)
//     {
//         if (strcpyFromUART(str, rx) ==0 )
//         {
//             float p_data;
//             if ( getFloatFromCmd(str, "p=", &p_data) == 0)
//             {
//                 printf("pdata = %f\n", p_data);
//             }
//             float i_data;
//             if ( getFloatFromCmd(str, "i=", &i_data) == 0)
//             {
//                 printf("idata = %f\n", i_data);
//             }
//         }
//         HAL_UART_Receive_IT(&MY_UART_1, &rx, RXBUFFER_SIZE);
//     }
// }


// this can't be used due to `sscanf()`
// float getFloatFromString(char cmd[], char cmd_char)
// {
//     float float_value;
//     char format[20];
//     sprintf(format, "%c=%%f", cmd_char);
//
//     if (sscanf(cmd, format, &float_value) == 1)
//     {
//
//         return float_value;
//     }
//     printf("[error] failed to get data from string\n[note] input format: p=1.1\n");
//     return -1;
// }

float getFloatFromUART(char cmd[], char rx)
{
    static char str[100];
    if (strcpyFromUART(str, rx) ==0 )
    {
        // printf("uart str = %s\n", str);
        float data = getFloatFromCmdString(str,cmd);
        // printf("data = %f\n", data);
        return data;
    }
    // printf("[error] failed to get data from cmd\n[note] input format: p=1.1\n");
    return -1;

}
uint32_t GetMicros(void)
{
    return __HAL_TIM_GET_COUNTER(&pid_update_htim);
}
float GetMicrosDelta_T(void)
{
    static uint32_t last_t = 0;
    static uint32_t start_time = 0;
    uint32_t current = GetMicros();
    uint32_t delta_t;
    if (current >= start_time)
    {
        delta_t = current - start_time;
    }
    else
    {
        // 处理定时器溢出
        delta_t = (65535 - start_time) + current + 1;
    }
    start_time = current;
    return (float)delta_t * 1e-6;
}
void loop(int ms)
{
    uint32_t last_time = HAL_GetTick();
    const uint32_t loop_period = ms; // 500ms

    while (1)
    {
        uint32_t current_time = HAL_GetTick();

        if (current_time - last_time >= loop_period)
        {
            // 执行500ms周期任务

            // 更新时间戳，保持精确周期
            last_time = current_time;
            // 或者用这种方式避免累积误差：
            // last_time += loop_period;
        }

        // 其他非周期性任务可以在这里执行
    }
}


// 逐字符接收函数
char* receiveStringFromUART(char rx_char) {
    // 检查是否是换行符
    // printf("rx char = %c\n", rx_char);
    char *result;
    if (rx_char == '\n' || rx_char == '\r') {
        if (buffer_index > 0) { // 确保有数据
            buffer[buffer_index] = '\0'; // 添加字符串结束符

            // 分配新内存存储完整字符串
            char *result = (char*)malloc((buffer_index + 1) * sizeof(char));
            if (result != NULL) {
                strcpy(result, buffer);
            }

            // 重置缓冲区
            buffer_index = 0;
            memset(buffer, 0, sizeof(buffer));

            return result; // 返回完整字符串
        }
    } else {
        // 添加字符到缓冲区（防止溢出）
        if (buffer_index < MAX_BUFFER_SIZE - 1) {
            buffer[buffer_index++] = rx_char;
        }
    }

    return NULL; // 还没接收到完整字符串
}

int strcpyFromUART(char *rxString, char rx_char)
{
    char *result = receiveStringFromUART(rx_char);
    if (result != NULL) {
        strcpy(rxString, result);
        free(result); // 记得释放内存
        return 0;
    }
    return -1;

}
/*
function: int strcpyFromUART(char *rxString, char rx_char)
usage example
*/
// #define RXBUFFER 10
// char str[RXBUFFER];
// uint8_t rx;
// void example_HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     if (huart->Instance == USART1)
//     {
//         //1.
//         // char *result = receiveStringFromUART(rx);
//         // if (result != NULL) {
//         //     printf("rx str: %s\n", result);
//         //     free(result); // 记得释放内存
//         // }
//         //2.
//         if (strcpyFromUART(str, rx) ==0 )
//         {
//             printf("uart str = %s\n", str);
//         }
//         HAL_UART_Receive_IT(&MY_UART_1, &rx, RXBUFFER_SIZE);
//     }
// }

// 逐字符接收函数
char* receiveStringFromUART_0(char rx_char) {
    // 检查是否是换行符
    // printf("rx char = %c\n", rx_char);
    if (rx_char == '\n' || rx_char == '\r') {
        if (buffer_index > 0) { // 确保有数据
            buffer[buffer_index] = '\0'; // 添加字符串结束符

            // 分配新内存存储完整字符串
            char *result = (char*)malloc((buffer_index + 1) * sizeof(char));
            if (result != NULL) {
                strcpy(result, buffer);
            }

            // 重置缓冲区
            buffer_index = 0;
            memset(buffer, 0, sizeof(buffer));

            return result; // 返回完整字符串
        }
    } else {
        // 添加字符到缓冲区（防止溢出）
        if (buffer_index < MAX_BUFFER_SIZE - 1) {
            buffer[buffer_index++] = rx_char;
        }
    }

    return NULL; // 还没接收到完整字符串
}
