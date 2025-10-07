#include "foc_motor.h"
#include "encoder.h"
#include "motor_config.h"
#include "stm32_hal.h"

void FOCMotor_init(FOCMotor_t *motor)
{
    if (!motor)
        return;

    // maximum angular velocity to be used for positioning
    motor->velocity_limit = DEF_VEL_LIM;
    // maximum voltage to be set to the motor
    motor->voltage_limit = DEF_POWER_SUPPLY;

    // index search velocity
    motor->velocity_index_search = DEF_INDEX_SEARCH_TARGET_VELOCITY;
    // sensor and motor align voltage
    motor->voltage_sensor_align = DEF_VOLTAGE_SENSOR_ALIGN;

    // electric angle of the zero angle
    motor->zero_electric_angle = 0;

    // default modulation is SinePWM
    motor->foc_modulation = FOCModulationType_SinePWM;

    // default target value
    motor->target = 0;
    motor->voltage_d = 0;
    motor->voltage_q = 0;

    // state variables initialization
    motor->shaft_angle = 0;
    motor->shaft_velocity = 0;
    motor->shaft_velocity_sp = 0;
    motor->shaft_angle_sp = 0;

    // motor configuration parameters
    motor->pole_pairs = 0;

    // default control type
    motor->controller = ControlType_voltage;

    // Initialize PID controller with default values
    PIDController_init(&motor->PID_velocity, DEF_PID_VEL_P, DEF_PID_VEL_I, DEF_PID_VEL_D, DEF_PID_VEL_U_RAMP, DEF_POWER_SUPPLY);
    //
    // // Initialize P controller for angle with default values
    PIDController_init(&motor->P_angle, DEF_P_ANGLE_P, 0, 0, 10, DEF_VEL_LIM);
    //
    // // Initialize low pass filter with default values
    LowPassFilter_init(&motor->LPF_velocity, DEF_VEL_FILTER_Tf);
    LowPassFilter_init(&motor->LPF_current_d, DEF_CURRENT_FILTER_Tf);
    LowPassFilter_init(&motor->LPF_current_q, DEF_CURRENT_FILTER_Tf);

    // sensor
    // motor->sensor = NULL;
    PRINT_OK("FOCMotor init");
}

void FOCMotor_linkSensor(FOCMotor_t *motor, Sensor_t *_sensor)
{
    if (!motor)
        return;
    motor->sensor = _sensor;
    printf("linked to sensor\n");
}

void FOCMotor_linkEncoder(FOCMotor_t *motor, Encoder_t *encoder)
{
    if (!motor)
        return;
    motor->encoder = encoder;
    printf("FOCMotor linked to encoder\n");
}
float FOCMotor_shaftAngle(FOCMotor_t *motor)
{
    if (!motor)
        return 0;
    // if no sensor linked return stored shaft_angle
    if (!motor->encoder)
    {
        printf("[error] motor encoder is bad in shaft_angle\n");
        return motor->shaft_angle;
    }
    return Encoder_getAngle(motor->encoder);
}

// shaft velocity calculation
float FOCMotor_shaftVelocity(FOCMotor_t *motor)
{
    if (!motor)
        return 0;
    // if no sensor linked return 0
    if (!motor->encoder)
        return 0;
    // return LowPassFilter(&motor->LPF_velocity,
    //                      Encoder_getVelocity(motor->sensor));
    return Encoder_getVelocity(motor->encoder);
}
float FOCMotor_shaftVelocityRPM(FOCMotor_t *motor)
{
    if (!motor)
        return 0;
    // if no sensor linked return 0
    if (!motor->encoder)
        return 0;
    // return LowPassFilter(&motor->LPF_velocity,
    //                      Encoder_getVelocity(motor->sensor));
    return Encoder_getVelocityRPM(motor->encoder);
}

void FOCMotor_linkAS5600(FOCMotor_t *focmotor, AS5600_t *as5600)
{
    if (!focmotor)
        return;
    focmotor->encoder->as5600 = as5600;
    printf("FOCMotor linked to as5600\n");
}
