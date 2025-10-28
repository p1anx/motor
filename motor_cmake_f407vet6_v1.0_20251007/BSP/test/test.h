
#ifndef __TEST_H
#define __TEST_H
#include "led.h"
#include "motor.h"

#include "main.h"
#include "pid.h"
#include "qam.h"
#include "stm32_hal.h"
#include "stm32_hal_legacy.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_uart.h"
#include "tim.h"
#include "types.h"
#include "usart.h"
// #include "usart_mx.h"
#include <stdint.h>
#include <stdio.h>
#include "pwm.h"
#include "encoder.h"
#include "tb6612_init.h"
#include "uart_it.h"
#include "common.h"

void test_main(void);
void test_pid(Motor_t *motor);

void test_motor_calibrate(void);
void test_led(void);
void test_printf(void);
int test_pwm(void);
int test_encoder(void);
int test_tb6612(void);
void test_uart(void);

int test_pid_vel(void);
int test_pid_vel_compare(void);
int test_pid_postion(void);

int test_motor_velociyLoop(void);
int test_motor_velociyLoop_disableTimer(void);

int test_motor_PositionLoop(void);
int test_motor_PositionLoop_0(void);

int test_encoder_angle(void);
int test_motor_AngleLoop(void);
int test_qam16(void);
void test_communication_with_python(void);

void test_magneticLoop(void);
void test_lis3mdl(void);
int test_motor_PositionLoop_1(void);
#endif // !__TEST_H
