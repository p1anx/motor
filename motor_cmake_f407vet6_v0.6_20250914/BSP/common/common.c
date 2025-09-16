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
