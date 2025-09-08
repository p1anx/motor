#ifndef __MY_MAIN_H
#define __MY_MAIN_H

#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#include "led.h"
#include "pwm.h"
#include "pwm_server.h"

#include "OLED.h"
#include "usart_mx.h"

#include "lis3mdl.h"
int mymain(void);

#endif // !__MY_MAIN_H