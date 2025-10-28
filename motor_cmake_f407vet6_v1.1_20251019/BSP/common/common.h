#ifndef __COMMON_H
#define __COMMON_H
#include "stm32_hal.h"
#include "pid.h"
typedef struct GlobalVar_t GlobalVar_t;

struct GlobalVar_t
{
    float pwm_duty;
    float B_avg;
};

int extract_integer(const char *str, int *result);
int getInt_from_str(const char *pstr, int skip_char);
float getFloat_from_str(const char *pstr, int skip_char);
float getFloatFromString(char cmd[], char cmd_char);
float getFloatFromCmdString(const char *pstr, const char *cmd);
// float getFloatFromCmd(char cmd_char, char rx);
float getFloatFromUART(char cmd[], char rx);
int getFloatFromCmd(const char *pstr, const char cmd[], float *data);

int getFloatArrayFromCmd(const char *pstr, const char cmd[], int array_size, float *data);
char* receiveStringFromUART(char rx_char);
int strcpyFromUART(char *rxString, char rx_char);

uint32_t GetMicros(void);
float GetMicrosDelta_T(void);
#endif // !__COMMON_H
