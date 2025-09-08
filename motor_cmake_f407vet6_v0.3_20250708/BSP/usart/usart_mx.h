#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

// #include "stm32f1xx.h"
#include "mymain.h"
#include <stdio.h>
/***********add command as follow to `platformio.ini`*************/
/*****************************************************************
build_flags = 
            -Wl,-u,_printf_float
******************************************************************/
// extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart1;

#define mx_huart1 huart1
void UART_SendStr(uint8_t *str);
int fputc(int ch, FILE *f);
int fgetc(FILE *f);
void my_printf(const char *fmt, ...); // custom printf() function
#endif /* __USART1_H */