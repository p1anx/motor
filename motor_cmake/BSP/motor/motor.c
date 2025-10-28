
#include "motor.h"
#include "encoder.h"
#include "lis3mdl.h"
#include "magneticSensor.h"
#include "pid.h"
#include "pwm.h"
#include "stdio.h"
#include "stm32_hal.h"
#include "sys/types.h"
#include "tb6612_init.h"
#include "types.h"
#include "eeprom.h"

extern int gKey_flag;
Motor_t motor;

float _constrain(float value, float min, float max)
{
    value = value > max ? max : (value < min ? min : value);
    return value;
}
// motor for lis3mdl
int motor_init_with_lis3mdl(Motor_t *motor)
{
    tb6612_init(motor->tb6612);
    PID_Init(motor->pid);
    // PID_dt_linkToEncoder(motor->pid, motor->tb6612->encoder);
    motor->tb6612->encoder->pid = motor->pid;

    LIS3MDL_InitStruct(&motor->lis3mdl);
    return 0;
}

int motor_init_with_last(Motor_t *motor)
{

    tb6612_init(motor->tb6612);
    PID_Init(motor->pid);
    eeprom_ReadFloat(EEPROM_LAST_ANGLE_ADDR,&motor->last_angle);



    MT6835_Init(&motor->mt6835, &mt6835_spi, MT6835_SPI_CS_PORT, MT6835_SPI_CS_PIN);

    // PID_dt_linkToEncoder(motor->pid, motor->tb6612->encoder);
    motor->tb6612->encoder->pid = motor->pid;
    return 0;
}

int motor_linkMT6835(Motor_t *motor, MT6835_Handle mt6835)
{
    motor->mt6835 = mt6835;
    return 0;
}
int motor_init(Motor_t *motor)
{
    tb6612_init(motor->tb6612);
    PID_Init(motor->pid);
    // PID_dt_linkToEncoder(motor->pid, motor->tb6612->encoder);
    motor->tb6612->encoder->pid = motor->pid;
    return 0;
}

int motor_linkTB6612(Motor_t *motor, TB6612_t *tb6612)
{
    motor->tb6612 = tb6612;
    return 0;
}
float motor_getVelocity(Motor_t *motor)
{
    return encoder_getVelocity(motor->tb6612->encoder);
}

float motor_getPosition(Motor_t *motor)
{
    return encoder_getPosition(motor->tb6612->encoder);
}
float motor_getAngle(Motor_t *motor)
{
    return Encoder_GetAngle(motor->tb6612->encoder);
}

float motor_getAngle_mt(Motor_t *motor)
{

    // MT6835_ReadAngle(&motor->mt6835);
    MT6835_Angle_Update_with_last(&motor->mt6835);
    // delay_ms(1);
    return MT6835_GetMotorAngleDegrees(&motor->mt6835);
}
void motor_setDirection(Motor_t *motor, int direction)
{
    tb6612_setDirection(motor->tb6612, direction);
}

int motor_enable(Motor_t *motor)
{
    motor->isEnable = 1;
    return tb6612_enable(motor->tb6612);
}
// int motor_enableByKey(Motor_t *motor)
// {

//     if (gKey_flag == 1)
//     {
//         motor->isEnable =1;
//     }
//     return tb6612_enable(motor->tb6612);
// }
int motor_disable(Motor_t *motor)
{
    motor->isEnable = 0;
    return tb6612_disable(motor->tb6612);
}
void motor_pid_velocity(Motor_t *motor, float target)
{
    int direction;
    float measued_target = motor_getVelocity(motor);
    motor->velocity = measued_target;
    motor->target_vel = target;
    PID_Update(motor->pid_velocity, target, measued_target);
    float output = motor->pid_velocity->output;
    if (output < 0)
    {
        output = -output;
        motor_setDirection(motor, 0);
    }
    else
    {
        motor_setDirection(motor, 1);
    }

    output = _constrain(output, 0, 1);
    pwm_setDuty(motor->tb6612->pwm, output);
    DEBUG_PRINT("vel=%f\n", motor->velocity);
}

void motor_pid_velocityLoop(Motor_t *motor, float target)
{
    int direction;
    float measued_target = motor_getVelocity(motor);
    motor->velocity = measued_target;
    motor->target_vel = target;
    PID_Update(motor->pid, target, measued_target);
    float output = motor->pid->output;
    if (output < 0)
    {
        output = -output;
        motor_setDirection(motor, 0);
    }
    else
    {
        motor_setDirection(motor, 1);
    }

    output = _constrain(output, 0, 1);
    pwm_setDuty(motor->tb6612->pwm, output);
    DEBUG_PRINT("vel=%f\n", motor->velocity);
}
void motor_pid_velocity_compare(Motor_t *motor, float target)
{
    int direction;
    float measued_target = motor_getVelocity(motor);
    motor->velocity = measued_target;
    motor->target_vel = target;
    PID_Update(motor->pid_velocity, target, measued_target);
    float output = motor->pid_velocity->output;
    if (output < 0)
    {
        output = -output;
        motor_setDirection(motor, 0);
    }
    else
    {
        motor_setDirection(motor, 1);
    }

    // output = _constrain(output, 0, 1);
    pwm_setCompare(motor->tb6612->pwm, output);
    DEBUG_PRINT("vel = %f\n", motor->velocity);
}

void motor_pid_positionLoop(Motor_t *motor, float target)
{
    int direction;
    float measued_target = motor_getPosition(motor);
    motor->position = measued_target;
    motor->target = target;
    PID_Update(motor->pid, target, measued_target);
    float output = motor->pid->output;
    if (output < 0)
    {
        output = -output;
        motor_setDirection(motor, 0);
    }
    else
    {
        motor_setDirection(motor, 1);
    }

    output = _constrain(output, 0, 1);
    pwm_setDuty(motor->tb6612->pwm, output);
    DEBUG_PRINT("pos = %f\n", motor->position);
}
void motor_pid_position(Motor_t *motor, float target)
{
    int direction;
    float measued_target = motor_getPosition(motor);
    motor->position = measued_target;
    motor->target_postion = target;
    PIDController_t *pid;
    pid = motor->pid_position;

    PID_Update(pid, target, measued_target);
    float output = pid->output;
    if (output < 0)
    {
        output = -output;
        motor_setDirection(motor, 0);
    }
    else
    {
        motor_setDirection(motor, 1);
    }

    output = _constrain(output, 0, 1);
    pwm_setDuty(motor->tb6612->pwm, output);
    DEBUG_PRINT("pos = %f\n", motor->position);
}

void motor_pid_angleLoop_mt(Motor_t *motor, float target)
{
    int direction;
    float measued_target = motor_getAngle_mt(motor);
    // delay_ms(1);
    motor->angle = measued_target;
    motor->target = target;
    PID_Update(motor->pid, target, measued_target);
    float output = motor->pid->output;
    if (output < 0)
    {
        output = -output;
        motor_setDirection(motor, 0);
    }
    else
    {
        motor_setDirection(motor, 1);
    }

    output = _constrain(output, 0, 1);
    pwm_setDuty(motor->tb6612->pwm, output);
    // printf("mt output = %f, %f\n",motor->pid->output);

    DEBUG_PRINT("angle = %f\n", motor->angle);
}
void motor_pid_angleLoop_with_last(Motor_t *motor, float target)
{
    int direction;
    float measued_target = motor_getAngle(motor) + motor->last_angle;
    motor->angle = measued_target;
    motor->target = target;
    PID_Update(motor->pid, target, measued_target);
    float output = motor->pid->output;
    if (output < 0)
    {
        output = -output;
        motor_setDirection(motor, 0);
    }
    else
    {
        motor_setDirection(motor, 1);
    }

    output = _constrain(output, 0, 1);
    pwm_setDuty(motor->tb6612->pwm, output);
    DEBUG_PRINT("angle = %f\n", motor->angle);
}
void motor_pid_angleLoop(Motor_t *motor, float target)
{
    int direction;
    float measued_target = motor_getAngle(motor);
    motor->angle = measued_target;
    motor->target = target;
    PID_Update(motor->pid, target, measued_target);
    float output = motor->pid->output;
    if (output < 0)
    {
        output = -output;
        motor_setDirection(motor, 0);
    }
    else
    {
        motor_setDirection(motor, 1);
    }

    output = _constrain(output, 0, 1);
    pwm_setDuty(motor->tb6612->pwm, output);
    DEBUG_PRINT("angle = %f\n", motor->angle);
}
float motor_getMagnetic(void)
{
    return MagneticSenor_getValue();
}
LIS3MDL_t motor_getMagneticGauss(void)
{
    return LIS3MDL_getMagneticGauss();
}
void motor_pid_magneticLoop(Motor_t *motor, float target)
{
    LIS3MDL_t lis3mdl = motor_getMagneticGauss();
    float measued_target = lis3mdl.data_x;
    motor->magnetic = measued_target;
    motor->target = target;
    PID_Update(motor->pid, target, measued_target);
    float output = motor->pid->output;
    if (output < 0)
    {
        output = -output;
        motor_setDirection(motor, 0);
    }
    else
    {
        motor_setDirection(motor, 1);
    }

    output = _constrain(output, 0, 1);
    // printf("output = %f\n", output);
    pwm_setDuty(motor->tb6612->pwm, output);
}
void motor_velocity_openLoop(Motor_t *motor, float target)
{
    float measued_target = motor_getVelocity(motor);
    motor->velocity = measued_target;
    motor->target = target;
    float output = target;
    // output = _constrain(output, 0, 1);
    pwm_setDuty(motor->tb6612->pwm, output);
}
void motor_pid(Motor_t *motor)
{
    motor_pid_velocity(motor, 1);
    // motor_pid_velocity_compare(motor, 1);
}
int motor_move(Motor_t *motor, float target)
{
    switch (motor->controllerType)
    {
    case PID_VelocityLoop:
        // motor_pid_velocity(motor, target);
        motor_pid_velocityLoop(motor, target);
        break;
    case PID_PositionLoop:
        // motor_pid_position(motor, target);
        motor_pid_positionLoop(motor, target);
        break;
    case PID_AngleLoop:

        // motor_pid_angleLoop_with_last(motor, target);
        motor_pid_angleLoop(motor, target);
            // motor_pid_angleLoop_mt(motor, target);
        break;
    case PID_MagneticLoop:
        motor_pid_magneticLoop(motor, target);
        break;
    case OpenLoop_Velocity:
        motor_velocity_openLoop(motor, target);
        break;
    default:
        motor_pid_velocity(motor, target);
        break;
    }
    return 0;
}
extern Motor_t motor;
extern Encoder_t encoder;
extern int gKey_flag_0;
extern int gKey_flag_1;

int motor_AngleLoop_toPython(void)
{
    PIDController_t pid_position;
    PIDController_t pid_velocity;
    PIDController_t pid;
    TB6612_t tb6612;
    PWM_t pwm;

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
    //3.
    // pid_position.Kp = 0.015f; // 0.0019 sinx
    // pid_position.Ki = 0.0f;
    // pid_position.Kd = 0.001;
    //4.
    pid_position.Kp = 0.015f; // 0.0019 sinx
    pid_position.Ki = 0.0f;
    pid_position.Kd = 0.001;

    // pid_velocity.dt = PID_UPDATE_TIME; // ms
    motor_setDirection(&motor, CLOCKWISE);
    motor.target = 00.0f;
    // LIS3MDL_Init();

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
                    motor.target += 10;
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
