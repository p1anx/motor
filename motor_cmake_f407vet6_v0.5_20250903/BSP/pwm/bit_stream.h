#ifndef __BIT_STREAM_H
#define __BIT_STREAM_H

#include "mymain.h"
#include "pid_init.h"

typedef struct
{
    float target_vel;
    float target_pos;
}Target;

int bit_stream_test();
int char_to_bit_stream(void);
char* uart_bitstream(const char* input);
// int char_to_bit_stream_uart(void);
int char_to_bit_stream_uart(Motor_InitStruct* motor);

#endif
