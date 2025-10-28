#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
