//
// Created by xwj on 10/22/25.
//

#include "esp8266.h"
#include "lis3mdl.h"
#include "motor.h"
#include "stm32_hal.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "uart_it.h"

extern uint8_t rxBuffer;
extern uint8_t rx;
extern int gKey_flag;
extern char str[RXBUFFER];

extern LIS3MDL_t lis3mdl;
extern Encoder_t encoder;
extern PIDController_t pid_velocity;
extern PIDController_t pid_position;
extern PIDController_t pid_magnetic;
extern PIDController_t pid_angle;
extern Motor_t motor;
extern TB6612_t tb6612;
extern PWM_t pwm;
extern ESP8266_HandleTypeDef g_esp8266;
extern int sync_flag;
extern int t_ms;
extern float angle;
extern float qam4_list[4];
extern float qam16_list[16];

static int test_motor_AngleLoop_wifi_uart(void)
{
    if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    {
        printf("failed to start uart it\n");
    }
    motor.pid = &pid_position;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_AngleLoop;
    motor.target = 0.0f;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    motor_init(&motor);

    // 1.
    pid_position.Kp = 0.015f;
    pid_position.Ki = 0.000f;
    pid_position.Kd = 0.001f;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);

    // wifi init
    ESP8266_ClientInit(&huart3);

    HAL_UART_Receive_IT(g_esp8266.huart, &rx, 1);

    // motor status of init
    // motor_enable(&motor);
    motor_disable(&motor);
    if (motor.isEnable == 0)
    {
        printf("[INFO] motor is disabled, please enable it\n");
    }
    else
    {
        printf("[INFO] motor is enabled\n");
    }
    int tx_isFirst = 1;
    char *start_flag = "start";
    char *enalbe_wifi = "Hello";
    char *from_tcp = "from_tcp";
    delay_ms(10);
    int last_t = 0;
    printf("[INFO] Waiting Sync\n");
    // const float qam4_angle[4] = {32.0f, 149.0f, 214.6f, 334.6f};
    const float qam4_angle[4] = {29.0f, 144.0f, 212.4f, 334.4f};
    int qam4_index = 0;

    while (1)
    {
        if (sync_flag == 1)
        {
                motor_enable(&motor);

                if (motor.target > 360)
                {
                    motor.target -= 360;
                }
                int current_t = HAL_GetTick();
                if (current_t - last_t > t_ms)
                {
                    printf("%.2f, %.2f\n", motor.target,motor.angle);
                    ESP8266_PrintDataFloat(2, motor.target, motor.angle);
                    // motor.target += angle;
                    // motor.target = qam4_angle[qam4_index++];
                    //1.qam4
                    // motor.target = qam4_list[qam4_index++];
                    // if (qam4_index == 4)
                    // {
                    //     qam4_index = 0;
                    // }
                    //2.qam16
                    motor.target = qam16_list[qam4_index++];
                    if (qam4_index == 16)
                    {
                        qam4_index = 0;
                    }
                    last_t = current_t;
                }
        }
    }
    return 0;
}

// it's ok
static int test_motor_AngleLoop_calibration_amplitude(void)
{
    if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    {
        printf("failed to start uart it\n");
    }
    motor.pid = &pid_position;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_AngleLoop;
    motor.target = 0.0f;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    motor_init(&motor);

    // 1.
    pid_position.Kp = 0.015f;
    pid_position.Ki = 0.000f;
    pid_position.Kd = 0.001f;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);

    // wifi init
    ESP8266_ClientInit(&huart3);

    HAL_UART_Receive_IT(g_esp8266.huart, &rx, 1);

    // motor status of init
    // motor_enable(&motor);
    motor_disable(&motor);
    if (motor.isEnable == 0)
    {
        printf("[INFO] motor is disabled, please enable it\n");
    }
    else
    {
        printf("[INFO] motor is enabled\n");
    }
    int tx_isFirst = 1;
    char *start_flag = "start";
    char *enalbe_wifi = "Hello";
    char *from_tcp = "from_tcp";
    delay_ms(10);
    int last_t = 0;
    printf("[INFO] Waiting Sync\n");

    while (1)
    {
        if (sync_flag == 1)
        {
            motor_enable(&motor);

            if (motor.target > 360)
            {
                // motor_disable(&motor);
                motor.target -= 360;
            }
            int current_t = HAL_GetTick();
            if (current_t - last_t > t_ms)
            {
                printf("%.2f, %.2f\n", motor.target,motor.angle);
                ESP8266_PrintDataFloat(2, motor.target, motor.angle);
                motor.target += angle;
                last_t = current_t;
            }
        }
    }
    return 0;
}
static int test_motor_AngleLoop_wifi(void)
{
    if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    {
        printf("failed to start uart it\n");
    }
    motor.pid = &pid_position;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_AngleLoop;
    motor.target = 0.0f;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    motor_init(&motor);

    // 1.
    pid_position.Kp = 0.015f;
    pid_position.Ki = 0.000f;
    pid_position.Kd = 0.001f;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);

    // wifi init
    ESP8266_ClientInit(&huart3);

    HAL_UART_Receive_IT(g_esp8266.huart, &rx, 1);

    // motor
    // motor_enable(&motor);
    motor_disable(&motor);
    if (motor.isEnable == 0)
    {
        printf("[INFO] motor is disabled, please enable it\n");
    }
    else
    {
        printf("[INFO] motor is enabled\n");
    }
    int tx_isFirst = 1;
    char *start_flag = "start";
    delay_ms(10);
    int last_t = 0;

    while (1)
    {
        // if (motor.isEnable)
        if (1)
        {
            // motor_enable(&motor);

            // if (tx_isFirst)
            // {
            //     tx_isFirst = 0;
            //     HAL_Delay(10);
            //     printf("test\n");
            //     printf("%s\n", start_flag);
            // }
            if (motor.target > 360)
            {
                // motor_disable(&motor);
                motor.target -= 360;
            }
            int current_t = HAL_GetTick();
            if (current_t - last_t > 5000)
            {
                printf("%.2f\n", motor.target);
                // ESP8266_PrintDataFloat(1, motor.angle);
                ESP8266_PrintDataFloat(1, motor.target);
                motor.target += 4;
                last_t = current_t;
            }
        }
    }
    return 0;
}

static int test_motor_AngleLoop0(void)
{
    if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    {
        printf("failed to start uart it\n");
    }
    motor.pid = &pid_position;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_AngleLoop;
    motor.target = 0.0f;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    motor_init(&motor);

    // 1.
    //  pid_position.Kp = 0.00002;
    //  pid_position.Ki = 0.002;
    //  pid_position.Kd = 0;
    // 2.
    pid_position.Kp = 0.015f;
    pid_position.Ki = 0.000f;
    pid_position.Kd = 0.001f;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    ESP8266_ClientInit(&huart3);
    HAL_UART_Receive_IT(g_esp8266.huart, &rx, 1);
    // motor_enable(&motor);
    motor_disable(&motor);
    if (motor.isEnable == 0)
    {
        printf("please enable motor\n");
    }
    int tx_isFirst = 1;
    char *start_flag = "start";
    delay_ms(10);
    int last_t = 0;

    while (1)
    {
        if (motor.isEnable)
        {
            motor_enable(&motor);
            // if (tx_isFirst)
            // {
            //     tx_isFirst = 0;
            //     HAL_Delay(10);
            //     printf("test\n");
            //     printf("%s\n", start_flag);
            // }
            if (motor.target > 360)
            {
                // motor_disable(&motor);
                motor.target -= 360;
            }
            int current_t = HAL_GetTick();
            if (current_t - last_t > 5000)
            {
                // printf("%.2f\n", motor.angle);
            ESP8266_PrintDataFloat(1, motor.angle);
                motor.target += 4;
                last_t = current_t;
            }
        }
    }
    return 0;
}

void test_motor(void)
{
    printf("[INFO] Testing Motor!!!\n");
    //1.
    // Example_ESP8266_0();
    //2.
    // test_motor_AngleLoop0();
    //3.
    // test_motor_AngleLoop_wifi();
    //4.
    test_motor_AngleLoop_wifi_uart();
    //5.
    // test_motor_AngleLoop_calibration_amplitude();
}
