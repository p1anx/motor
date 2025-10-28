#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 256

static char buffer[MAX_BUFFER_SIZE];
static int buffer_index = 0;

// 逐字符接收函数
char *receiveStringFromUART(char rx_char)
{
    // 检查是否是换行符
    if (rx_char == '\n' || rx_char == '\r')
    {
        if (buffer_index > 0)
        {                                // 确保有数据
            buffer[buffer_index] = '\0'; // 添加字符串结束符

            // 分配新内存存储完整字符串
            char *result = (char *)malloc((buffer_index + 1) * sizeof(char));
            if (result != NULL)
            {
                strcpy(result, buffer);
            }

            // 重置缓冲区
            buffer_index = 0;
            memset(buffer, 0, sizeof(buffer));

            return result; // 返回完整字符串
        }
    }
    else
    {
        // 添加字符到缓冲区（防止溢出）
        if (buffer_index < MAX_BUFFER_SIZE - 1)
        {
            buffer[buffer_index++] = rx_char;
        }
    }

    return NULL; // 还没接收到完整字符串
}

// 使用示例
int main()
{
    // 模拟接收字符
    char test_data[] = "Hello World\n";

    for (int i = 0; test_data[i] != '\0'; i++)
    {
        char *result = receiveStringFromUART(test_data[i]);
        if (result != NULL)
        {
            printf("接收到完整字符串: %s\n", result);
            free(result); // 记得释放内存
        }
    }

    return 0;
}
