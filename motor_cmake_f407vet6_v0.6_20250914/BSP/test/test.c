#include "test.h"
#include "led.h"
#include "main.h"
#include "motor.h"
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
#include "usart_mx.h"
#include <stdint.h>
#include <stdio.h>
#include "pwm.h"
#include "encoder.h"
#include "tb6612_init.h"
#include "uart_it.h"
#include "common.h"

extern uint8_t rxBuffer;
extern uint8_t rx;

extern GlobalVar_t g_var;

Encoder_t encoder;
PIDController_t pid_velocity;
PIDController_t pid_position;
PIDController_t pid_angle;
Motor_t motor;
TB6612_t tb6612;
PWM_t pwm;
QAM_t qam16;
QAM_t qam;

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

int test_encoder_angle(void);
int test_motor_AngleLoop(void);
int test_qam16(void);

void test_main(void)
{
    test_led();
    // test_pwm();
    // test_encoder();
    // test_tb6612();
    // test_uart();
    // test_pid_vel();
    // test_pid_vel_compare();
    // test_pid_postion();
    // test_motor_velociyLoop();
    // test_motor_velociyLoop_disableTimer();
    // test_motor_PositionLoop();
    // test_encoder_angle();
    // test_motor_AngleLoop();
    test_qam16();
}
void test_pid(Motor_t *motor)
{
    // motor_pid_velocity(motor, 2);
    motor_move(motor, motor->target);
}

void test_led(void)
{

    LED1_TOGGLE();
    delay_ms(1000);
}

void test_printf(void)
{
    while (1)
    {
        printf("hello\n");
        delay_ms(1000);
    }
}
// extern TIM_HandleTypeDef *htim1;
int test_pwm(void)
{
    PWM_t pwm;
    pwm.tim = &htim1;
    pwm.channel = TIM_CHANNEL_4;
    pwm.resolution = 4096; // 12bit -> 4096
    pwm.frequency = 10e3;  // Hz
    pwm.tim_psc = pwm_calculatePSC(&pwm);
    pwm.arr = __HAL_TIM_GetAutoreload(&htim1);
    pwm_init(&pwm);
    printf("f = %d Hz\n", pwm.frequency);
    printf("psc = %d\n", pwm.tim_psc);
    printf("arr = %d\n", pwm.arr);
    return 0;
}

int test_encoder(void)
{
    encoder_init(&encoder);
    TB6612_ENABLE();

    while (1)
    {
        int now_count = __HAL_TIM_GET_COUNTER(encoder.htim);
        DEBUG_PRINT("count = %d, pulses = %d", now_count, encoder_getPulses(&encoder));

        // printf("coutn = %d\n", now_count);
        printf("pulses = %d\n", encoder.pulses);
        printf("overflow_count = %d\n", encoder.overflow_count);
        delay_ms(1000);
    }

    return 0;
}
int test_tb6612(void)
{
    encoder.htim = &encoder_htim;
    encoder.count_period = 0xffff;
    encoder.overflow_count = 0;
    encoder.pulses = 0;

    PWM_t pwm;
    pwm.tim = &htim1;
    pwm.channel = TIM_CHANNEL_4;
    pwm.resolution = 4096; // 12bit -> 4096
    pwm.frequency = 1e3;   // Hz
    pwm.tim_psc = pwm_calculatePSC(&pwm);

    TB6612_t tb6612;
    tb6612.encoder = &encoder;
    tb6612.pwm = &pwm;
    tb6612.IN1_GPIO_Port = TB6612_AIN1_GPIO_PORT;
    tb6612.IN1_GPIO_Pin = TB6612_AIN1_GPIO_PIN;
    tb6612.IN2_GPIO_Port = TB6612_AIN2_GPIO_PORT;
    tb6612.IN2_GPIO_Pin = TB6612_AIN2_GPIO_PIN;
    tb6612.STBYx_GPIO_Port = TB6612_STBY_GPIO_PORT;
    tb6612.STBYx_GPIO_Pin = TB6612_STBY_GPIO_PIN;

    Motor_t motor;
    motor_linkTB6612(&motor, &tb6612);

    tb6612_init(&tb6612);
    pwm.duty = 0.2;
    pwm_setPWM(&pwm);
    g_var.pwm_duty = 0.1;

    if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    {
        printf("failed to start uart it\n");
    }
    while (1)
    {
        pwm.duty = g_var.pwm_duty;
        pwm_setPWM(&pwm);
        tb6612_setDirection(&tb6612, 1);
        int now_count = __HAL_TIM_GET_COUNTER(encoder.htim);
        // DEBUG_PRINT("count = %d, pulses = %d", now_count, encoder_getPulses(&encoder));
        printf("pulses = %d\n", encoder.pulses);
        float vel = encoder_getVelocity(&encoder);
        // DEBUG_PRINT("vel = %f\n", vel);
        printf("ve = %f\n", vel);
        delay_ms(1000);
    }
    return 0;
}

void test_uart(void)
{
    if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    {
        printf("failed to start uart it\n");
    }
    else
    {
        printf("start uart rx\n");
    }
    while (1)
    {
        // printf("uart\n");
    }
}

int test_pid_vel(void)
{

    if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    {
        printf("failed to start uart it\n");
    }
    encoder.htim = &encoder_htim;
    encoder.count_period = 0xffff;
    encoder.overflow_count = 0;
    encoder.pulses = 0;

    // = {.htim = &htim8, .count_period = 0xffff, .overflow_count = 0, .pulses = 0}
    pwm.tim = &htim1;
    pwm.channel = TIM_CHANNEL_4;
    pwm.resolution = 4096; // 12bit -> 4096
    pwm.frequency = 10e3;  // Hz
    pwm.tim_psc = pwm_calculatePSC(&pwm);

    tb6612.encoder = &encoder;
    tb6612.pwm = &pwm;
    tb6612.IN1_GPIO_Port = TB6612_AIN1_GPIO_PORT;
    tb6612.IN1_GPIO_Pin = TB6612_AIN1_GPIO_PIN;
    tb6612.IN2_GPIO_Port = TB6612_AIN2_GPIO_PORT;
    tb6612.IN2_GPIO_Pin = TB6612_AIN2_GPIO_PIN;
    tb6612.STBYx_GPIO_Port = TB6612_STBY_GPIO_PORT;
    tb6612.STBYx_GPIO_Pin = TB6612_STBY_GPIO_PIN;

    motor.pid_velocity = &pid_velocity;

    motor_linkTB6612(&motor, &tb6612);

    tb6612_init(&tb6612);
    pwm.duty = 0.0;
    pwm_setPWM(&pwm);

    // tb6612_setDirection(&tb6612, 1);

    // pid_velocity.tim = &htim8;

    // if (HAL_TIM_Base_Start_IT(&htim8) != HAL_OK)
    // {
    //     printf("[error] pid update timer init");
    // }
    // else
    // {
    //     printf("[ok] pid update timer init");
    // }
    // __HAL_TIM_SET_PRESCALER(pid->tim, SystemCoreClock / pid->frequency - 1);
    // __HAL_TIM_SetAutoreload(pid->tim, pid->dt * pid->frequency - 1);
    PID_Init(&pid_velocity);

    pid_velocity.Kp = 0.3;
    pid_velocity.Ki = 0.05;
    pid_velocity.Kd = 0;
    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    while (1)
    {
        // printf("arr = %d\n", __HAL_TIM_GetAutoreload(&pid_htim));
        // delay_ms(1000);

        // pwm.duty = g_var.pwm_duty;
        // pwm_setPWM(&pwm);
        // float vel = motor_getVelocity(&motor);
        // DEBUG_PRINT("vel = %f\n", vel);

        // DEBUG_PRINT("pulses = %d", encoder_getPulses(&encoder));
        // printf("pulses = %d\n", encoder.pulses);
        // float vel = encoder_getVelocity(&encoder);

        // motor_pid_velocity(&motor, 2);
        // DEBUG_PRINT("output = %f", motor.pid_velocity->output);
        // DEBUG_PRINT("now vel = %f", motor.velocity);

        // DEBUG_PRINT("%.3f\n", motor.velocity);
        printf("%d,", __HAL_TIM_GetAutoreload(&pid_htim));
        printf("%.3f, %.3f\n", motor.target_vel, motor.velocity);

        // // motor_pid_velocity(&motor, 1);
        // delay_ms(pid_velocity.dt);
    }
    return 0;
}

int test_pid_vel_compare(void)
{

    if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    {
        printf("failed to start uart it\n");
    }
    encoder.htim = &encoder_htim;
    encoder.count_period = 0xffff;
    encoder.overflow_count = 0;
    encoder.pulses = 0;

    // = {.htim = &htim8, .count_period = 0xffff, .overflow_count = 0, .pulses = 0}
    pwm.tim = &htim1;
    pwm.channel = TIM_CHANNEL_4;
    pwm.resolution = 4096; // 12bit -> 4096
    pwm.frequency = 10e3;  // Hz
    pwm.tim_psc = pwm_calculatePSC(&pwm);

    tb6612.encoder = &encoder;
    tb6612.pwm = &pwm;
    tb6612.IN1_GPIO_Port = TB6612_AIN1_GPIO_PORT;
    tb6612.IN1_GPIO_Pin = TB6612_AIN1_GPIO_PIN;
    tb6612.IN2_GPIO_Port = TB6612_AIN2_GPIO_PORT;
    tb6612.IN2_GPIO_Pin = TB6612_AIN2_GPIO_PIN;
    tb6612.STBYx_GPIO_Port = TB6612_STBY_GPIO_PORT;
    tb6612.STBYx_GPIO_Pin = TB6612_STBY_GPIO_PIN;

    motor.pid_velocity = &pid_velocity;

    motor_linkTB6612(&motor, &tb6612);

    tb6612_init(&tb6612);
    pwm.duty = 0.1;
    pwm_setPWM(&pwm);

    tb6612_setDirection(&tb6612, 1);

    // pid_velocity.tim = &htim8;

    // if (HAL_TIM_Base_Start_IT(&htim8) != HAL_OK)
    // {
    //     printf("[error] pid update timer init");
    // }
    // else
    // {
    //     printf("[ok] pid update timer init");
    // }
    // __HAL_TIM_SET_PRESCALER(pid->tim, SystemCoreClock / pid->frequency - 1);
    // __HAL_TIM_SetAutoreload(pid->tim, pid->dt * pid->frequency - 1);
    PID_Init(&pid_velocity);

    pid_velocity.integral_max = 4000;
    pid_velocity.integral_min = -4000;
    pid_velocity.output_max = 4000;
    pid_velocity.output_min = -4000;
    pid_velocity.Kp = 4000;
    pid_velocity.Ki = 1000;
    pid_velocity.Kd = 0;
    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    while (1)
    {
        // printf("arr = %d\n", __HAL_TIM_GetAutoreload(&pid_htim));
        // delay_ms(1000);

        // pwm.duty = g_var.pwm_duty;
        // pwm_setPWM(&pwm);
        // float vel = motor_getVelocity(&motor);
        // DEBUG_PRINT("vel = %f\n", vel);

        // DEBUG_PRINT("pulses = %d", encoder_getPulses(&encoder));
        // printf("pulses = %d\n", encoder.pulses);
        // float vel = encoder_getVelocity(&encoder);

        // motor_pid_velocity(&motor, 2);
        // DEBUG_PRINT("output = %f", motor.pid_velocity->output);
        // DEBUG_PRINT("now vel = %f", motor.velocity);

        // DEBUG_PRINT("%.3f\n", motor.velocity);
        printf("%.3f, %.3f\n", motor.target_vel, motor.velocity);
        // printf("output = %f\n", motor.pid_velocity->output);

        // // motor_pid_velocity(&motor, 1);
        // delay_ms(pid_velocity.dt);
    }
    return 0;
}

int test_pid_postion(void)
{

    if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    {
        printf("failed to start uart it\n");
    }
    encoder.htim = &encoder_htim;
    encoder.count_period = 0xffff;
    encoder.overflow_count = 0;
    encoder.pulses = 0;

    // = {.htim = &htim8, .count_period = 0xffff, .overflow_count = 0, .pulses = 0}
    pwm.tim = &htim1;
    pwm.channel = TIM_CHANNEL_4;
    pwm.resolution = 4096; // 12bit -> 4096
    pwm.frequency = 10e3;  // Hz
    pwm.tim_psc = pwm_calculatePSC(&pwm);

    tb6612.encoder = &encoder;
    tb6612.pwm = &pwm;
    tb6612.IN1_GPIO_Port = TB6612_AIN1_GPIO_PORT;
    tb6612.IN1_GPIO_Pin = TB6612_AIN1_GPIO_PIN;
    tb6612.IN2_GPIO_Port = TB6612_AIN2_GPIO_PORT;
    tb6612.IN2_GPIO_Pin = TB6612_AIN2_GPIO_PIN;
    tb6612.STBYx_GPIO_Port = TB6612_STBY_GPIO_PORT;
    tb6612.STBYx_GPIO_Pin = TB6612_STBY_GPIO_PIN;

    // motor.pid_velocity = &pid_velocity;
    motor.pid_position = &pid_position;

    motor_linkTB6612(&motor, &tb6612);

    tb6612_init(&tb6612);
    pwm.duty = 0.0;
    pwm_setPWM(&pwm);

    // tb6612_setDirection(&tb6612, 1);
    PID_Init(&pid_position);

    pid_position.Kp = 0.00002;
    pid_position.Ki = 0.002;
    pid_position.Kd = 0;
    while (1)
    {
        // printf("arr = %d\n", __HAL_TIM_GetAutoreload(&pid_htim));
        // delay_ms(1000);

        // pwm.duty = g_var.pwm_duty;
        // pwm_setPWM(&pwm);
        // float vel = motor_getVelocity(&motor);
        // DEBUG_PRINT("vel = %f\n", vel);

        // DEBUG_PRINT("pulses = %d", encoder_getPulses(&encoder));
        // printf("pulses = %d\n", encoder.pulses);
        // float vel = encoder_getVelocity(&encoder);

        // motor_pid_velocity(&motor, 2);
        // DEBUG_PRINT("output = %f", motor.pid_velocity->output);
        // DEBUG_PRINT("now vel = %f", motor.velocity);

        // DEBUG_PRINT("%.3f\n", motor.velocity);
        printf("%.3f, %.3f, %.3f\n", motor.target_postion, motor.position,
               motor_getVelocity(&motor));

        // // motor_pid_velocity(&motor, 1);
        // delay_ms(pid_velocity.dt);
    }
    return 0;
}

int test_motor_velociyLoop(void)
{

    if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    {
        printf("failed to start uart it\n");
    }
    motor.pid = &pid_velocity;
    motor.controllerType = PID_VelocityLoop;
    motor.target = 1;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    motor_init(&motor);

    pid_velocity.Kp = 0.3;
    pid_velocity.Ki = 0.05;
    pid_velocity.Kd = 0;
    // *pid_velocity.pdt = 0.01;
    float dt = 0.01;
    // pid_velocity.dt = dt;
    // motor.pid->dt = dt;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    motor_enable(&motor);
    delay_ms(100);

    while (1)
    {
        // delay_ms(1000);
        // printf("dt = %f\n", motor.pid->dt);
        // printf("dt encoder = %f\n", encoder.pid->dt);

        printf("%d,", __HAL_TIM_GetAutoreload(&pid_update_htim));
        printf("%.3f, %.3f\n", motor.target_vel, motor.velocity);
    }
    return 0;
}

int test_motor_velociyLoop_disableTimer(void)
{

    if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    {
        printf("failed to start uart it\n");
    }
    motor.pid = &pid_velocity;
    motor.controllerType = PID_VelocityLoop;
    motor.target = 1;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    motor_init(&motor);

    pid_velocity.Kp = 0.3;
    pid_velocity.Ki = 0.05;
    pid_velocity.Kd = 0;
    // *pid_velocity.pdt = 0.01;
    // float dt = 0.01;
    // pid_velocity.dt = dt;
    // motor.pid->dt = dt;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    // motor_disable(&motor);

    __HAL_TIM_SET_PRESCALER(&pid_update_htim, 83);
    __HAL_TIM_SetAutoreload(&pid_update_htim, 65535);

    motor_setDirection(&motor, NOT_CLOCKWISE);
    motor_enable(&motor);

    delay_ms(1000);
    while (1)
    {
        // motor_move(&motor, motor.target);

        // uint32_t now_t = GetMicros();
        // // uint32_t now_t = HAL_GetTick();
        // uint32_t delta_t = now_t - last_t;
        // last_t = now_t;

        // printf("delta_t = %f s\n", delta_t * 1e-6);
        // printf("delta_t = %f s\n", motor.pid->dt);

        printf("%f,", motor.pid->dt);
        motor_move(&motor, motor.target);
        // delay_ms(10);
        motor.pid->dt = GetMicrosDelta_T();
        // printf("arr = %d\n", __HAL_TIM_GetAutoreload(&pid_htim));
        // delay_ms(5);

        // printf("dt = %f\n", motor.pid->dt);
        // printf("dt encoder = %f\n", encoder.pid->dt);
        //
        // printf("%d,", __HAL_TIM_GetAutoreload(&pid_update_htim));
        printf("%.3f, %.3f\n", motor.target_vel, motor.velocity);
    }
    return 0;
}

int test_motor_PositionLoop(void)
{

    if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    {
        printf("failed to start uart it\n");
    }
    motor.pid = &pid_position;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_PositionLoop;
    motor.target = 20000;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    motor_init(&motor);

    pid_position.Kp = 0.00002;
    pid_position.Ki = 0.002;
    pid_position.Kd = 0;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    motor_enable(&motor);
    delay_ms(100);

    while (1)
    {
        // delay_ms(1000);
        // printf("dt = %f\n", motor.pid->dt);
        // printf("dt encoder = %f\n", encoder.pid->dt);
        motor.target += 5000;
        delay_ms(1000);

        // printf("%d,", __HAL_TIM_GetAutoreload(&pid_update_htim));
        printf("%.3f, %.3f\n", motor.target, motor.position);
    }
    return 0;
}

int test_encoder_angle(void)
{

    if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    {
        printf("failed to start uart it\n");
    }
    motor.pid = &pid_position;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_PositionLoop;
    motor.target = 20000;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    motor_init(&motor);

    pid_position.Kp = 0.00002;
    pid_position.Ki = 0.002;
    pid_position.Kd = 0;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    motor_enable(&motor);
    delay_ms(100);

    while (1)
    {
        delay_ms(1000);
        // Encoder_AngleUpdate(&encoder);
        // float angle = IncrementalEncoder_GetAngle(&encoder);
        float angle = motor_getAngle(&motor);
        printf("angle = %f\n", angle);
    }
    return 0;
}

int test_motor_AngleLoop(void)
{

    if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    {
        printf("failed to start uart it\n");
    }
    motor.pid = &pid_angle;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_AngleLoop;
    motor.target = 90;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    motor_init(&motor);

    pid_angle.Kp = 0.0015;
    pid_angle.Ki = 0.0008;
    pid_angle.Kd = 0;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    motor_enable(&motor);
    printf("The angleloop is starting!!!\n");
    delay_ms(1000);

    while (1)
    {
        motor.target += 90;
        delay_ms(500);

        printf("%.3f, %.3f\n", motor.target, motor.angle);
    }
    return 0;
}
int test_qam16(void)
{
    if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    {
        printf("failed to start uart it\n");
    }
    motor.pid = &pid_angle;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_AngleLoop;
    motor.target = 90;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    motor_init(&motor);

    pid_angle.Kp = 0.0015;
    pid_angle.Ki = 0.0008;
    pid_angle.Kd = 0;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    motor_enable(&motor);

    qam_init(&qam16);

    printf("The angleloop is starting!!!\n");
    delay_ms(1000);

    while (1)
    {
        qam_loop(&motor, &qam16, 1000);
        printf("%.3f, %.3f\n", motor.target, motor.angle);
    }
    return 0;
}
