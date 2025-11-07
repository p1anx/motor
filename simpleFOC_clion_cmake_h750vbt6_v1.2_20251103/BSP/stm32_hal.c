#include "stm32_hal.h"
#include <string.h>
#include <stdio.h>

void delay_ms(int ms)
{
    HAL_Delay(ms);
}
// 发送浮点数数组的函数
void UART_SendDataFloat(float *data, uint8_t count)
{
    char buffer[200]; // 缓冲区大小根据实际需要调整
    char temp[20];    // 临时存储单个浮点数

    // 清空缓冲区
    memset(buffer, 0, sizeof(buffer));

    // 构建格式化字符串
    for(uint8_t i = 0; i < count; i++)
    {
        sprintf(temp, "%.3f", data[i]); // 保留2位小数
        strcat(buffer, temp);

        if(i < count - 1) // 不是最后一个数据时添加逗号
        {
            strcat(buffer, ",");
        }
    }

    // 添加换行符
    strcat(buffer, "\n");

    // 发送数据
    HAL_UART_Transmit(&print_uart, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
}

// 使用示例
void Example(void)
{
    float sensor_data[4] = {3.14159f, 2.71828f, 1.41421f, 0.57721f};

    UART_SendFloatData(&huart1, sensor_data, 4);
}