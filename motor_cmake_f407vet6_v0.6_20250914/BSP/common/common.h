#ifndef __COMMON_H
#define __COMMON_H

typedef struct GlobalVar_t GlobalVar_t;

struct GlobalVar_t
{
    float pwm_duty;
};

int extract_integer(const char *str, int *result);
int getInt_from_str(const char *pstr, int skip_char);
float getFloat_from_str(const char *pstr, int skip_char);
#endif // !__COMMON_H
