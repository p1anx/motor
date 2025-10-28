//
// Created by xwj on 10/15/25.
//
#include "motor.h"
#include "mt6835.h"
#include "eeprom.h"
#include "uart/uart.h"

extern Motor_t motor;
extern Encoder_t encoder;
extern int gKey_flag_0;
extern int gKey_flag_1;
PIDController_t pid_velocity;
PIDController_t pid_position;
PIDController_t pid;
TB6612_t tb6612;
PWM_t pwm;

int test_motor_velociyLoop(void);
void test_check(void);
int test_motor_PositionLoop(void);
int test_motor_PositionLoop_key(void);
int test_motor_PositionLoop_with_mt(void);
int test_motor_AngleLoop_with_mt(void);
int test_motor_AngleLoop(void);
int test_motor_PositionLoop_lis3mdl(void);
int test_motor_AngleLoop_toPython(void);
int test_motor_AngleLoop_toEqualAmp(void);
int test_motor_AngleLoop_calc(void);
int test_motor_PositionLoop_calc(void);
int test_motor_openLoop(void);
#include "main.h"
#include "string.h"

void test_motor(void)
{
    // 1.
    // test_motor_velociyLoop();
    //  test_check();
    // 2.
    // test_motor_PositionLoop();
    // 3.
    //  test_motor_PositionLoop_key();
    // 4.
    //  test_motor_PositionLoop_with_mt();
    // 5.
    //  test_motor_AngleLoop_with_mt();
    // 6.
    //  test_motor_PositionLoop_lis3mdl();
    // 7.
    test_motor_AngleLoop();
    // 8.
    // test_motor_AngleLoop_toPython();
    // motor_AngleLoop_toPython();
    //9.
    // test_motor_AngleLoop_toEqualAmp();
    //10.
    // test_motor_AngleLoop_calc();
    //11.
    // test_motor_PositionLoop_calc();
    //12.
    // test_motor_openLoop();
}
int test_motor_PositionLoop_calc(void)
{

    // if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    // {
    //     printf("failed to start uart it\n");
    // }
    motor.pid = &pid_position;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_PositionLoop;
    motor.target = 10000;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    motor_init(&motor);

    // 1.
    //  pid_position.Kp = 0.00002;
    //  pid_position.Ki = 0.002;
    //  pid_position.Kd = 0;
    // 2.
    pid_position.Kp = 0.0009f; // 0.0019 sinx
    pid_position.Ki = 0.003f;
    pid_position.Kd = 0.00008;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    motor_enable(&motor);
    delay_ms(100);

    while (1)
    {
        // delay_ms(1000);
        // printf("dt = %f\n", motor.pid->dt);
        // printf("dt encoder = %f\n", encoder.pid->dt);
        static int count = 0;
        if (count++ > 10000)
        {
            count = 0;
            // motor.target += 500;
        }

        delay_ms(10);

        // printf("%d,", __HAL_TIM_GetAutoreload(&pid_update_htim));
        // printf("%.3f, %.3f\n", motor.target, motor.position);
    }
    return 0;
}
int test_motor_AngleLoop_calc(void)
{

    motor.pid = &pid_position;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_AngleLoop;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    // 1.
    //  motor_init(&motor);
    // 2.
    motor_init_with_lis3mdl(&motor);

    // 1.
    //  pid_position.Kp = 0.00002;
    //  pid_position.Ki = 0.002;
    //  pid_position.Kd = 0;
    // 2.
    // pid_position.Kp = 0.0002;
    // pid_position.Ki = 0.002;
    // pid_position.Kd = 0;
    pid_position.Kp = 0.02f; // 0.0019 sinx
    pid_position.Ki = 0.000f;
    pid_position.Kd = 0.001;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    // LIS3MDL_Init();
    motor.target = 180.0f;

    // motor_disable(&motor);
    motor_enable(&motor);
    delay_ms(100);

    int tx_flag = 1;
    int start_flag = 0;
    while (1)
    {
        if (gKey_flag_0 == 1)
        {
            motor_enable(&motor);
            static int count = 0;
            // printf("start\n");
            if (tx_flag == 1)
            {
                start_flag = 1;
                tx_flag = 0;
                printf("start\n");
            }
            if (count++ > 100)
            {
                count = 0;
                if (motor.target == 360.0 || motor.target > 360.0)
                {
                    // motor.target = motor.target - 360.0;
                    motor.target = motor.target;
                    printf("end\n");
                }
                else
                {
                    motor.target += 2;
                }
                // printf("%.3f,%.3f,%.3f,%.3f\n", motor.lis3mdl.data_x, motor.lis3mdl.data_y,
                //        motor.lis3mdl.data_z, motor.angle);
            }
        }
        else if (gKey_flag_0 == 0)
        {
            // motor_disable(&motor);
        }
        LIS3MDL_DataUpdate(&motor.lis3mdl);

        delay_ms(10);

        // 1.
        printf("%.3f,%.3f,%.3f,%.3f,%.3f\n", motor.target, motor.angle, motor.lis3mdl.data_x,
               motor.lis3mdl.data_y, motor.pid->output);
        //  2.
        //  if (start_flag == 1) {
        //      start_flag = 0;
        //      printf("start\n");
        //      delay_ms(1000);
        //  }
        //  printf("%.3f,%.3f,%.3f,%.3f\n", motor.lis3mdl.data_x,
        //  motor.lis3mdl.data_y,motor.lis3mdl.data_z, motor.position);
    }
    return 0;
}

int test_motor_AngleLoop_toEqualAmp(void)
{
    motor.pid = &pid_position;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_AngleLoop;
    motor.target = 0.0f;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    // 1.
    //  motor_init(&motor);
    // 2.
    motor_init_with_lis3mdl(&motor);

    // 1.
    //  pid_position.Kp = 0.00002;
    //  pid_position.Ki = 0.002;
    //  pid_position.Kd = 0;
    // 2.
    // pid_position.Kp = 0.0002;
    // pid_position.Ki = 0.002;
    // pid_position.Kd = 0;
    pid_position.Kp = 0.015f; // 0.0019 sinx
    pid_position.Ki = 0.0f;
    pid_position.Kd = 0.001;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    // LIS3MDL_Init();

    motor_disable(&motor);
    // motor_enable(&motor);
    delay_ms(100);

    int tx_flag = 1;
    int start_flag = 0;
    int buffer_i = 0;
    int target_buffer[] = {2, 20, 36, 54, 76, 110, 167, 188, 204, 220, 235, 256, 282, 341};
    while (1)
    {
        if (gKey_flag_0 == 1)
        {
            motor_enable(&motor);
            static int count = 0;
            // printf("start\n");
            if (tx_flag == 1)
            {
                start_flag = 1;
                tx_flag = 0;
                printf("start\n");
            }
            if (count++ > 100)
            {
                count = 0;
                if (motor.target == 360.0 || motor.target > 360.0)
                {
                    motor.target = motor.target - 360.0;
                }
                else
                {
                    motor.target = (float)target_buffer[buffer_i++];
                }
                if (buffer_i == sizeof(target_buffer)/sizeof(target_buffer[0]))
                {
                   buffer_i = 0;
                }
                // printf("%.3f,%.3f,%.3f,%.3f\n", motor.lis3mdl.data_x, motor.lis3mdl.data_y,
                //        motor.lis3mdl.data_z, motor.angle);
            }
        }
        else if (gKey_flag_0 == 0)
        {
            motor_disable(&motor);
        }
        LIS3MDL_DataUpdate(&motor.lis3mdl);

        delay_ms(10);

        // 1.
        printf("%.3f,%.3f,%.3f,%.3f,%.3f\n", motor.target, motor.angle, motor.lis3mdl.data_x,
               motor.lis3mdl.data_y, motor.lis3mdl.data_z);
        //  2.
        //  if (start_flag == 1) {
        //      start_flag = 0;
        //      printf("start\n");
        //      delay_ms(1000);
        //  }
        //  printf("%.3f,%.3f,%.3f,%.3f\n", motor.lis3mdl.data_x,
        //  motor.lis3mdl.data_y,motor.lis3mdl.data_z, motor.position);
    }
    return 0;
}
int test_motor_AngleLoop_toPython(void)
{

    motor.pid = &pid_position;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_AngleLoop;
    motor.target = 0.0f;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    // 1.
    //  motor_init(&motor);
    // 2.
    motor_init_with_lis3mdl(&motor);

    // 1.
    //  pid_position.Kp = 0.00002;
    //  pid_position.Ki = 0.002;
    //  pid_position.Kd = 0;
    // 2.
    // pid_position.Kp = 0.0002;
    // pid_position.Ki = 0.002;
    // pid_position.Kd = 0;
    pid_position.Kp = 0.015f; // 0.0019 sinx
    pid_position.Ki = 0.0f;
    pid_position.Kd = 0.001;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    // LIS3MDL_Init();

    motor_disable(&motor);
    // motor_enable(&motor);
    delay_ms(100);

    int tx_flag = 1;
    int start_flag = 0;
    while (1)
    {
        if (gKey_flag_0 == 1)
        {
            motor_enable(&motor);
            static int count = 0;
            // printf("start\n");
            if (tx_flag == 1)
            {
                start_flag = 1;
                tx_flag = 0;
                printf("start\n");
            }
            if (count++ > 100)
            {
                count = 0;
                if (motor.target == 360.0 || motor.target > 360.0)
                {
                    // motor.target = motor.target - 360.0;
                    motor.target = motor.target;
                    printf("end\n");
                }
                else
                {
                    motor.target += 2;
                }
                printf("%.3f,%.3f,%.3f,%.3f\n", motor.lis3mdl.data_x, motor.lis3mdl.data_y,
                       motor.lis3mdl.data_z, motor.angle);
            }
        }
        else if (gKey_flag_0 == 0)
        {
            motor_disable(&motor);
        }
        LIS3MDL_DataUpdate(&motor.lis3mdl);

        delay_ms(1);

        // 1.
        //  printf("%.3f,%.3f,%.3f,%.3f,%.3f\n", motor.target, motor.angle, motor.lis3mdl.data_x,
        //         motor.lis3mdl.data_y, motor.lis3mdl.data_z);
        //  2.
        //  if (start_flag == 1) {
        //      start_flag = 0;
        //      printf("start\n");
        //      delay_ms(1000);
        //  }
        //  printf("%.3f,%.3f,%.3f,%.3f\n", motor.lis3mdl.data_x,
        //  motor.lis3mdl.data_y,motor.lis3mdl.data_z, motor.position);
    }
    return 0;
}
int test_motor_AngleLoop(void)
{

    // if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    // {
    //     printf("failed to start uart it\n");
    // }
    motor.pid = &pid_position;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_AngleLoop;
    motor.target = 90.0f;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    // 1.
    //  motor_init(&motor);
    // 2.
    motor_init_with_lis3mdl(&motor);

    // 1.
    //  pid_position.Kp = 0.00002;
    //  pid_position.Ki = 0.002;
    //  pid_position.Kd = 0;
    // 2.
    // pid_position.Kp = 0.0002;
    // pid_position.Ki = 0.002;
    // pid_position.Kd = 0;
    pid_position.Kp = 0.015f; // 0.0019 sinx
    pid_position.Ki = 0.0f;
    pid_position.Kd = 0.001;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    // LIS3MDL_Init();

    motor_disable(&motor);
    // motor_enable(&motor);
    delay_ms(100);

    int tx_flag = 1;
    int start_flag = 0;
    while (1)
    {
        // delay_ms(1000);
        // printf("dt = %f\n", motor.pid->dt);
        // printf("dt encoder = %f\n", encoder.pid->dt);
        //
        if (gKey_flag_0 == 1)
        {
            motor_enable(&motor);
            static int count = 0;
            if (tx_flag == 1)
            {
                start_flag = 1;
                tx_flag = 0;
            }
            if (start_flag == 1)
            {
                start_flag = 0;
                // printf("start\n");
                // delay_ms(10000);
            }
            if (count++ > 100)
            {
                count = 0;
                if (motor.target > 360.0)
                {
                    motor.target = motor.target - 360.0;
                }
                motor.target += 90;
                // printf("%.3f,%.3f,%.3f,%.3f\n", motor.lis3mdl.data_x,
                // motor.lis3mdl.data_y,motor.lis3mdl.data_z, motor.position);
            }
        }
        else if (gKey_flag_0 == 0)
        {
            motor_disable(&motor);
        }
        LIS3MDL_DataUpdate(&motor.lis3mdl);

        delay_ms(10);

        // printf("%d,", __HAL_TIM_GetAutoreload(&pid_update_htim));
        printf("%.3f,%.3f,%.3f,%.3f,%.3f\n", motor.target, motor.angle, motor.lis3mdl.data_x,
               motor.lis3mdl.data_y, motor.lis3mdl.data_z);
        // 2.
        //  if (start_flag == 1) {
        //      start_flag = 0;
        //      printf("start\n");
        //      // delay_ms(10000);
        //  }
        //  printf("%.3f,%.3f,%.3f,%.3f\n", motor.lis3mdl.data_x,
        //  motor.lis3mdl.data_y,motor.lis3mdl.data_z, motor.position);
    }
    return 0;
}
int test_motor_PositionLoop_lis3mdl(void)
{

    // if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    // {
    //     printf("failed to start uart it\n");
    // }
    motor.pid = &pid_position;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_PositionLoop;
    motor.target = 0.0f;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    // 1.
    //  motor_init(&motor);
    // 2.
    motor_init_with_lis3mdl(&motor);

    // 1.
    //  pid_position.Kp = 0.00002;
    //  pid_position.Ki = 0.002;
    //  pid_position.Kd = 0;
    // 2.
    // pid_position.Kp = 0.0002;
    // pid_position.Ki = 0.002;
    // pid_position.Kd = 0;
    pid_position.Kp = 0.0009f; // 0.0019 sinx
    pid_position.Ki = 0.003f;
    pid_position.Kd = 0.00008;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    // LIS3MDL_Init();
    motor_disable(&motor);
    delay_ms(100);

    int tx_flag = 1;
    int start_flag = 0;
    while (1)
    {
        // delay_ms(1000);
        // printf("dt = %f\n", motor.pid->dt);
        // printf("dt encoder = %f\n", encoder.pid->dt);
        //
        if (gKey_flag_0 == 1)
        {
            motor_enable(&motor);
            static int count = 0;
            if (tx_flag == 1)
            {
                start_flag = 1;
                tx_flag = 0;
            }
            if (start_flag == 1)
            {
                start_flag = 0;
                // printf("start\n");
                // delay_ms(10000);
            }
            if (count++ > 100)
            {
                count = 0;
                motor.target += 5000;
                // printf("%.3f,%.3f,%.3f,%.3f\n", motor.lis3mdl.data_x,
                // motor.lis3mdl.data_y,motor.lis3mdl.data_z, motor.position);
            }
        }
        else if (gKey_flag_0 == 0)
        {
            motor_disable(&motor);
        }
        LIS3MDL_DataUpdate(&motor.lis3mdl);

        delay_ms(10);

        // printf("%d,", __HAL_TIM_GetAutoreload(&pid_update_htim));
        printf("%.3f, %.3f, %.3f,%.3f,%.3f\n", motor.target, motor.position, motor.lis3mdl.data_x,
               motor.lis3mdl.data_y, motor.lis3mdl.data_z);
        // 2.
        //  if (start_flag == 1) {
        //      start_flag = 0;
        //      printf("start\n");
        //      // delay_ms(10000);
        //  }
        //  printf("%.3f,%.3f,%.3f,%.3f\n", motor.lis3mdl.data_x,
        //  motor.lis3mdl.data_y,motor.lis3mdl.data_z, motor.position);
    }
    return 0;
}

MT6835_Handle mt6835; // 编码器句柄
int test_motor_AngleLoop_with_mt(void)
{

    // if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    // {
    //     printf("failed to start uart it\n");
    // }
    motor.pid = &pid;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_AngleLoop;

    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    // motor_init(&motor);
    motor_init_with_last(&motor);

    // 1.
    //  pid_position.Kp = 0.00002;
    //  pid_position.Ki = 0.002;
    //  pid_position.Kd = 0;
    // 2.
    pid.Kp = 0.0015f;
    pid.Ki = 0.0005f;
    // pid.Ki = 0.0000f;
    pid.Kd = 0;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    // motor_disable(&motor);
    motor_enable(&motor);

    // 初始化编码器
    // motor_linkMT6835(&motor, mt6835);
    delay_ms(100);
    float motor_angle = motor.last_angle;

    motor.target = motor.last_angle;
    // float last_saved_angle = 0.0f;
    // eeprom_ReadFloat(0, &last_saved_angle);
    int last_t = 0;
    while (1)
    {
        // delay_ms(1000);
        // printf("dt = %f\n", motor.pid->dt);
        // printf("dt encoder = %f\n", encoder.pid->dt);
        //
        // 1.
        // if (gKey_flag_0 == 1) {
        //     motor_enable(&motor);
        //     static  int count = 0;
        //     if (count++ > 100) {
        //         count = 0;
        //         motor.target += 90;
        //     }
        //     MT6835_Angle_Update_with_last(&mt6835);
        //     motor_angle = MT6835_GetMotorAngleDegrees(&mt6835);
        //
        //
        // }
        // else if (gKey_flag_0 == 0) {
        //     motor_disable(&motor);
        //     eeprom_WriteFloat(0, motor_angle);
        // }
        // 2.
        // if (gKey_flag_0 == 1) {
        //     eeprom_WriteFloat(EEPROM_LAST_ANGLE_ADDR,motor.angle);
        //     printf("angle is saved\n");
        // }
        // 3.
        // MT6835_Angle_Update(&motor.mt6835);
        // float motor_anlge = motor_getAngle_mt(&motor);

        // 4.

        motor_move(&motor, motor.target);
        if (gKey_flag_0 == 1)
        {
            motor.target += 90.0f;
            gKey_flag_0 = 0;
        }
        if (gKey_flag_1 == 1)
        {
            eeprom_WriteFloat(EEPROM_LAST_ANGLE_ADDR, motor.angle);
            gKey_flag_1 = 0;
        }

        // printf("%d,", __HAL_TIM_GetAutoreload(&pid_update_htim));
        // printf("%.3f, %.3f, %.3f\n", motor.target, motor.angle, motor_angle);
        printf("%.3f, %f\n", motor.last_angle, motor.angle);
        delay_ms(10);
    }
    return 0;
}
int test_motor_PositionLoop_with_mt(void)
{

    // if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    // {
    //     printf("failed to start uart it\n");
    // }
    motor.pid = &pid_position;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_PositionLoop;
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
    pid_position.Kp = 0.0002;
    pid_position.Ki = 0.002;
    pid_position.Kd = 0;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    motor_disable(&motor);

    MT6835_Handle mt6835; // 编码器句柄

    // 初始化编码器
    MT6835_Init(&mt6835, &mt6835_spi, MT6835_SPI_CS_PORT, MT6835_SPI_CS_PIN);
    delay_ms(100);
    float motor_angle = 0.0f;
    float last_saved_angle = 0.0f;
    eeprom_ReadFloat(0, &last_saved_angle);
    while (1)
    {
        // delay_ms(1000);
        // printf("dt = %f\n", motor.pid->dt);
        // printf("dt encoder = %f\n", encoder.pid->dt);
        //
        if (gKey_flag_0 == 1)
        {
            motor_enable(&motor);
            static int count = 0;
            if (count++ > 100)
            {
                count = 0;
                motor.target += 800;
            }
            MT6835_Angle_Update(&mt6835);
            motor_angle = MT6835_GetMotorAngleDegrees(&mt6835);
        }
        else if (gKey_flag_0 == 0)
        {
            motor_disable(&motor);
            eeprom_WriteFloat(0, motor_angle);
        }

        delay_ms(10);

        // printf("%d,", __HAL_TIM_GetAutoreload(&pid_update_htim));
        printf("%.3f, %.3f, %.3f\n", motor.target, motor.position, motor_angle);
        printf("last_saved_angle = %.3f\n", last_saved_angle);
    }
    return 0;
}
int test_motor_PositionLoop_key(void)
{

    // if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    // {
    //     printf("failed to start uart it\n");
    // }
    motor.pid = &pid_position;
    // motor.pid_position = &pid_position;
    motor.controllerType = PID_PositionLoop;
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
    pid_position.Kp = 0.0002;
    pid_position.Ki = 0.002;
    pid_position.Kd = 0;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    motor_disable(&motor);
    delay_ms(100);

    while (1)
    {
        // delay_ms(1000);
        // printf("dt = %f\n", motor.pid->dt);
        // printf("dt encoder = %f\n", encoder.pid->dt);
        //
        if (gKey_flag_0 == 1)
        {
            motor_enable(&motor);
            static int count = 0;
            if (count++ > 100)
            {
                count = 0;
                motor.target += 500;
            }
        }
        else if (gKey_flag_0 == 0)
        {
            motor_disable(&motor);
        }

        delay_ms(10);

        // printf("%d,", __HAL_TIM_GetAutoreload(&pid_update_htim));
        printf("%.3f, %.3f\n", motor.target, motor.position);
    }
    return 0;
}
int test_motor_PositionLoop(void)
{

    // if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    // {
    //     printf("failed to start uart it\n");
    // }
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
    pid_position.Kp = 0.0009f; // 0.0019 sinx
    pid_position.Ki = 0.003f;
    pid_position.Kd = 0.00008;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    motor_enable(&motor);
    delay_ms(100);

    while (1)
    {
        // delay_ms(1000);
        // printf("dt = %f\n", motor.pid->dt);
        // printf("dt encoder = %f\n", encoder.pid->dt);
        static int count = 0;
        if (count++ > 200)
        {
            count = 0;
            motor.target += 500;
        }

        delay_ms(2);

        // printf("%d,", __HAL_TIM_GetAutoreload(&pid_update_htim));
        printf("%.3f, %.3f\n", motor.target, motor.position);
    }
    return 0;
}
void test_check(void)
{
    pwm_init(&pwm);
    int arr = __HAL_TIM_GET_AUTORELOAD(&pwm_htim);
    __HAL_TIM_SET_COMPARE(&pwm_htim, TIM_CHANNEL_4, 0.5 * arr);
    printf("arr = %d\n", arr);
}

int test_motor_velociyLoop(void)
{

    // if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    // {
    //     printf("failed to start uart it\n");
    // }
    motor.pid = &pid_velocity;
    motor.controllerType = PID_VelocityLoop;

    // encoder_init(&encoder);
    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    motor_init(&motor);

    // pid_velocity.Kp = 2.0f;
    // pid_velocity.Ki = 0.0f; // 0.05
    // pid_velocity.Kd = 0.0;
    //1.
    pid_velocity.Kp = 2.0f;
    pid_velocity.Ki = 0.0f; // 0.05
    pid_velocity.Kd = 0.0;
    // *pid_velocity.pdt = 0.01;
    float dt = 0.01;
    // pid_velocity.dt = dt;
    // motor.pid->dt = dt;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);

    motor.target = 2;
    motor_enable(&motor);
    // motor_disable(&motor);
    delay_ms(100);
    // LIS3MDL_Init();

    while (1)
    {
        // 3.
        //  int pulse = encoder_getPulses(&encoder);
        //  int counter = __HAL_TIM_GET_COUNTER(encoder.htim);

        // printf("p = %d, c = %d\n", pulse, counter);
        // printf("pid output = %f,\n", motor.pid->output);
        printf("%f,%f\n", motor.target, motor.velocity);
        delay_ms(10);
        // 1.
        //  lis3mdl = motor_getMagneticGauss();
        //  printf("%f,%f,%f\n", lis3mdl.data_x, lis3mdl.data_y, lis3mdl.data_z);
        // 2.
        //  LIS3MDL_DataRead();
        //  delay_ms(1000);
        //  printf("dt = %f\n", motor.pid->dt);
        //  printf("dt encoder = %f\n", encoder.pid->dt);

        // printf("%d,", __HAL_TIM_GetAutoreload(&pid_update_htim));
        // printf("%.3f, %.3f\n", motor.target_vel, motor.velocity);
    }
    return 0;
}
int test_motor_openLoop(void)
{

    // if (HAL_UART_Receive_IT(&huart1, &rx, RXBUFFER_SIZE) != HAL_OK)
    // {
    //     printf("failed to start uart it\n");
    // }
    motor.pid = &pid_velocity;
    motor.controllerType = OpenLoop_Velocity;

    // encoder_init(&encoder);
    tb6612_linkEncoder(&tb6612, &encoder);
    tb6612_linkPWM(&tb6612, &pwm);
    motor_linkTB6612(&motor, &tb6612);
    motor_init(&motor);

    motor_setDirection(&motor, CLOCKWISE);

    motor.target = 0.1f;
    motor_enable(&motor);
    // motor_disable(&motor);
    delay_ms(100);

    while (1)
    {
        printf("%f,%f\n", motor.target, motor.velocity);
        delay_ms(10);
    }
    return 0;
}
