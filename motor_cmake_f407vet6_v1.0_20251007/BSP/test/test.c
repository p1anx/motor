#include "test.h"
#include "lis3mdl.h"
#include "motor.h"
#include "magneticSensor.h"
#include "stm32_hal.h"
#include "stm32f4xx_hal.h"
#include "usart_mx.h"
#include <stdio.h>

extern uint8_t rxBuffer;
extern uint8_t rx;
extern int gKey_flag;

extern GlobalVar_t g_var;

LIS3MDL_t lis3mdl;
Encoder_t encoder;
PIDController_t pid_velocity;
PIDController_t pid_position;
PIDController_t pid_magnetic;
PIDController_t pid_angle;
Motor_t motor;
TB6612_t tb6612;
PWM_t pwm;
QAM_t qam16;
QAM_t qam;

void test_main(void)
{
    printf("[ok] start stm32\n");
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
    // test_motor_PositionLoop_0();
    // 3.
    test_motor_PositionLoop_1();
    // test_encoder_angle();
    // test_motor_AngleLoop();
    // test_qam16();
    // 1.
    // test_motor_calibrate();
    // test_communication_with_python();
    // 2.
    // test_magneticLoop();
    // test_lis3mdl();
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
    LIS3MDL_Init();

    while (1)
    {
        lis3mdl = motor_getMagneticGauss();
        printf("%f,%f,%f\n", lis3mdl.data_x, lis3mdl.data_y, lis3mdl.data_z);
        // LIS3MDL_DataRead();
        // delay_ms(1000);
        // printf("dt = %f\n", motor.pid->dt);
        // printf("dt encoder = %f\n", encoder.pid->dt);

        // printf("%d,", __HAL_TIM_GetAutoreload(&pid_update_htim));
        // printf("%.3f, %.3f\n", motor.target_vel, motor.velocity);
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

    // 1.
    //  pid_position.Kp = 0.00002;
    //  pid_position.Ki = 0.002;
    //  pid_position.Kd = 0;
    // 2.
    pid_position.Kp = 0.00003;
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
        motor.target += 500;
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

    delay_ms(1000);
    motor_disable(&motor);
    printf("The angleloop is starting!!!\n");
    printf("Please enable motor\n");

    while (1)
    {
        if (gKey_flag == 1)
        {
            motor_enable(&motor);
            qam_loop(&motor, &qam16, 1000);
            printf("%.3f, %.3f\n", motor.target, motor.angle);
        }
    }
    return 0;
}

int test_motor_PositionLoop_0(void)
{

    if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    {
        printf("failed to start uart it\n");
    }
    motor.pid = &pid_position;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_PositionLoop;
    motor.target = 00000;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    motor_init(&motor);

    // 1.
    //  pid_position.Kp = 0.00002;
    //  pid_position.Ki = 0.002;
    //  pid_position.Kd = 0;
    // 2.
    pid_position.Kp = 0.0002f;
    pid_position.Ki = 0.0005f;
    pid_position.Kd = 0;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    motor_enable(&motor);
    delay_ms(100);
    motor_disable(&motor);

    int last_t = 0;
    while (1)
    {
        // delay_ms(1000);
        // printf("dt = %f\n", motor.pid->dt);
        // printf("dt encoder = %f\n", encoder.pid->dt);
    }

    // printf("%d,", __HAL_TIM_GetAutoreload(&pid_update_htim));
    printf("%.3f, %.3f\n", motor.target, motor.position);
    if (gKey_flag == 1)
    {
        motor_enable(&motor);
        int current_t = HAL_GetTick();
        int delta_t = current_t - last_t;
        if (delta_t > 10000)
        {
            motor.target += 500;
            last_t = current_t;
        }
    }
    else
    {
        motor_disable(&motor);
    }
    return 0;
}
void test_motor_calibrate(void)
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
    LIS3MDL_Init();

    while (1)
    {
        // lis3mdl = motor_getMagneticGauss();
        // printf("%f,%f,%f\n", lis3mdl.data_x, lis3mdl.data_y, lis3mdl.data_z);
        LIS3MDL_DataRead();
        // delay_ms(1000);
        // printf("dt = %f\n", motor.pid->dt);
        // printf("dt encoder = %f\n", encoder.pid->dt);

        // printf("%d,", __HAL_TIM_GetAutoreload(&pid_update_htim));
        // printf("%.3f, %.3f\n", motor.target_vel, motor.velocity);
    }
}
void test_communication_with_python(void)
{
    if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    {
        printf("failed to start uart it\n");
    }
    motor.pid = &pid_position;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_PositionLoop;
    motor.target = 00000;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    motor_init(&motor);

    // 1.
    //  pid_position.Kp = 0.00002;
    //  pid_position.Ki = 0.002;
    //  pid_position.Kd = 0;
    // 2.
    pid_position.Kp = 0.0002f;
    pid_position.Ki = 0.0005f;
    pid_position.Kd = 0;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    motor_enable(&motor);
    delay_ms(100);
    motor_disable(&motor);

    while (1)
    {

        float B = MagneticSenor_getValue();
        printf("B = %f\n", B);
        delay_ms(1000);
    }
}

void test_magneticLoop(void)
{
    if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    {
        printf("failed to start uart it\n");
    }
    motor.pid = &pid_magnetic;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_MagneticLoop;
    motor.target = 1.5f;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    motor_init(&motor);

    // 1.
    //  pid_position.Kp = 0.00002;
    //  pid_position.Ki = 0.002;
    //  pid_position.Kd = 0;
    // 2.
    // pid_position.Kp = 0.0002f;
    // pid_position.Ki = 0.0005f;
    // pid_position.Kd = 0;
    // 3.
    pid_magnetic.Kp = 0.2f;
    pid_magnetic.Ki = 0.1f;
    pid_magnetic.Kd = 0.000f;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    motor_enable(&motor);
    delay_ms(100);
    motor_disable(&motor);
    printf("starting motor\n");
    printf("please enable motor\n");
    LIS3MDL_Init();
    int last_t = 0;
    int direction = 0;

    while (1)
    {
        // printf("%f\n", motor.magnetic);
        float position = motor_getPosition(&motor);
        printf("%f,%f\n", motor.magnetic, position);
        if (gKey_flag == 1)
        {
            // printf("motor is enabled\n");
            motor_enable(&motor);
            // motor.target += 1.1;
            float position = motor_getPosition(&motor);
            printf("%f,%f\n", motor.magnetic, position);
            // 1.
            //  int current_t = HAL_GetTick();
            //  int delta_t = current_t - last_t;
            //  if (delta_t > 2000)
            //  {
            //      if (direction)
            //      {
            //          motor.target += 1.1;
            //      }
            //      else
            //      {
            //          motor.target -= 1.1;
            //      }
            //      if (motor.target > 4)
            //      {
            //          direction = 1;
            //      }
            //      if (motor.target < -3.0f)
            //      {
            //          direction = 0;
            //      }
            //      last_t = current_t;
            //  }

            // 2.
            //  float B = MagneticSenor_getValue();
            //  printf("B = %f\n", motor.magnetic);
            //  printf("%f,%f\n", motor.magnetic, motor.pid->output);
            //  printf("output = %f\n", motor.pid->output);
            //  delay_ms(1000);
        }
        else
        {
            motor_disable(&motor);
        }

        // float B = MagneticSenor_getValue();
        // printf("B = %f\n", B);
        // delay_ms(1000);
    }
}
void test_lis3mdl(void)
{
    LIS3MDL_Init();
    while (1)
    {
        LIS3MDL_DataRead();
    }
}

int test_motor_PositionLoop_1(void)
{

    if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    {
        printf("failed to start uart it\n");
    }
    motor.pid = &pid_position;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_PositionLoop;
    motor.target = 00000;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    motor_init(&motor);

    // 1.
    //  pid_position.Kp = 0.00002;
    //  pid_position.Ki = 0.002;
    //  pid_position.Kd = 0;
    // 2.
    pid_position.Kp = 0.0002f;
    pid_position.Ki = 0.0002f;
    pid_position.Kd = 0;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    motor_enable(&motor);
    delay_ms(100);
    motor_disable(&motor);
    // float position_target[] = {1716, 849, 988, 728, 862, 925, 1187, 2745};
    // float position_target[] = {1716, 849,  988, 728, 862, 925, 1187, 2745,
    //                            2745, 1187, 925, 862, 728, 988, 849,  1716};
    float position_target[] = {2887.3125, 1103.3125, 787.3125,  806.3125,  718.3125, 968.3125,
                               963.3125,  1863.3125, 2643.3125, 1103.3125, 808.3125, 807.3125,
                               866.3125,  821.3125,  1062.3125, 1790.3125};
    int i = 0;

    int last_t = 0;
    LIS3MDL_Init();
    while (1)
    {
        // delay_ms(1000);
        // printf("dt = %f\n", motor.pid->dt);
        // printf("dt encoder = %f\n", encoder.pid->dt);

        // printf("%d,", __HAL_TIM_GetAutoreload(&pid_update_htim));
        LIS3MDL_DataRead();
        // printf("%.3f, %.3f\n", motor.target, motor.position);
        if (gKey_flag == 1)
        {
            motor_enable(&motor);
            int current_t = HAL_GetTick();
            int delta_t = current_t - last_t;
            if (delta_t > 1000)
            {
                motor.target += position_target[i++];
                if (i == 17)
                {
                    i = 0;
                }

                last_t = current_t;
            }
        }
        else
        {
            motor_disable(&motor);
        }
    }
    return 0;
}