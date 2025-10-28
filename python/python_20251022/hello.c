// #include <stdio.h>

// float getFloatFromCMDString(const char *pstr, int skip_char)
// {
//     char *endptr;
//     char *pstrFloat;
//     float result;
//     // PID_Controller* pid_init = motor->inc_pid_vel_init;
//     pstrFloat = pstr + skip_char;
//     result = strtof(pstrFloat, &endptr);
//     return result;
// }

// int main(void)
// {
//     char *endptr;
//     char *str = "pid=10.0";
//     float result = strtof(str, &endptr);
//     printf("str = %s\n", str);
//     printf("data = %f\n", result);
//     return 0;
// }
#include <stdio.h>
// #include <string.h>

int getCMD(char cmd[], char cmd_char, float *float_value)
{
    char format[20];
    sprintf(format, "%c=%%f", cmd_char);

    if (sscanf(cmd, format, float_value) == 1)
    {
        printf("cmd data: %f\n", *float_value);
        return 0;
    }
    return -1;
}
float getFloatFromCMD(char cmd[], char cmd_char)
{
    float float_value;
    char format[20];
    sprintf(format, "%c=%%f", cmd_char);

    if (sscanf(cmd, format, &float_value) == 1)
    {

        return float_value;
    }
    printf("[error] failed to get data from cmd\n[note] input format: p=1.1\n");
    return -1;
}
float getFloatFromString(char cmd[], char cmd_char)
{
    float float_value;
    char format[20];
    sprintf(format, "%c=%%f", cmd_char);

    if (sscanf(cmd, format, &float_value) == 1)
    {

        return float_value;
    }
    printf("[error] failed to get data from string\n[note] input format: p=1.1\n");
    return -1;
}

void test0(void)
{

    char str1[] = "The number is 123";
    char str2[] = "Value: 45.67";
    char str3[] = "123.45 678 9.10";
    char str4[] = "p=1";

    int int_val;
    float float_val;

    if (sscanf(str4, "p=%f", &float_val) == 1)
    {
        printf("my提取的整数: %f\n", float_val);
    }
    // 从字符串中提取整数
    if (sscanf(str1, "The number is %d", &int_val) == 1)
    {
        printf("提取的整数: %d\n", int_val);
    }

    // 从字符串中提取浮点数
    if (sscanf(str2, "Value: %f", &float_val) == 1)
    {
        printf("提取的浮点数: %f\n", float_val);
    }

    // 从字符串中提取多个数值
    float f1, f2;
    int i1;
    if (sscanf(str3, "%f %d %f", &f1, &i1, &f2) == 3)
    {
        printf("提取的值: %f, %d, %f\n", f1, i1, f2);
    }
}
int main()
{
    char cmd[] = "p=10.1";
    char cmd_char = 'p';
    float data;

    // getCMD(cmd, 'p', &data);
    // 2.
    data = getFloatFromString(cmd, 'p');
    printf("data = %f\n", data);
    // char cmd[] = "p=123.45";
    // float float_value;

    // // 动态构建格式字符串
    // char format[20];
    // char command_char = 'p';

    // sprintf(format, "%c=%%f", command_char); // 注意%%f表示字面量%f
    // printf("构建的格式: %s\n", format);

    // if (sscanf(cmd, format, &float_value) == 1)
    // {
    //     printf("提取的值: %f\n", float_value);
    // }

    return 0;
}