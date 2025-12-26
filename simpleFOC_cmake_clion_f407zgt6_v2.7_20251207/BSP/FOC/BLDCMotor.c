
#include "BLDCMotor.h"
#include "LoopController.h"
#include "as5600.h"
#include "currentSense.h"
#include "encoder.h"
#include "foc_base.h"
#include "foc_motor.h"
#include "key.h"
#include "lowpass_filter.h"
#include "pid.h"
#include "pwm.h"
#include "stm32_hal.h"
#include <stdio.h>
#include "motor_config.h"
#include <string.h>
#include "vofa.h"

extern int g_keyFlag;
BLDCMotor_t motor;
extern CurrentSense_t currentSense;
extern MultiRate_Controller_t MultiRate_Controller;

int BLDCMotor_enable(BLDCMotor_t *motor)
{
    BLDCDriverPWM_enable(&motor->Driver);
    motor->isEnable = 1;
    return 0;
}
int BLDCMotor_disable(BLDCMotor_t *motor)
{
    HAL_GPIO_WritePin(motor->enable_Port, motor->enable_Pin, RESET);
    // BLDCMotor_SVPWM(motor, 0, 0, motor->e_angle);
    // BLDCMotor_setPhaseVoltage(motor, 0, 0, motor->e_angle);
    BLDCDriver_setPwm(&motor->Driver, 0, 0, 0);
    motor->isEnable = 0;
    return 0;
}
void BLDCDriver_writeDutyCycle3PWM(float dc_a, float dc_b, float dc_c)
{
    // transform duty cycle from [0,1] to [0,4095]
    // TIM1->CCR1 = (int)(dc_a * _PWM_RANGE);
    //	TIM1->CCR2 = (int)(dc_b * _PWM_RANGE);
    //	TIM4->CCR4 = (int)(dc_c * _PWM_RANGE);

    // int pwm_range = htim1.Instance->ARR;
    // __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, dc_a * pwm_range);
    // __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, dc_b * pwm_range);
    // pwm_range = htim4.Instance->ARR;
    // __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, dc_c * pwm_range);
}
void BLDCDriver3PWM_setPwm(BLDCDriver_t *driver, float Ua, float Ub, float Uc)
{

    // limit the voltage in driver
    Ua = _constrain(Ua, 0.0, driver->voltage_limit);
    Ub = _constrain(Ub, 0.0, driver->voltage_limit);
    Uc = _constrain(Uc, 0.0, driver->voltage_limit);
    // calculate duty cycle
    // limited in [0,1]
    float dc_a = _constrain(Ua / driver->voltage_power_supply, 0.0, 1.0);
    float dc_b = _constrain(Ub / driver->voltage_power_supply, 0.0, 1.0);
    float dc_c = _constrain(Uc / driver->voltage_power_supply, 0.0, 1.0);

    // hardware specific writing
    // hardware specific function - depending on driver and mcu
    // _writeDutyCycle3PWM(dc_a, dc_b, dc_c, driver->pwmA, driver->pwmB,
    //                     driver->pwmC);
    BLDCDriver_writeDutyCycle3PWM(dc_a, dc_b, dc_c);
}
void BLDCDriver_writeDutyCycle6PWM(float dc_a, float dc_b, float dc_c)
{

    int pwm_range = pwm_tim.Instance->ARR;
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_1, dc_a * pwm_range);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_2, dc_b * pwm_range);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_3, dc_c * pwm_range);
}
void BLDCDriver6PWM_setPwm(BLDCDriver_t *driver, float Ua, float Ub, float Uc)
{

    // limit the voltage in driver
    Ua = _constrain(Ua, 0.0, driver->voltage_limit);
    Ub = _constrain(Ub, 0.0, driver->voltage_limit);
    Uc = _constrain(Uc, 0.0, driver->voltage_limit);
    // calculate duty cycle
    // limited in [0,1]
    float dc_a = _constrain(Ua / driver->voltage_power_supply, 0.0, 1.0);
    float dc_b = _constrain(Ub / driver->voltage_power_supply, 0.0, 1.0);
    float dc_c = _constrain(Uc / driver->voltage_power_supply, 0.0, 1.0);

    // hardware specific writing
    // hardware specific function - depending on driver and mcu
    // _writeDutyCycle3PWM(dc_a, dc_b, dc_c, driver->pwmA, driver->pwmB,
    //                     driver->pwmC);
    BLDCDriver_writeDutyCycle6PWM(dc_a, dc_b, dc_c);
}


void BLDCMotor_setPhaseVoltage(BLDCMotor_t *motor, float Uq, float Ud, float electrical_angle)
{
    BLDCDriver_setPhaseVoltage(&motor->Driver, Uq, Ud, electrical_angle);
}
void BLDCMotor_SVPWM(BLDCMotor_t *motor, float Uq, float Ud, float electrical_angle)
{
    if (motor)
    {
        if (motor->deadzone_enable)
        {
            float dz = motor->deadzone_uq;
            if (dz < 0.0f) dz = 0.0f;
            if (Uq > 0.0f && Uq < dz) Uq = dz;
            else if (Uq < 0.0f && -Uq < dz) Uq = -dz;
        }
    }
    BLDCDriver_SVPWM(&motor->Driver, Uq, Ud, electrical_angle);
}

void BLDCMotor_initAll(BLDCMotor_t *motor, int pp, EncoderType_enum encoderType, ControlType_t controllerType)
{
    if (!motor)
        return;

    motor->foc_motor.pole_pairs = pp;
    motor->foc_motor.controller = controllerType;
    motor->foc_motor.foc_modulation = FOCModulationType_SinePWM;
    // motor->foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;
    FOCMotor_init(&motor->foc_motor);


    motor->foc_motor.Encoder.EncoderType = encoderType;
    Encoder_init0(&motor->foc_motor.Encoder);


    BLDCDriver_init(&motor->Driver, CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION, CONFIG_VoltageSupply, CONFIG_VoltageLimit);

    // save pole pairs number

    // Initialize open loop timestamp
    motor->open_loop_timestamp = 0;
    motor->angle = 0;
    motor->lastAngle = 0;


    // BLDCDriverPWM_enable(&motor->Driver);
    BLDCMotor_alignSensor(motor);
}
void BLDCMotor_initPID(BLDCMotor_t *motor, int pp, EncoderType_enum encoderType, ControlType_t controllerType, PIDController pid_param)
{
    if (!motor)
        return;
    // PWM6_Init(CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION);
    // BLDCMotor_enable(motor);
    // CurrentSense_InitADC(&motor->CurrentSense);

    motor->foc_motor.pole_pairs = pp;
    motor->foc_motor.controller = controllerType;
    // motor->foc_motor.foc_modulation = FOCModulationType_SinePWM;
    motor->foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;
    FOCMotor_init(&motor->foc_motor);


    motor->foc_motor.Encoder.EncoderType = encoderType;
    Encoder_init0(&motor->foc_motor.Encoder);


    BLDCDriver_init(&motor->Driver, CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION, CONFIG_VoltageSupply, CONFIG_VoltageLimit);
    BLDCMotor_enable(motor);
    motor->currentSense = &currentSense;
    printf("is calibration..\n");
    CurrentSense_InitADC(motor->currentSense);

    // save pole pairs number

    // Initialize open loop timestamp
    motor->open_loop_timestamp = 0;
    motor->angle = 0;
    motor->lastAngle = 0;


    delay_ms(1000);
    // BLDCDriverPWM_enable(&motor->Driver);
    BLDCMotor_alignSensor(motor);

    PIDController_init(&motor->PID, pid_param.P,pid_param.I,pid_param.D,5,8.0f);
}
void BLDCMotor_initPID_CurrentDQ(BLDCMotor_t *motor, int pp, EncoderType_enum encoderType, ControlType_t controllerType, PIDController pid_id, PIDController pid_iq)
{
    if (!motor)
        return;
    // PWM6_Init(CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION);
    // BLDCMotor_enable(motor);
    // CurrentSense_InitADC(&motor->CurrentSense);

    motor->foc_motor.pole_pairs = pp;
    motor->foc_motor.controller = controllerType;
    // motor->foc_motor.foc_modulation = FOCModulationType_SinePWM;
    motor->foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;
    FOCMotor_init(&motor->foc_motor);


    motor->foc_motor.Encoder.EncoderType = encoderType;
    Encoder_init0(&motor->foc_motor.Encoder);


    BLDCDriver_init(&motor->Driver, CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION, CONFIG_VoltageSupply, CONFIG_VoltageLimit);
    BLDCMotor_enable(motor);
    motor->currentSense = &currentSense;
    printf("is calibration..\n");
    CurrentSense_InitADC(motor->currentSense);

    // save pole pairs number

    // Initialize open loop timestamp
    motor->open_loop_timestamp = 0;
    motor->angle = 0;
    motor->lastAngle = 0;


    delay_ms(1000);
    // BLDCDriverPWM_enable(&motor->Driver);
    BLDCMotor_alignSensor(motor);

    PIDController_init(&motor->PID_id, pid_id.P,pid_id.I,pid_id.D,5,8.0f);
    PIDController_init(&motor->PID_iq,pid_iq.P,pid_iq.I,pid_iq.D,5,8.0f);
    LowPassFilter_init(&motor->lpf_id, 0.05);
    LowPassFilter_init(&motor->lpf_iq, 0.05);
}
void BLDCMotor_initPID_CurrentVelocity(BLDCMotor_t *motor, int pp, EncoderType_enum encoderType, ControlType_t controllerType, PIDController pid_id, PIDController pid_iq, PIDController pid_velocity)
{
    if (!motor)
        return;
    // PWM6_Init(CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION);
    // BLDCMotor_enable(motor);
    // CurrentSense_InitADC(&motor->CurrentSense);

    motor->foc_motor.pole_pairs = pp;
    motor->foc_motor.controller = controllerType;
    // motor->foc_motor.foc_modulation = FOCModulationType_SinePWM;
    motor->foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;
    FOCMotor_init(&motor->foc_motor);


    motor->foc_motor.Encoder.EncoderType = encoderType;
    Encoder_init0(&motor->foc_motor.Encoder);


    BLDCDriver_init(&motor->Driver, CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION, CONFIG_VoltageSupply, CONFIG_VoltageLimit);
    BLDCMotor_enable(motor);
    motor->currentSense = &currentSense;
    printf("is calibration..\n");
    CurrentSense_InitADC(motor->currentSense);

    // save pole pairs number

    // Initialize open loop timestamp
    motor->open_loop_timestamp = 0;
    motor->angle = 0;
    motor->lastAngle = 0;


    delay_ms(1000);
    // BLDCDriverPWM_enable(&motor->Driver);
    BLDCMotor_alignSensor(motor);

    PIDController_init(&motor->PID_id, pid_id.P,pid_id.I,pid_id.D,5,8.0f);
    PIDController_init(&motor->PID_iq,pid_iq.P,pid_iq.I,pid_iq.D,5,8.0f);
    PIDController_init(&motor->PID_velocity,pid_velocity.P,pid_velocity.I,pid_velocity.D,5,8.0f);
    LowPassFilter_init(&motor->lpf_id, 0.05);
    LowPassFilter_init(&motor->lpf_iq, 0.05);
    LowPassFilter_init(&motor->lpf_velocity, 0.05);
}

void BLDCMotor_initPID_CurrentVelocityAngle(BLDCMotor_t *motor, int pp, EncoderType_enum encoderType, ControlType_t controllerType, PIDController pid_id, PIDController pid_iq, PIDController pid_velocity, PIDController pid_degree)
{
    if (!motor)
        return;
    // PWM6_Init(CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION);
    // BLDCMotor_enable(motor);
    // CurrentSense_InitADC(&motor->CurrentSense);

    // Initialize open loop timestamp
    motor->open_loop_timestamp = 0;
    motor->angle = 0;
    motor->lastAngle = 0;

    motor->foc_motor.pole_pairs = pp;
    motor->foc_motor.controller = controllerType;
    // motor->foc_motor.foc_modulation = FOCModulationType_SinePWM;
    // motor->foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;
    motor->Driver.FOCModulationType = FOCModulationType_SinePWM;
    // motor->Driver.FOCModulationType = FOCModulationType_SpaceVectorPWM;
    FOCMotor_init(&motor->foc_motor);


    motor->foc_motor.Encoder.EncoderType = encoderType;
    Encoder_init0(&motor->foc_motor.Encoder);


    BLDCDriver_init(&motor->Driver, CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION, CONFIG_VoltageSupply, CONFIG_VoltageLimit);
    BLDCMotor_enable(motor);

    printf("is calibration..\n");
    motor->currentSense = &currentSense;
    CurrentSense_InitADC(motor->currentSense);

    // save pole pairs number


    delay_ms(1000);
    // BLDCDriverPWM_enable(&motor->Driver);
    BLDCMotor_alignSensor(motor);

    PIDController_init(&motor->PID_id, pid_id.P,pid_id.I,pid_id.D,100.0f,6.8f);
    PIDController_init(&motor->PID_iq,pid_iq.P,pid_iq.I,pid_iq.D,100.0f,6.8f);
    if (controllerType == ControlType_velocityClosedLoop) {
        PIDController_init(&motor->PID_velocity,pid_velocity.P,pid_velocity.I,pid_velocity.D,100,6.8f);
    }
    else {
        PIDController_init(&motor->PID_velocity,pid_velocity.P,pid_velocity.I,pid_velocity.D,100,0.5f);
    }
    PIDController_init(&motor->PID_degree,pid_degree.P,pid_degree.I,pid_degree.D,200,360.0f*20);
    LowPassFilter_init(&motor->lpf_id, CONFIG_FILTER_Tf);
    LowPassFilter_init(&motor->lpf_iq, CONFIG_FILTER_Tf);
    LowPassFilter_init(&motor->lpf_ia, CONFIG_FILTER_Tf);
    LowPassFilter_init(&motor->lpf_ib, CONFIG_FILTER_Tf);
    LowPassFilter_init(&motor->lpf_velocity, CONFIG_FILTER_Tf);
    LowPassFilter_init(&motor->lpf_degree, CONFIG_FILTER_Tf);
    LowPassFilter_init(&motor->lpf_eAngle, CONFIG_FILTER_Tf);

    const float cutoff_freq = 50;
    const float sample_freq = CONFIG_PWM_HZ;
    lpf_init(&motor->lpf_ia0, cutoff_freq, sample_freq);
    lpf_init(&motor->lpf_ib0, cutoff_freq, sample_freq);
    lpf_init(&motor->lpf_iq0, cutoff_freq, sample_freq);
    lpf_init(&motor->lpf_id0, cutoff_freq, sample_freq);
    lpf_init(&motor->filter_velocity, cutoff_freq, sample_freq);
    lpf_init(&motor->filter_angle, cutoff_freq, sample_freq);


    motor->deadzone_enable = 0;
    motor->deadzone_uq = 0.2;
}
void BLDCMotor_PIDInit(BLDCMotor_t *motor) {
    PIDController_init(&motor->PID_id, motor->PID_id.P,motor->PID_id.I,motor->PID_id.D,100.0f,motor->PID_id.limit);
    PIDController_init(&motor->PID_iq,motor->PID_iq.P,motor->PID_iq.I,motor->PID_iq.D,100.0f,motor->PID_iq.limit);
    if (motor->foc_motor.controller == ControlType_velocityClosedLoop) {
        PIDController_init(&motor->PID_velocity,motor->PID_velocity.P,motor->PID_velocity.I,motor->PID_velocity.D,100,motor->PID_velocity.limit);
    }
    else {
        PIDController_init(&motor->PID_velocity,motor->PID_velocity.P,motor->PID_velocity.I,motor->PID_velocity.D,100,motor->PID_velocity.limit);
    }
    PIDController_init(&motor->PID_degree,motor->PID_degree.P,motor->PID_degree.I,motor->PID_degree.D,200,motor->PID_degree.limit);
}
void BLDCMotor_LowpassFilterInit(BLDCMotor_t *motor) {

    LowPassFilter_init(&motor->lpf_id, CONFIG_FILTER_Tf);
    LowPassFilter_init(&motor->lpf_iq, CONFIG_FILTER_Tf);
    LowPassFilter_init(&motor->lpf_ia, CONFIG_FILTER_Tf);
    LowPassFilter_init(&motor->lpf_ib, CONFIG_FILTER_Tf);
    LowPassFilter_init(&motor->lpf_velocity,CONFIG_VELOCITY_FILTER_Tf);
    LowPassFilter_init(&motor->lpf_degree, CONFIG_FILTER_Tf);
    LowPassFilter_init(&motor->lpf_eAngle, CONFIG_FILTER_Tf);

    const float cutoff_freq = 50;
    const float id_cutoff_freq = 10;
    const float iq_cutoff_freq = 10;
    const float sample_freq = CONFIG_PWM_HZ;
    lpf_init(&motor->lpf_ia0, cutoff_freq, sample_freq);
    lpf_init(&motor->lpf_ib0, cutoff_freq, sample_freq);
    lpf_init(&motor->lpf_iq0, iq_cutoff_freq, sample_freq);
    lpf_init(&motor->lpf_id0, id_cutoff_freq, sample_freq);
    lpf_init(&motor->filter_velocity, cutoff_freq, sample_freq);
    lpf_init(&motor->filter_angle, cutoff_freq, sample_freq);
}
void BLDCMotor_init3508(BLDCMotor_t *motor, EncoderType_enum encoderType, ControlType_t controllerType)
{
    if (!motor)
        return;

    // Initialize open loop timestamp
    motor->open_loop_timestamp = 0;
    motor->angle = 0;
    motor->lastAngle = 0;

    motor->foc_motor.pole_pairs = CONFIG_PP;
    motor->foc_motor.controller = controllerType;
    // motor->foc_motor.foc_modulation = FOCModulationType_SinePWM;
    // motor->foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;
    motor->Driver.FOCModulationType = FOCModulationType_SinePWM;
    // motor->Driver.FOCModulationType = FOCModulationType_SpaceVectorPWM;
    UsTimer_init();
    FOCMotor_init(&motor->foc_motor);

    motor->foc_motor.Encoder.EncoderType = encoderType;
    Encoder_init0(&motor->foc_motor.Encoder);

    BLDCDriver_init(&motor->Driver, CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION, CONFIG_VoltageSupply, CONFIG_VoltageLimit);


    // BLDCDriverPWM_enable(&motor->Driver);
    BLDCMotor_alignSensor(motor);
    BLDCMotor_PIDInit(motor);
    BLDCMotor_LowpassFilterInit(motor);

    BLDCMotor_disable(motor);
    delay_ms(10);

    printf("is calibration..\n");
    motor->currentSense = &currentSense;
    CurrentSense_InitADC(motor->currentSense);

    delay_ms(1000);


    motor->deadzone_enable = 0;
    motor->deadzone_uq = 0.2;
}

void BLDCMotor_initPID_CurrentVelocityAngle_Inline(BLDCMotor_t *motor, int pp, EncoderType_enum encoderType, ControlType_t controllerType, PIDController pid_id, PIDController pid_iq, PIDController pid_velocity, PIDController pid_degree)
{
    if (!motor)
        return;
    // PWM6_Init(CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION);
    // BLDCMotor_enable(motor);
    // CurrentSense_InitADC(&motor->CurrentSense);

    printf("is calibration..\n");
    motor->currentSense = &currentSense;
    CurrentSense_InitADC_Regular(motor->currentSense);

    // Initialize open loop timestamp
    motor->open_loop_timestamp = 0;
    motor->angle = 0;
    motor->lastAngle = 0;

    motor->foc_motor.pole_pairs = pp;
    motor->foc_motor.controller = controllerType;
    motor->Driver.FOCModulationType = FOCModulationType_SinePWM;
    // motor->Driver.FOCModulationType = FOCModulationType_SpaceVectorPWM;

    FOCMotor_init(&motor->foc_motor);


    motor->foc_motor.Encoder.EncoderType = encoderType;
    Encoder_init0(&motor->foc_motor.Encoder);


    BLDCDriver_init(&motor->Driver, CONFIG_PWM_HZ, CONFIG_PWM_RESOLUTION, CONFIG_VoltageSupply, CONFIG_VoltageLimit);
    BLDCMotor_enable(motor);


    // save pole pairs number


    delay_ms(1000);
    // BLDCDriverPWM_enable(&motor->Driver);
    BLDCMotor_alignSensor(motor);

    PIDController_init(&motor->PID_id, pid_id.P,pid_id.I,pid_id.D,5,8.0f);
    PIDController_init(&motor->PID_iq,pid_iq.P,pid_iq.I,pid_iq.D,5,6.8f);
    PIDController_init(&motor->PID_velocity,pid_velocity.P,pid_velocity.I,pid_velocity.D,100,8.0f);
    PIDController_init(&motor->PID_degree,pid_degree.P,pid_degree.I,pid_degree.D,100,14.0f);
    LowPassFilter_init(&motor->lpf_id, 0.001);
    LowPassFilter_init(&motor->lpf_iq, 0.001);
    LowPassFilter_init(&motor->lpf_ia, 0.001);
    LowPassFilter_init(&motor->lpf_ib, 0.001);
    LowPassFilter_init(&motor->lpf_velocity, 0.1);
    LowPassFilter_init(&motor->lpf_degree, 0.1);

    const float cutoff_freq = 30;
    const float sample_freq = CONFIG_PWM_HZ;
    lpf_init(&motor->lpf_ia0, cutoff_freq, sample_freq);
    lpf_init(&motor->lpf_ib0, cutoff_freq, sample_freq);
    lpf_init(&motor->lpf_iq0, cutoff_freq, sample_freq);
    lpf_init(&motor->lpf_id0, cutoff_freq, sample_freq);
    lpf_init(&motor->filter_velocity, cutoff_freq, sample_freq);
    lpf_init(&motor->filter_angle, cutoff_freq, sample_freq);


    motor->deadzone_enable = 0;
    motor->deadzone_uq = 0.2;
}
void BLDCMotor_init(BLDCMotor_t *motor, int pp)
{
    if (!motor)
        return;

    FOCMotor_init(&motor->foc_motor);

    // save pole pairs number
    motor->foc_motor.pole_pairs = pp;

    // Initialize phase voltages
    motor->Ua = 0;
    motor->Ub = 0;
    motor->Uc = 0;
    motor->Ualpha = 0;
    motor->Ubeta = 0;

    // Initialize driver pointer
    motor->driver = NULL;

    // Initialize open loop timestamp
    motor->open_loop_timestamp = 0;
    motor->angle = 0;
    motor->lastAngle = 0;

    // motor->foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;
    motor->foc_motor.foc_modulation = FOCModulationType_SinePWM;
    motor->enable_Port = MOTOR_ENABLE_PORT;
    motor->enable_Pin = MOTOR_ENABLE_PIN;

    PRINT_OK("BLDCMotor init");
}

void BLDCMotor_linkDriver(BLDCMotor_t *motor, BLDCDriver_t *_driver)
{
    if (!motor)
        return;
    motor->driver = _driver;

    PRINT_OK("linked to driver");
}
void BLDCMotor_linkEncoder(BLDCMotor_t *motor, Encoder_t *encoder)
{
    if (!motor)
    {
        PRINT_ERROR("motor isn't ready");
        return;
    }

    // motor->foc_motor.encoder = encoder;
    // 1.
    // FOCMotor_linkEncoder(motor->FOCMotor, encoder);
    // 2.
    FOCMotor_linkEncoder(&motor->foc_motor, encoder);

    PRINT_OK("linked ot encoder");
}
void BLDCMotor_linkAS5600(BLDCMotor_t *motor, AS5600_t *as5600)
{
    if (!motor)
        return;
    motor->as5600 = as5600;
    printf("motor linked to as5600\n");
}
float BLDCMotor_getVelocity(BLDCMotor_t *motor)
{
    return FOCMotor_shaftVelocity(&motor->foc_motor);
}
float BLDCMotor_getVelocityRPM(BLDCMotor_t *motor)
{
    return FOCMotor_shaftVelocityRPM(&motor->foc_motor);
}

float AS5600_GetAngularSpeed_v0(AS5600_t *sensor, uint8_t mode, bool update)
{
    if (!sensor)
        return 0;

    if (update)
    {
        sensor->lastReadAngle = AS5600_ReadAngle(sensor);
        if (sensor->error != AS5600_OK)
        {
            return 0; // 或者返回 NAN
        }
    }

    uint32_t now = getMicros();
    int angle = sensor->lastReadAngle;
    uint32_t deltaT = now - sensor->lastMeasurement;
    int deltaA = angle - sensor->lastAngle;

    // 假设两次测量之间旋转不超过180度
    if (deltaA > 2048)
        deltaA -= 4096;
    else if (deltaA < -2048)
        deltaA += 4096;

    float speed = (deltaA * 1000000.0f) / deltaT;

    // 记住最后的时间和角度
    sensor->lastMeasurement = now;
    sensor->lastAngle = angle;

    // 返回弧度、RPM或度数
    if (mode == AS5600_MODE_RADIANS)
    {
        return speed * AS5600_RAW_TO_RADIANS;
    }
    if (mode == AS5600_MODE_RPM)
    {
        return speed * AS5600_RAW_TO_RPM;
    }
    // 默认返回度数
    return speed * AS5600_RAW_TO_DEGREES;
}

float BLDCMotor_getVelocityRPM_v1(BLDCMotor_t *motor)
{
    int angle = AS5600_ReadAngle(motor->as5600);
    // if (motor->as5600->error != AS5600_OK)
    // {
    //     printf("[ERROR] AS5600 get speed\n");
    //     return 0; // 或者返回 NAN
    // }

    uint32_t deltaT = motor->pid_dt;
    int deltaA = angle - motor->as5600->lastReadAngle;

    // 假设两次测量之间旋转不超过180度
    if (deltaA > 2048)
        deltaA -= 4096;
    else if (deltaA < -2048)
        deltaA += 4096;

    float speed = (deltaA) / (deltaT * 1e-3);

    // 记住最后的时间和角度
    motor->as5600->lastReadAngle = angle;

    // 返回弧度、RPM或度数
    {
        return speed * AS5600_RAW_TO_RPM;
    }
    // 默认返回度数
}
float BLDCMotor_getAngle(BLDCMotor_t *motor)
{
    // return _electricalAngle(FOCMotor_shaftAngle(&motor->foc_motor), motor->foc_motor.pole_pairs);
    // return FOCMotor_shaftAngle(motor->FOCMotor);
    return FOCMotor_shaftAngle(&motor->foc_motor);
    // return AS5600_getAngle(motor->as5600);
}
float BLDCMotor_getAngleDegrees(BLDCMotor_t *motor)
{
    return FOCMotor_shaftAngleDegrees(&motor->foc_motor);
}
float BLDCMotor_getElectricalAngle(BLDCMotor_t *motor)
{
    motor->angle = 1 * BLDCMotor_getAngle(motor);
    motor->e_angle = _normalizeAngle(_electricalAngle(motor->angle, motor->foc_motor.pole_pairs));
    return motor->e_angle;
}

float BLDCMotor_getCurrentDQ_noFilter(BLDCMotor_t *motor)
{
    float i_a, i_b, i_alpha, i_beta, i_q, i_d;

    i_a = motor->currentSense->i_a;
    i_b = motor->currentSense->i_b;
    Clarke_Transform(i_a, i_b, &i_alpha, &i_beta);
    Park_Transform(i_alpha, i_beta, _normalizeAngle(motor->e_angle), &i_d, &i_q);
    motor->currentSense->i_d = i_d;
    motor->currentSense->i_q = i_q;
    return 0;
}
float BLDCMotor_getCurrentDQ_Polling(BLDCMotor_t *motor)
{

    // float i_a, i_b, i_alpha, i_beta, i_q, i_d;
    // i_a = motor->currentSense->i_a;
    // i_b = motor->currentSense->i_b;
    // Clarke_Transform(i_a, i_b, &i_alpha, &i_beta);
    // Park_Transform(i_alpha, i_beta, _normalizeAngle(motor->e_angle), &i_d, &i_q);
    // i_d = LowPassFilter(&motor->foc_motor.LPF_current_d, i_d);
    // i_q = LowPassFilter(&motor->foc_motor.LPF_current_q, i_q);
    // motor->currentSense->i_d = i_d;
    // motor->currentSense->i_q = i_q;

    currentSense_pollingGetCurrent(motor->currentSense);

    motor->currentSense->i_a = lpf_process(&motor->lpf_ia0, motor->currentSense->i_a);
    motor->currentSense->i_b = lpf_process(&motor->lpf_ib0, motor->currentSense->i_b);
    // motor->currentSense->i_a =LowPassFilter(&motor->lpf_ia, motor->currentSense->i_a);
    // motor->currentSense->i_b =LowPassFilter(&motor->lpf_ib, motor->currentSense->i_b);
    // if (!motor->currentSense->isSetIc) {
        motor->currentSense->i_alpha = motor->currentSense->i_a;
        motor->currentSense->i_beta = _1_SQRT3*motor->currentSense->i_a + _2_SQRT3 *motor->currentSense->i_b;
    // }
    // else {
    //     float mid = (1.f/3)*(motor->currentSense->i_a + motor->currentSense->i_b + motor->currentSense->i_c);
    //     float a = motor->currentSense->i_a -mid;
    //     float b = motor->currentSense->i_b -mid;
    //     motor->currentSense->i_alpha = a;
    //     motor->currentSense->i_beta = _1_SQRT3 * a + _2_SQRT3 * b;
    // }
    float _cos_e = _cos(motor->e_angle);
    float _sin_e = _sin(motor->e_angle);
    float i_alpha = motor->currentSense->i_alpha;
    float i_beta = motor->currentSense->i_beta;

    motor->currentSense->i_d = i_alpha * _cos_e + i_beta * _sin_e;
    motor->currentSense->i_q = -i_alpha * _sin_e + i_beta * _cos_e;
    motor->currentSense->i_d = lpf_process(&motor->lpf_id0, motor->currentSense->i_d);
    motor->currentSense->i_q = lpf_process(&motor->lpf_iq0, motor->currentSense->i_q);
    // motor->currentSense->i_d =LowPassFilter(&motor->lpf_id, motor->currentSense->i_d);
    // motor->currentSense->i_q =LowPassFilter(&motor->lpf_iq, motor->currentSense->i_q);
    motor->CurrentSense.i_a = motor->currentSense->i_a;
    motor->CurrentSense.i_b = motor->currentSense->i_b;
    motor->CurrentSense.i_alpha = motor->currentSense->i_alpha;
    motor->CurrentSense.i_beta = motor->currentSense->i_beta;
    motor->CurrentSense.i_d = motor->currentSense->i_d;
    motor->CurrentSense.i_q = motor->currentSense->i_q;
    return 0;
}
float BLDCMotor_getCurrentDQ(BLDCMotor_t *motor)
{


    // motor->currentSense->i_a = lpf_process(&motor->lpf_ia0, motor->currentSense->i_a);
    // motor->currentSense->i_b = lpf_process(&motor->lpf_ib0, motor->currentSense->i_b);
    motor->currentSense->i_a =LowPassFilter(&motor->lpf_ia, motor->currentSense->i_a);
    motor->currentSense->i_b =LowPassFilter(&motor->lpf_ib, motor->currentSense->i_b);
    motor->currentSense->i_alpha = motor->currentSense->i_a;
    motor->currentSense->i_beta = _1_SQRT3*motor->currentSense->i_a + _2_SQRT3 *motor->currentSense->i_b;

    float _cos_e = _cos(motor->e_angle);
    float _sin_e = _sin(motor->e_angle);
    float i_alpha = motor->currentSense->i_alpha;
    float i_beta = motor->currentSense->i_beta;

    motor->currentSense->i_d = i_alpha * _cos_e + i_beta * _sin_e;
    motor->currentSense->i_q = -i_alpha * _sin_e + i_beta * _cos_e;
    // motor->currentSense->i_d = lpf_process(&motor->lpf_id0, motor->currentSense->i_d);
    // motor->currentSense->i_q = lpf_process(&motor->lpf_iq0, motor->currentSense->i_q);
    motor->currentSense->i_d =LowPassFilter(&motor->lpf_id, motor->currentSense->i_d);
    motor->currentSense->i_q =LowPassFilter(&motor->lpf_iq, motor->currentSense->i_q);
    motor->CurrentSense.i_a = motor->currentSense->i_a;
    motor->CurrentSense.i_b = motor->currentSense->i_b;
    motor->CurrentSense.i_alpha = motor->currentSense->i_alpha;
    motor->CurrentSense.i_beta = motor->currentSense->i_beta;
    motor->CurrentSense.i_d = motor->currentSense->i_d;
    motor->CurrentSense.i_q = motor->currentSense->i_q;
    return 0;
}

// int BLDCMotor_alignSensor(BLDCMotor_t *motor)
// {
//     if (!motor)
//         return 0;
//
//     //  if(monitor_port) monitor_port->println("MOT: Align sensor.");
//     // align the electrical phases of the motor and sensor
//     // set angle -90 degrees
//
//     float start_angle = FOCMotor_shaftAngle(&motor->foc_motor);
//     for (int i = 0; i <= 5; i++)
//     {
//         float angle = _3PI_2 + _2PI * i / 6.0f;
//         BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_sensor_align, 0, angle);
//         _delay(200);
//     }
//     float mid_angle = FOCMotor_shaftAngle(&motor->foc_motor);
//     for (int i = 5; i >= 0; i--)
//     {
//         float angle = _3PI_2 + _2PI * i / 6.0f;
//         BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_sensor_align, 0, angle);
//         _delay(200);
//     }
//     if (mid_angle < start_angle)
//     {
//         //    if(monitor_port) monitor_port->println("MOT: natural_direction==CCW");
//         if (motor->foc_motor.sensor)
//         {
//             motor->foc_motor.sensor->natural_direction = Direction_CCW;
//         }
//     }
//     else if (mid_angle == start_angle)
//     {
//         //    if(monitor_port) monitor_port->println("MOT: Sensor failed to notice
//         //    movement");
//     }
//     else
//     {
//         //    if(monitor_port) monitor_port->println("MOT: natural_direction==CW");
//     }
//
//     // let the motor stabilize for 2 sec
//     _delay(2000);
//     return 0;
// }
/**
 * @brief 带自动验证和修正的电角度对齐函数
 * @return 0=成功, -1=失败
 */
int BLDCMotor_alignSensor_AutoCorrect(BLDCMotor_t *motor)
{
    if (!motor) return -1;
    BLDCMotor_enable(motor);

    printf("\n=== Electric Angle Alignment with Auto-Correction ===\n");

    // ========== 第一部分：标准对齐流程 ==========
    printf("[Step 1] Standard alignment procedure...\n");

    // 清除历史状态
    motor->angle = 0;
    motor->lastAngle = 0;
    if (motor->foc_motor.encoder)
    {
        motor->foc_motor.encoder->prev_Th = 0;
        motor->foc_motor.encoder->prev_timestamp_us = getMicros();
    }

    // 选择对齐角度（270度）
    float target_electrical_angle = _3PI_2;
    float align_voltage = 6.0f;
    if (align_voltage < 0.5f) align_voltage = 6.0f;

    // 渐增电压，强制转子到指定位置
    printf("[Step 1.1] Forcing rotor to alignment position (%.1f V)...\n", align_voltage);
    for (int i = 1; i <= 10; i++)
    {
        float voltage = align_voltage * i / 10.0f;
        BLDCMotor_setPhaseVoltage(motor, voltage, 0, target_electrical_angle);
        _delay(50);
    }

    // 保持全电压
    for (int i = 0; i < 20; i++)
    {
        BLDCMotor_setPhaseVoltage(motor, align_voltage, 0, target_electrical_angle);
        _delay(50);
    }

    _delay(500);  // 额外稳定时间

    // 多次采样求平均
    printf("[Step 1.2] Reading encoder position (50 samples average)...\n");
    float angle_sum = 0;
    const int samples = 50;

    for (int i = 0; i < samples; i++)
    {
        BLDCMotor_setPhaseVoltage(motor, align_voltage, 0, target_electrical_angle);
        _delay(10);
        float angle = FOCMotor_shaftAngle(&motor->foc_motor);
        angle_sum += angle;
    }

    float shaft_angle = angle_sum / samples;
    printf("[Step 1.2] Measured shaft angle: %.4f rad (%.2f deg)\n",
           shaft_angle, shaft_angle * 180.0f / _PI);

    // 计算零点偏移
    float electrical_angle = _electricalAngle(shaft_angle, motor->foc_motor.pole_pairs);
    motor->foc_motor.zero_electric_angle = _normalizeAngle(
        target_electrical_angle - electrical_angle
    );

    printf("[Step 1.3] Initial zero_electric_angle: %.4f rad (%.2f deg)\n",
           motor->foc_motor.zero_electric_angle,
           motor->foc_motor.zero_electric_angle * 180.0f / _PI);

    // ========== 第二部分：自动验证和修正 ==========
    printf("\n[Step 2] Auto-correction verification...\n");

    int correction_attempts = 0;
    const int max_attempts = 6;
    bool alignment_correct = false;

    while (correction_attempts < max_attempts && !alignment_correct)
    {
        correction_attempts++;
        printf("\n[Attempt %d/%d] Testing alignment correctness...\n",
               correction_attempts, max_attempts);

        // 关闭电压，让电机自由
        BLDCMotor_setPhaseVoltage(motor, 0, 0, 0);
        _delay(500);

        // 记录起始位置
        float start_angle = FOCMotor_shaftAngle(&motor->foc_motor);
        printf("  Start angle: %.4f rad (%.2f deg)\n",
               start_angle, start_angle * 180.0f / _PI);

        // 施加正的Vq（理论上应该正转）
        float test_voltage = 2.0f;  // 2V测试电压
        printf("  Applying +%.1fV Vq for 2 seconds...\n", test_voltage);

        for (int i = 0; i < 100; i++)  // 2秒（20ms × 100）
        {
            float current_angle = FOCMotor_shaftAngle(&motor->foc_motor);
            float e_angle = _normalizeAngle(
                _electricalAngle(current_angle, motor->foc_motor.pole_pairs) +
                motor->foc_motor.zero_electric_angle
            );

            // 施加正Vq，Ud=0
            BLDCMotor_setPhaseVoltage(motor, test_voltage, 0, e_angle);
            _delay(20);
        }

        // 记录结束位置
        float end_angle = FOCMotor_shaftAngle(&motor->foc_motor);
        printf("  End angle: %.4f rad (%.2f deg)\n",
               end_angle, end_angle * 180.0f / _PI);

        // 计算角度变化（处理2π跳变）
        float rotation = end_angle - start_angle;
        if (rotation > _PI) rotation -= _2PI;
        if (rotation < -_PI) rotation += _2PI;

        printf("  Net rotation: %.4f rad (%.2f deg)\n",
               rotation, rotation * 180.0f / _PI);

        // 判断方向
        const float rotation_threshold = 0.3f;  // 至少转17度才算有效

        if (rotation > rotation_threshold)
        {
            // 正转：正确！
            printf("  ✅ Direction: Forward (Correct!)\n");
            alignment_correct = true;
        }
        else if (rotation < -rotation_threshold)
        {
            // 反转：需要修正
            printf("  ❌ Direction: Backward (Wrong!)\n");
            printf("  🔧 Auto-correcting: Adding π to zero_electric_angle...\n");

            motor->foc_motor.zero_electric_angle = _normalizeAngle(
                motor->foc_motor.zero_electric_angle + _PI
            );

            printf("  New zero_electric_angle: %.4f rad (%.2f deg)\n",
                   motor->foc_motor.zero_electric_angle,
                   motor->foc_motor.zero_electric_angle * 180.0f / _PI);
        }
        else
        {
            // 几乎不动：可能电压不够或有负载
            printf("  ⚠️  Motor barely moved (rotation < %.1f deg)\n",
                   rotation_threshold * 180.0f / _PI);
            printf("  Possible causes:\n");
            printf("    - Voltage too low (current: %.1fV)\n", test_voltage);
            printf("    - Mechanical load too high\n");
            printf("    - Motor not properly connected\n");

            // 增加电压重试
            if (correction_attempts < max_attempts)
            {
                test_voltage += 1.0f;
                printf("  Increasing test voltage to %.1fV for next attempt...\n", test_voltage);
                correction_attempts--;  // 不计入尝试次数
            }
        }

        // 停止电机
        BLDCMotor_setPhaseVoltage(motor, 0, 0, 0);
        _delay(500);
    }

    // ========== 第三部分：最终验证 ==========
    if (alignment_correct)
    {
        printf("\n[Step 3] Final verification test...\n");

        // 再次测试正反转
        BLDCMotor_setPhaseVoltage(motor, 0, 0, 0);
        _delay(500);

        float verify_start = FOCMotor_shaftAngle(&motor->foc_motor);

        // 测试反转（Vq < 0）
        printf("  Testing reverse direction (Vq = -2V)...\n");
        for (int i = 0; i < 100; i++)
        {
            float current_angle = FOCMotor_shaftAngle(&motor->foc_motor);
            float e_angle = _normalizeAngle(
                _electricalAngle(current_angle, motor->foc_motor.pole_pairs) +
                motor->foc_motor.zero_electric_angle
            );
            BLDCMotor_setPhaseVoltage(motor, -2.0f, 0, e_angle);
            _delay(20);
        }

        float verify_end = FOCMotor_shaftAngle(&motor->foc_motor);
        float verify_rotation = verify_end - verify_start;
        if (verify_rotation > _PI) verify_rotation -= _2PI;
        if (verify_rotation < -_PI) verify_rotation += _2PI;

        printf("  Reverse rotation: %.2f deg\n", verify_rotation * 180.0f / _PI);

        if (verify_rotation < -0.3f)
        {
            printf("  ✅ Reverse direction also correct!\n");
        }
        else
        {
            printf("  ⚠️  Reverse test inconclusive\n");
        }
    }

    // 关闭所有输出
    BLDCMotor_setPhaseVoltage(motor, 0, 0, 0);
    _delay(500);

    // ========== 总结 ==========
    printf("\n=== Alignment Summary ===\n");
    if (alignment_correct)
    {
        printf("✅ Status: SUCCESS (after %d attempt(s))\n", correction_attempts);
        printf("✅ Final zero_electric_angle: %.4f rad (%.2f deg)\n",
               motor->foc_motor.zero_electric_angle,
               motor->foc_motor.zero_electric_angle * 180.0f / _PI);
        printf("✅ Motor ready for closed-loop control\n\n");
        return 0;
    }
    else
    {
        printf("❌ Status: FAILED\n");
        printf("❌ Could not verify alignment after %d attempts\n", max_attempts);
        printf("❌ Please check:\n");
        printf("   1. Motor connections (phase wiring)\n");
        printf("   2. Encoder connections and magnet position\n");
        printf("   3. Power supply voltage (should be > 12V)\n");
        printf("   4. Motor pole pairs setting (current: %d)\n",
               motor->foc_motor.pole_pairs);
        printf("\n");
        return -1;
    }
}
int BLDCMotor_alignSensor(BLDCMotor_t *motor)
{
    if (!motor)
        return 0;
    const int delay_t = 30;
    const int resolution = 40;

    BLDCMotor_enable(motor);
    motor->foc_motor.voltage_sensor_align = 1.0f;
    printf("[Align] Starting sensor alignment...\n");

    // ========== 步骤1：将电机转到固定电角度位置 ==========
    // 使用270度（3π/2）作为校准位置
    const float target_electrical_angle = _3PI_2;  // 270度
    // const int resolution = 20;

    printf("[Align] Moving motor to alignment position (270 deg electrical)...\n");

    // 施加电压，让电机转到270度电角度位置
    for (int i = 0; i < 5; i++)
    {
        // BLDCMotor_setPhaseVoltage(motor,0, motor->foc_motor.voltage_sensor_align, target_electrical_angle);
        // BLDCMotor_setPhaseVoltage(motor,  motor->foc_motor.voltage_sensor_align,0, target_electrical_angle);
        BLDCMotor_SVPWM(motor,  motor->foc_motor.voltage_sensor_align,0, target_electrical_angle);
        // BLDCMotor_SVPWM(motor,  motor->foc_motor.voltage_sensor_align,0, target_electrical_angle);
        _delay(delay_t);  // 等待电机稳定
    }

    _delay(500);  // 额外延迟确保完全稳定

    // ========== 步骤2：读取此时的机械角度 ==========
    float shaft_angle = FOCMotor_shaftAngle(&motor->foc_motor);
    printf("[Align] Shaft angle at alignment position: %.4f rad (%.2f deg)\n",
           shaft_angle, shaft_angle * 180.0f / 3.14159265f);

    // ========== 步骤3：计算电角度零点 ==========
    // zero_electric_angle = 目标电角度 - (机械角度 × 极对数)
    float electrical_angle = _electricalAngle(shaft_angle, motor->foc_motor.pole_pairs);
    // motor->foc_motor.zero_electric_angle = _normalizeAngle(target_electrical_angle-electrical_angle);
    motor->foc_motor.zero_electric_angle = electrical_angle;

    printf("[Align]  zero_electric_angle = %.4f rad (%.2f deg)\n",
           motor->foc_motor.zero_electric_angle,
           motor->foc_motor.zero_electric_angle * 180.0f / 3.14159265f);
    if (electrical_angle > _PI) {
        electrical_angle = _2PI - electrical_angle;
    }
    motor->foc_motor.zero_electric_angle = electrical_angle;

    // ========== 步骤4：测试方向（可选） ==========
    float start_angle = shaft_angle;
    delay_ms(2000);

    printf("[Align] Testing motor direction...\n");

    // 正向转动60度
    for (int i = 0; i <= resolution; i++)
    {
        float angle = _3PI_2 + _2PI * i / (resolution+1);
        // angle = _normalizeAngle(angle-motor->foc_motor.zero_electric_angle);
        angle = _normalizeAngle(angle);
        // BLDCMotor_setPhaseVoltage(motor,  motor->foc_motor.voltage_sensor_align,0, target_electrical_angle);
        BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_sensor_align, 0, angle);
        // BLDCMotor_SVPWM(motor, 0, motor->foc_motor.voltage_sensor_align, angle);
        _delay(delay_t);
    }
    float end_angle = FOCMotor_shaftAngle(&motor->foc_motor);
    float deltaAngle = end_angle - start_angle;
    printf("[Align] start angle = %f, end angle = %f, delta = %f\n", start_angle, end_angle, end_angle-start_angle);

    for (int i = 0; i <= resolution; i++)
    {
        float angle = _3PI_2 + _2PI *(resolution - i) / (resolution+1);
        angle = _normalizeAngle(angle);
        // BLDCMotor_setPhaseVoltage(motor,  motor->foc_motor.voltage_sensor_align,0, target_electrical_angle);
        BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_sensor_align, 0, angle);
        // BLDCMotor_SVPWM(motor, 0, motor->foc_motor.voltage_sensor_align, angle);
        _delay(delay_t);
    }

    if (fabsf(deltaAngle) > 0.8*_2PI)
    {
        if ((deltaAngle) > 0)
        {
            printf("[Align] Motor direction: CCW (Counter-Clockwise)\n");
            motor->direction = Direction_CCW;
        }
        else
        {
            printf("[Align] Motor direction: CW (Counter-Clockwise)\n");
            motor->direction = Direction_CW;
        }

    }
    else
    {
        if (end_angle > start_angle)
        {
            printf("[Align] Motor direction: CW (Clockwise)\n");
            motor->direction = Direction_CW;
        }
        else if (end_angle < start_angle)
        {
            printf("[Align] Motor direction: CCW (Counter-Clockwise)\n");
            motor->direction = Direction_CCW;
        }
        else
        {
            printf("[Align] Warning: Sensor failed to notice movement!\n");
        }

    }

    // ========== 步骤5：关闭电压，让电机自由停止 ==========
    // BLDCMotor_setPhaseVoltage(motor, 3, 0, 0);

    printf("[Align] Alignment complete! Waiting for motor to stabilize...\n");
    _delay(1000);

    return 0;
}
// Iterative function looping FOC algorithm, setting Uq on the Motor
// The faster it can be run the better
void BLDCMotor_loopFOC(BLDCMotor_t *motor)
{
    if (!motor)
        return;

    // shaft angle
    motor->foc_motor.shaft_angle = FOCMotor_shaftAngle(&motor->foc_motor);
    // set the phase voltage - FOC heart function :)
    BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, _electricalAngle(motor->foc_motor.shaft_angle, motor->foc_motor.pole_pairs));
}
void BLDCMotor_move(BLDCMotor_t *motor, float new_target)
{
    if (!motor)
        return;

    // printf("hello move\n");
    // set internal target variable
    if (new_target != NOT_SET)
        motor->target = new_target;
    switch (motor->foc_motor.controller)
    {
    case ControlType_voltage:
        break;
    case ControlType_current:
        break;
    case ControlType_angle:
        break;
    case ControlType_angleClosedLoop:
        BLDCMotor_angleClosedLoop(motor, new_target);
        break;
    case ControlType_velocity:
        // BLDCMotor_velocityClosedLoop_without_i(motor, new_target);
        BLDCMotor_velocityClosedLoop(motor,new_target);
        break;
    case ControlType_velocityAngleClosedLoop:
        BLDCMotor_velocityAngleClosedLoop(motor, new_target);
        break;
    case ControlType_velocityClosedLoopInline:
        // BLDCMotor_velocityClosedLoop_without_i(motor, new_target);
        BLDCMotor_velocityClosedLoopInline(motor,new_target);
        break;
    case ControlType_velocity_openloop:
        BLDCMotor_velocityOpenloop(motor, new_target);
        break;
    case ControlType_currentVelocityOpenLoop:
        BLDCMotor_currentVelocityOpenloop(motor, new_target);
        break;
    case ControlType_currentVelocityOpenLoopInline:
        BLDCMotor_currentVelocityOpenloopInline(motor, new_target);
        break;
    case ControlType_angle_openloop:
        motor->foc_motor.shaft_angle_sp = motor->foc_motor.target;
        break;
    case ControlType_velocityClosedLoop:
        BLDCMotor_velocityClosedLoop(motor, new_target);
        break;
    case ControlType_currentClosedLoop:
        //1.
        // BLDCMotor_currentClosedLoop(motor, new_target);
        //2.
        BLDCMotor_currentClosedLoop_with_bandwidth(motor, new_target);
        //3.
        // BLDCMotor_positionCascade_with_bandwidth(motor, new_target);
        break;
    case ControlType_currentVelocityClosedLoop:
        BLDCMotor_currentVelocityClosedLoop(motor, new_target);
        break;
    case ControlType_currentAngleClosedLoopBandwidth:
        BLDCMotor_currentAngleClosedLoopBandwith(motor, new_target);
        break;
    case ControlType_currentVelocityAngleClosedLoop:
        BLDCMotor_currentVelocityAngleClosedLoop(motor, new_target);
        break;
    case ControlType_currentClosedLoopBandwidth:
        BLDCMotor_currentClosedLoopBandwith(motor, new_target);
        break;
    case ControlType_currentClosedLoopBandwidthInline:
        BLDCMotor_currentClosedLoopBandwithInline(motor, new_target);
        break;
    case ControlType_currentVelocityClosedLoopBandwidth:
        BLDCMotor_currentVelocityClosedLoopBandwith(motor, new_target);
        break;
    case ControlType_currentVelocityRPMClosedLoopBandwidth:
        BLDCMotor_currentVelocityRPMClosedLoopBandwith(motor, new_target);
        break;
    case ControlType_currentVelocityAngleClosedLoopBandwidth:
        BLDCMotor_currentVelocityAngleClosedLoopBandwith(motor, new_target);
        break;
    case ControlType_currentClosedLoopBandwidth_debug:
        BLDCMotor_currentClosedLoopBandwith_debug(motor, new_target);
        break;
    default:
        printf("other controller type\n");
        break;
    }
}

void BLDCMotor_velocityOpenloop_with_timer(BLDCMotor_t *motor, float target_velocity)
{
    if (!motor)
        return;

    float Ts = motor->pid_dt * 1e-3f;
    float angle = _normalizeAngle(motor->angle + target_velocity * Ts);
    motor->angle = angle;
    motor->velocity = (motor->angle - motor->lastAngle)/Ts;
    motor->lastAngle = angle;
    float e_angle = _electricalAngle(angle, motor->foc_motor.pole_pairs);
    motor->e_angle = e_angle;
    BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_limit, 0, e_angle);
}
void BLDCMotor_velocityOpenloop(BLDCMotor_t *motor, float target_velocity)
{
    if (!motor)
        return;
    // get current timestamp
    unsigned long now_us = getUs();
    // calculate the sample time from last call
    float Ts = (now_us - motor->open_loop_timestamp) * 1e-6f;

    float velocity_radian = target_velocity * _2PI;
    float angle = _normalizeAngle(motor->angle + velocity_radian* Ts);
    motor->angle = angle;
    // float e_angle = _normalizeAngle(_electricalAngle(angle, motor->foc_motor.pole_pairs));
    float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
    motor->e_angle = e_angle;
    // BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_limit, 0, e_angle);
    // BLDCMotor_SVPWM(motor, 2, 0, e_angle);
    BLDCMotor_setPhaseVoltage(motor, 0.5, 0, e_angle);
    motor->open_loop_timestamp = now_us;
}
void BLDCMotor_currentVelocityOpenloop(BLDCMotor_t *motor, float target_velocity)
{
    if (!motor)
        return;
    // get current timestamp
    // while (!motor->currentSense->isReady);
    if (motor->currentSense->isReady)
    {
        // uint32_t now_us = getUs();
        uint32_t now_us = HAL_GetTickUs();
        motor->velocity  = BLDCMotor_getVelocity(motor);
        motor->velocity  = lpf_process(&motor->filter_velocity, motor->velocity);
        // calculate the sample time from last call
        // float Ts_us =(float)(now_us - motor->open_loop_timestamp) ;
        uint32_t Ts_us =  (now_us - motor->open_loop_timestamp) ;
        // if (Ts_us > 10000) {
        //     Ts_us = 10000;
        // }
        motor->delta_us = Ts_us;
        float Ts = (float)Ts_us * 1e-6f;

        float velocity_radian = target_velocity * _2PI;
        float angle = _normalizeAngle(motor->angle + velocity_radian* Ts);
        // float angle = (motor->angle + velocity_radian* Ts);
        motor->angle = angle;
        // float e_angle = _normalizeAngle(_electricalAngle(angle, motor->foc_motor.pole_pairs));
        // e_angle = _normalizeAngle(e_angle + _PI_2);
        float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
        motor->e_angle = e_angle;
        // BLDCMotor_getCurrentDQ(motor);
        // BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_limit, 0, e_angle);
        // BLDCMotor_SVPWM(motor, 3.0f, 0, e_angle);
        BLDCMotor_setPhaseVoltage(motor, 3, 0, e_angle);

        motor->open_loop_timestamp = now_us;
        motor->currentSense->isReady = false;
    }
}
float now_e_angle = _PI_2*0.5;
float last_e_angle;
struct g_I {
    float a,b,c,alpha,beta,d,q;
}g_I;
void BLDCMotor_currentVelocityOpenloopInline(BLDCMotor_t *motor, float target_velocity)
{
    if (!motor)
        return;
    // get current timestamp
    // if (motor->currentSense->isReady)
    {
        // calculate the sample time from last call
        // currentSense_pollingGetCurrent(motor->currentSense);
        // motor->currentSense->i_a = lpf_process(&motor->lpf_ia0, motor->currentSense->i_a);
        // motor->currentSense->i_b = lpf_process(&motor->lpf_ib0, motor->currentSense->i_b);
        // Clark_Park_Transform(motor->currentSense->i_a, motor->currentSense->i_b,
        //                      &motor->currentSense->i_d, &motor->currentSense->i_q,
        //                      motor->e_angle);
        // motor->currentSense->i_d = lpf_process(&motor->lpf_id0, motor->currentSense->i_d);
        // motor->currentSense->i_q = lpf_process(&motor->lpf_iq0, motor->currentSense->i_q);
        //
        // motor->CurrentSense.i_a = motor->currentSense->i_a;
        // motor->CurrentSense.i_b = motor->currentSense->i_b;
        // motor->CurrentSense.i_d = motor->currentSense->i_d;
        // motor->CurrentSense.i_q = motor->currentSense->i_q;

       // UART_SendFloat(4, motor->currentSense->i_a, motor->currentSense->i_b, motor->currentSense->i_d, motor->currentSense->i_q);

        // static int isFirst = 1;
        // if (isFirst) {
        //     motor->angle = BLDCMotor_getAngle(motor);
        //     isFirst = 0;
        // }
        unsigned long now_us = getUs();
        float Ts_us =(float)(now_us - motor->open_loop_timestamp) ;
        float Ts = Ts_us * 1e-6f;
        if (Ts > 0.5 || Ts == 0) {
            Ts = 1e-3f;
        }
        // float shaft_angle = BLDCMotor_getAngle(motor);
        motor->velocity = BLDCMotor_getVelocity(motor);
        // now_e_angle = _normalizeAngle(_electricalAngle(shaft_angle, motor->foc_motor.pole_pairs));
        // now_e_angle = _electricalAngle_calibrated(motor->direction, shaft_angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);

        float velocity_radian = target_velocity * _2PI;
        // float angle = motor->foc_motor.Encoder.angle;
        float angle =  _normalizeAngle(motor->angle + velocity_radian* Ts);
        motor->angle = angle;
        float e_angle = _normalizeAngle(_electricalAngle(angle, motor->foc_motor.pole_pairs));
        // float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
        motor->e_angle = e_angle;

        // currentSense_pollingGetCurrent(motor->currentSense);
        BLDCMotor_getCurrentDQ(motor);
        // motor->currentSense->i_a = lpf_process(&motor->lpf_ia0, motor->currentSense->i_a);
        // motor->currentSense->i_b = lpf_process(&motor->lpf_ib0, motor->currentSense->i_b);

        motor->CurrentSense.i_a = motor->currentSense->i_a;
        motor->CurrentSense.i_b = motor->currentSense->i_b;
        motor->CurrentSense.i_alpha = motor->currentSense->i_alpha;
        motor->CurrentSense.i_beta = motor->currentSense->i_beta;
        motor->CurrentSense.i_d = motor->currentSense->i_d;
        motor->CurrentSense.i_q = motor->currentSense->i_q;


        // BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_limit/2, 0, e_angle);
        // BLDCMotor_SVPWM(motor, 0, motor->foc_motor.voltage_limit/2, e_angle);
        BLDCMotor_setPhaseVoltage(motor, 3, 0, e_angle);


        motor->open_loop_timestamp = now_us;
        // UART_SendFloat(1, Ts_us);
    }
}
int isFirst = 1;
int BLDCMotor_velocityClosedLoop(BLDCMotor_t *motor, float target)
{
    if (motor->currentSense->isReady) {
        motor->currentSense->isReady = false;

    motor->velocity = BLDCMotor_getVelocity(motor);
    motor->velocity = _constrain(motor->velocity, -20, 20);

    // motor->velocity =LowPassFilter(&motor->lpf_velocity,motor->velocity);
        motor->velocity =lpf_process(&motor->filter_velocity,motor->velocity);

    motor->angle = motor->foc_motor.Encoder.angle;

    float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
    motor->e_angle = e_angle;
    // BLDCMotor_getCurrentDQ(motor);
    // motor->velocity =lpf_process(&motor->filter_velocity, motor->velocity);

    motor->foc_motor.voltage_q = PIDController_update(&motor->PID_velocity, target - motor->velocity);
    BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, 0,(e_angle));
    // BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_q, 0, e_angle);
    }
    return 0;
}
int BLDCMotor_Monitor(BLDCMotor_t *motor, MotorMonitor_Type monitorType) {
    float monitorBuffer[10] = {0};
    int len;
    if (monitorType == Monitor_NULL) {
        printf("[note] Monitor is Disabled\n");
        return 0;
    }
    switch (monitorType) {
    case Monitor_Velocity:
        monitorBuffer[0]  = motor->target;
        monitorBuffer[1]  = motor->velocity;
        len = 2;
        break;
    case Monitor_Angle:
        monitorBuffer[0]  = motor->target;
        monitorBuffer[1]  = motor->angle;
        len = 2;
        break;
    case Monitor_IdIq:
        monitorBuffer[0]  = motor->currentSense->i_d;
        monitorBuffer[1]  = motor->currentSense->i_q;
        len = 2;
        break;
    case Monitor_TargetIdIq:
        monitorBuffer[0]  = motor->target;
        monitorBuffer[1]  = motor->currentSense->i_d;
        monitorBuffer[2]  = motor->currentSense->i_q;
        monitorBuffer[3]  = motor->foc_motor.voltage_q;
        len = 4;
        break;
    case Monitor_IaIbIdIqUabc:
        monitorBuffer[0]  = motor->currentSense->i_a;
        monitorBuffer[1]  = motor->currentSense->i_b;
        monitorBuffer[2]  = motor->currentSense->i_d;
        monitorBuffer[3]  = motor->currentSense->i_q;
        monitorBuffer[4]  = motor->Driver.Ua;
        monitorBuffer[5]  = motor->Driver.Ub;
        monitorBuffer[6]  = motor->Driver.Uc;
        monitorBuffer[7]  = motor->e_angle;
        len = 8;
        break;
    case Monitor_IalphaIbetaUabc:
        monitorBuffer[0]  = motor->currentSense->i_alpha;
        monitorBuffer[1]  = motor->currentSense->i_beta;
        monitorBuffer[2]  = motor->currentSense->i_d;
        monitorBuffer[3]  = motor->currentSense->i_q;
        monitorBuffer[4]  = motor->Driver.Ua;
        monitorBuffer[5]  = motor->Driver.Ub;
        monitorBuffer[6]  = motor->Driver.Uc;
        monitorBuffer[7]  = motor->e_angle;
        len = 8;
        break;
    case Monitor_IaIbIdIq:
        monitorBuffer[0]  = motor->currentSense->i_a;
        monitorBuffer[1]  = motor->currentSense->i_b;
        monitorBuffer[2]  = motor->currentSense->i_d;
        monitorBuffer[3]  = motor->currentSense->i_q;
        len = 4;
        break;
    case Monitor_UaUbUc:
        monitorBuffer[0]  = motor->Driver.Ua;
        monitorBuffer[1]  = motor->Driver.Ub;
        monitorBuffer[2]  = motor->Driver.Uc;
        len = 3;
        break;
    default:
        monitorBuffer[0]  = motor->currentSense->i_a;
        monitorBuffer[1]  = motor->currentSense->i_b;
        monitorBuffer[2]  = motor->currentSense->i_d;
        monitorBuffer[3]  = motor->currentSense->i_q;
        len = 4;
        break;
    }
    // VOFA_SendJustFloat(monitorBuffer, len);
    VOFA_SendJustFloat_DMA(monitorBuffer, len);

}
int BLDCMotor_velocityAngleClosedLoop(BLDCMotor_t *motor, float target)
{
    if (motor->currentSense->isReady) {
        motor->currentSense->isReady = false;

        static float target_velocity = 0;
        static  int velocityLoop_counter = 0;
        const int velocityLoop_period = 1;
        motor->velocity = BLDCMotor_getVelocity(motor);
        motor->velocity = _constrain(motor->velocity, -motor->PID_velocity.limit, motor->PID_velocity.limit);

        // motor->velocity =LowPassFilter(&motor->lpf_velocity,motor->velocity);

        motor->angle = motor->foc_motor.Encoder.angle;
        motor->degree = motor->angle / _2PI;

        float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
        motor->e_angle = e_angle;
        // BLDCMotor_getCurrentDQ(motor);
        // motor->velocity =lpf_process(&motor->filter_velocity, motor->velocity);
        motor->velocity =lpf_process(&motor->filter_velocity,motor->velocity);
        // motor->velocity = LowPassFilter(&motor->lpf_velocity, motor->velocity);

        if (velocityLoop_counter++ % velocityLoop_period == 0)
        {
            // motor->velocity =lpf_process(&motor->filter_velocity, velocity);

            target_velocity = PIDController_update(&motor->PID_degree, target - motor->degree);
            // motor->target_iq = target_iq;
        }

        motor->foc_motor.voltage_q = PIDController_update(&motor->PID_velocity, target_velocity - motor->velocity);
        BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, 0,(e_angle));
        // BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_q, 0, e_angle);
    }
    return 0;
}
int BLDCMotor_velocityClosedLoopInline(BLDCMotor_t *motor, float target)
{
    motor->velocity = BLDCMotor_getVelocity(motor);
    // if (motor->velocity < 1) {
    //     UART_SendFloat(2, motor->velocity, (float)motor->foc_motor.Encoder.mt6835.delta_t_us);
    // }
    motor->angle = motor->foc_motor.Encoder.angle;

    float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
    motor->e_angle = e_angle;
    BLDCMotor_getCurrentDQ(motor);

    motor->foc_motor.voltage_q = PIDController_update(&motor->PID_velocity, target - motor->velocity);
    // BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, 0,(e_angle));
    BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_q, 0, e_angle);
    return 0;
}

int BLDCMotor_velocityClosedLoop_0(BLDCMotor_t *motor, float target)
{

    //1.
    // motor->velocity = BLDCMotor_getVelocityRPM(motor)/60;
    // // motor->velocity = BLDCMotor_getVelocityRPM(motor)/60;
    // // motor->angle = BLDCMotor_getAngle(motor);
    // motor->angle = motor->foc_motor.encoder->as5600_Instance.angle;
    //2.
    motor->velocity = BLDCMotor_getVelocity(motor);
    motor->angle = BLDCMotor_getAngle(motor);
    // BLDCMotor_getCurrentDQ_noFilter(motor);

    // motor->angle = motor->foc_motor.encoder->as5600_Instance.angle;
    // float e_angle = _normalizeAngle(MOTOR_DIRECTION*_electricalAngle(motor->angle, motor->foc_motor.pole_pairs) + motor->foc_motor.zero_electric_angle);
    float e_angle = _normalizeAngle(1*_electricalAngle(motor->angle, motor->foc_motor.pole_pairs) + motor->foc_motor.zero_electric_angle);
    // e_angle = _normalizeAngle(e_angle + _PI_2);


    // angle_el = _normalizeAngle(angle_el + motor->foc_motor.zero_electric_angle);
    // motor->e_angle = e_angle;
    // if (motor->velocity<0)
    // {
    //     motor->e_angle  = -motor->e_angle;
    // }
    motor->foc_motor.voltage_q = PIDController_update(motor->pid, target - motor->velocity);
    // BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, 0, _normalizeAngle(e_angle));
    BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, 0,(e_angle));
    if (isFirst++ < 15)
    {

        printf("velocity = %f, angle = %f, vq = %f, vd = %f\n", motor->velocity, motor->angle, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d);
        printf("e_angle = %f, Ua: %f, Ub: %f, Uc: %f\n\n", motor->e_angle, motor->Ua, motor->Ub, motor->Uc);
        // isFirst = 0;

    }
    // DEBUG_PRINT("e angle = %f", e_angle);
    // DEBUG_PRINT("pp  = %d", motor->foc_motor.pole_pairs);
    return 0;
}
int BLDCMotor_velocityClosedLoop_v1(BLDCMotor_t *motor, float target)
{
    float target_iq, target_id;
    motor->velocity = BLDCMotor_getVelocityRPM(motor);
    // printf("vel = %f\n", motor->velocity);
    BLDCMotor_getElectricalAngle(motor);
    BLDCMotor_getCurrentDQ(motor);
    motor->velocity = LowPassFilter(&motor->foc_motor.LPF_velocity, motor->velocity);
    motor->foc_motor.i_d = LowPassFilter(&motor->foc_motor.LPF_current_d, motor->foc_motor.i_d);
    motor->foc_motor.i_q = LowPassFilter(&motor->foc_motor.LPF_current_q, motor->foc_motor.i_q);

    target_iq = PIDController_update(motor->foc_motor.PID_velocityLoop, target - motor->velocity);
    motor->foc_motor.voltage_q = PIDController_update(motor->foc_motor.PID_current_q, target_iq - motor->foc_motor.i_q);
    motor->foc_motor.voltage_d = PIDController_update(motor->foc_motor.PID_current_d, 0 - motor->foc_motor.i_d);
    // 1.
    //  BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, motor->e_angle);
    // 2.
    BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, motor->e_angle);
    // printf("%f, %f, %f, %f, %f\n", motor->foc_motor.i_q, motor->foc_motor.i_d, target, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d);

    return 0;
}
int BLDCMotor_velocityClosedLoop_without_i(BLDCMotor_t *motor, float target)
{
    motor->velocity = BLDCMotor_getVelocityRPM(motor);
    // printf("vel = %f\n", motor->velocity);
    BLDCMotor_getElectricalAngle(motor);
    motor->velocity = LowPassFilter(&motor->foc_motor.LPF_velocity, motor->velocity);

    motor->foc_motor.voltage_q = PIDController_update(motor->foc_motor.PID_velocityLoop, target - motor->velocity);
    // 1.
    //  BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, motor->e_angle);
    // 2.
    BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, 0, motor->e_angle);
    // printf("%f, %f, %f, %f, %f\n", motor->foc_motor.i_q, motor->foc_motor.i_d, target, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d);

    return 0;
}
int BLDCMotor_currentClosedLoop_v0(BLDCMotor_t *motor, float target)
{

    motor->target = target;
    // LowPassFilter_t lpf_d;
    // LowPassFilter_t lpf_q;
    //
    // LowPassFilter_init(&lpf_d, 0.5);
    // LowPassFilter_init(&lpf_q, 0.5);
    BLDCMotor_getElectricalAngle(motor);
    BLDCMotor_getCurrentDQ(motor);
    // printf("current\n");

    // printf("%f, %f\n", motor->foc_motor.i_d, motor->foc_motor.i_q);
    // delay_ms(1);

    motor->foc_motor.i_d = LowPassFilter(&motor->foc_motor.LPF_current_d, motor->foc_motor.i_d);
    motor->foc_motor.i_q = LowPassFilter(&motor->foc_motor.LPF_current_q, motor->foc_motor.i_q);
    motor->foc_motor.voltage_q = PIDController_update(motor->foc_motor.PID_current_q, target - motor->foc_motor.i_q);
    motor->foc_motor.voltage_d = PIDController_update(motor->foc_motor.PID_current_d, 0 - motor->foc_motor.i_d);

    BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, motor->e_angle);
    // printf("%f, %f, %f, %f, %f\n", motor->foc_motor.i_q, motor->foc_motor.i_d, target, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d);
    // 1.
    // printf(">");
    // printf("iq:%f,id:%f,target:%f,", motor->foc_motor.i_q, motor->foc_motor.i_d, target);
    // printf("vq:%f,vd:%f", motor->foc_motor.voltage_q, motor->foc_motor.voltage_d);
    // printf("\r\n");
    return 0;
}

int BLDCMotor_currentClosedLoop_0(BLDCMotor_t *motor, float target)
{
    motor->velocity = BLDCMotor_getVelocity(motor);
    motor->angle = BLDCMotor_getAngle(motor);
    float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
    motor->e_angle = e_angle;
    // motor->foc_motor.voltage_q = PIDController_update(&motor->foc_motor.PID_i_q, target - motor->currentSense->i_q);
    motor->foc_motor.voltage_d = PIDController_update(&motor->PID, 0 - motor->currentSense->i_d);
    // BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d,(e_angle));
    BLDCMotor_SVPWM(motor, 2.0f, motor->foc_motor.voltage_d, e_angle);
    return 0;
}
int BLDCMotor_currentClosedLoop(BLDCMotor_t *motor, float target)
{
    motor->velocity = BLDCMotor_getVelocity(motor);
    // motor->angle = BLDCMotor_getAngle(motor);
    motor->angle = motor->foc_motor.Encoder.AS5600.angle;
    float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
    motor->e_angle = e_angle;
    motor->currentSense->i_d = LowPassFilter(&motor->lpf_id, motor->currentSense->i_d);
    motor->currentSense->i_q = LowPassFilter(&motor->lpf_iq, motor->currentSense->i_q);
    motor->foc_motor.voltage_q = PIDController_update(&motor->PID_iq, target - motor->currentSense->i_q);
    motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);
    //1. for id, iq pid
    BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);
    //2. for id pid
    // motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);
    // BLDCMotor_SVPWM(motor, 2, motor->foc_motor.voltage_d, e_angle);
    return 0;
}
int BLDCMotor_currentClosedLoop_with_bandwidth(BLDCMotor_t *motor, float target)
{

    if (MultiRate_Controller.current_enable)
    {
        static int velocityLoop_counter = 0;
        static int positionLoop_counter = 0;
        static int velocityIsFirst = 1;
        static float target_iq = 0;
        static float target_velocity = 0;
        const int velocityLoop_period = 5;
        const int positionLoop_period = 5;

        motor->velocity = BLDCMotor_getVelocity(motor);
        motor->angle = motor->foc_motor.Encoder.AS5600.angle;
        motor->degree = motor->angle / _PI * 180.0f;

        float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
        motor->e_angle = e_angle;
        motor->currentSense->i_d = LowPassFilter(&motor->lpf_id, motor->currentSense->i_d);
        motor->currentSense->i_q = LowPassFilter(&motor->lpf_iq, motor->currentSense->i_q);
        motor->velocity = LowPassFilter(&motor->lpf_velocity, motor->velocity);
        motor->degree = LowPassFilter(&motor->lpf_degree, motor->degree);
        if (velocityLoop_counter++ % velocityLoop_period == 0)
        {

            if (positionLoop_counter++ % positionLoop_period == 0)
            {
                target_velocity = PIDController_update(&motor->PID_degree, target - motor->degree);
                // UART_SendFloat(1, motor->degree);
                // UART_SendDataFloat(&motor->degree, 1);
            }
            target_iq = PIDController_update(&motor->PID_velocity, target_velocity - motor->velocity);
            //1.
            // target_iq = PIDController_update(&motor->PID_velocity, target - motor->velocity);
        }
        motor->foc_motor.voltage_q = PIDController_update(&motor->PID_iq, target_iq - motor->currentSense->i_q);
        motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);
        BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);

        //1.
        // motor->foc_motor.voltage_q = PIDController_update(&motor->PID_iq, target - motor->currentSense->i_q);
        // motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);
        // BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);

        //2.
        // motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);
        // BLDCMotor_SVPWM(motor, 2, motor->foc_motor.voltage_d, e_angle);

        // UART_SendFloat(6, motor->target,motor->degree,motor->velocity, motor->currentSense->i_q, motor->currentSense->i_d, motor->foc_motor.voltage_q);

    }
    return 0;
}
int BLDCMotor_currentClosedLoopBandwith(BLDCMotor_t *motor, float target)
{
    if (motor->currentSense->isReady)
    {
        motor->currentSense->isReady = false;
        motor->velocity = BLDCMotor_getVelocity(motor);
        // motor->velocity = LowPassFilter(&motor->lpf_velocity, motor->velocity);
        motor->velocity = lpf_process(&motor->filter_velocity, motor->velocity);
        motor->angle = motor->foc_motor.Encoder.angle;

        // BLDCMotor_getCurrentDQ(motor);
        float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
        motor->e_angle = e_angle;
        // motor->CurrentSense.i_d = motor->currentSense->i_d;
        // motor->CurrentSense.i_q = motor->currentSense->i_q;


        // motor->foc_motor.voltage_d = 0;

        // BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);

        //2.
#if DEBUG_PID_CURRENT_ID
        motor->foc_motor.voltage_q = 0;
        motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);
#else
        motor->foc_motor.voltage_q = PIDController_update(&motor->PID_iq, target - motor->currentSense->i_q);
        motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);
#endif
        BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);

        // BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);
        // const int counter = 10;
        // static int i = 0;
        // if ( i++ % counter == 0) {
        //     UART_SendFloat(2, motor->currentSense->i_a,  motor->currentSense->i_b);
        // }

    }
    // int t1 = getUs();
    // float delta_t = (float)(t1 - t0) / 1e3;
    // printf("delata = %f, %f\n", delta_t, motor->velocity);
    return 0;
}
int BLDCMotor_currentClosedLoopBandwith_v0(BLDCMotor_t *motor, float target)
{
    // if (MultiRate_Controller.current_enable)
    // int t0 = getUs();
    //1.
    // while (!motor->currentSense->isReady){};
    //2.
    if (motor->currentSense->isReady)
    {
        motor->currentSense->isReady = false;
        motor->velocity = BLDCMotor_getVelocity(motor);
        motor->velocity = LowPassFilter(&motor->lpf_velocity, motor->velocity);
        motor->angle = motor->foc_motor.Encoder.angle;

        // BLDCMotor_getCurrentDQ(motor);
        float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
        motor->e_angle = e_angle;
        // motor->CurrentSense.i_d = motor->currentSense->i_d;
        // motor->CurrentSense.i_q = motor->currentSense->i_q;

        motor->foc_motor.voltage_q = PIDController_update(&motor->PID_iq, target - motor->currentSense->i_q);
        // motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);

        motor->foc_motor.voltage_d = 0;

        // BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);

        //2.
        // motor->foc_motor.voltage_q = 1;
        // motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);
        BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);

        // BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);
        // const int counter = 10;
        // static int i = 0;
        // if ( i++ % counter == 0) {
        //     UART_SendFloat(2, motor->currentSense->i_a,  motor->currentSense->i_b);
        // }

    }
    // int t1 = getUs();
    // float delta_t = (float)(t1 - t0) / 1e3;
    // printf("delata = %f, %f\n", delta_t, motor->velocity);
    return 0;
}
int BLDCMotor_currentClosedLoopBandwithInline(BLDCMotor_t *motor, float target)
{
    {
        motor->velocity = BLDCMotor_getVelocity(motor);
        motor->angle = motor->foc_motor.Encoder.angle;


        float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
        motor->e_angle = e_angle;

        BLDCMotor_getCurrentDQ(motor);
        //
        // motor->CurrentSense.i_a = motor->currentSense->i_a;
        // motor->CurrentSense.i_b = motor->currentSense->i_b;
        // motor->CurrentSense.i_d = motor->currentSense->i_d;
        // motor->CurrentSense.i_q = motor->currentSense->i_q;

        //1.
        // currentSense_pollingGetCurrent(motor->currentSense);
        // motor->currentSense->i_a = lpf_process(&motor->lpf_ia0, motor->currentSense->i_a);
        // motor->currentSense->i_b = lpf_process(&motor->lpf_ib0, motor->currentSense->i_b);
        // Clark_Park_Transform(motor->currentSense->i_a, motor->currentSense->i_b,
        //                      &motor->currentSense->i_d, &motor->currentSense->i_q,
        //                      motor->e_angle);
        // motor->currentSense->i_d = lpf_process(&motor->lpf_id0, motor->currentSense->i_d);
        // motor->currentSense->i_q = lpf_process(&motor->lpf_iq0, motor->currentSense->i_q);

        motor->CurrentSense.i_a = motor->currentSense->i_a;
        motor->CurrentSense.i_b = motor->currentSense->i_b;
        motor->CurrentSense.i_d = motor->currentSense->i_d;
        motor->CurrentSense.i_q = motor->currentSense->i_q;

        //1.
        motor->foc_motor.voltage_q = PIDController_update(&motor->PID_iq, target - motor->currentSense->i_q);
        // motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);
        motor->foc_motor.voltage_d = 0;
        // BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);
        BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);

        //2.
        // motor->foc_motor.voltage_q = 2;
        // motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);
        // BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);

        // BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);
        motor->currentSense->isReady = false;
    }
    return 0;
}
int BLDCMotor_currentClosedLoopBandwith_debug(BLDCMotor_t *motor, float target)
{
    // if (MultiRate_Controller.current_enable)
    // int t0 = getUs();
    while (!motor->currentSense->isReady){};
    // if (motor->currentSense->isReady)
    {
        motor->currentSense->isReady = false;
        motor->velocity = BLDCMotor_getVelocity(motor);
        // motor->velocity = 0;
        // motor->angle = motor->foc_motor.Encoder.AS5600.angle;
        motor->angle = motor->foc_motor.Encoder.angle;

        float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
        motor->e_angle = e_angle;
        Clark_Park_Transform(motor->currentSense->i_a, motor->currentSense->i_b, &motor->currentSense->i_d, &motor->currentSense->i_q, e_angle);
        motor->currentSense->i_d = LowPassFilter(&motor->lpf_id, motor->currentSense->i_d);
        motor->currentSense->i_q = LowPassFilter(&motor->lpf_iq, motor->currentSense->i_q);
        motor->CurrentSense.i_d = motor->currentSense->i_d;
        motor->CurrentSense.i_q = motor->currentSense->i_q;

        motor->foc_motor.voltage_q = PIDController_update(&motor->PID_iq, target - motor->currentSense->i_q);
        motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);
        // BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);
        // BLDCMotor_SVPWM(motor, 0, _sin(), e_angle);
    }
    // int t1 = getUs();
    // float delta_t = (float)(t1 - t0) / 1e3;
    // printf("delata = %f, %f\n", delta_t, motor->velocity);
    return 0;
}
int BLDCMotor_currentVelocityClosedLoopBandwith(BLDCMotor_t *motor, float target)
{
    if (motor->currentSense->isReady)
    {
        motor->Iq_ref_ff = 0.0f;
        static int velocityLoop_counter = 0;
        const int velocityLoop_period = 2;
        static float target_iq = 0;
        float velocity = BLDCMotor_getVelocity(motor)*360;
        motor->angle = motor->foc_motor.Encoder.angle;

        float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
        motor->e_angle = e_angle;
        // BLDCMotor_getCurrentDQ(motor);
        motor->velocity = LowPassFilter(&motor->lpf_velocity, velocity);
        // motor->velocity = velocity;

        if (velocityLoop_counter++ % velocityLoop_period == 0)
        {
            // motor->velocity =lpf_process(&motor->filter_velocity, velocity);

            target_iq = PIDController_update(&motor->PID_velocity, target - motor->velocity);
            if (target_iq > 0) {
                target_iq   = target_iq + motor->Iq_ref_ff;
            }
            else {
                target_iq = target_iq - motor->Iq_ref_ff;
            }
            motor->target_iq = target_iq;
        }
        motor->foc_motor.voltage_q = PIDController_update(&motor->PID_iq, target_iq - motor->currentSense->i_q);
        motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);
        // motor->foc_motor.voltage_d   = 0;
        // BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);
        BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);
        motor->currentSense->isReady = false;
    }
    return 0;
}
int BLDCMotor_currentAngleClosedLoopBandwith(BLDCMotor_t *motor, float target)
{
    if (motor->currentSense->isReady)
    {
        motor->Iq_ref_ff = 0.0f;
        static int velocityLoop_counter = 0;
        const int velocityLoop_period = 2;
        static float target_iq = 0;
        // float velocity = BLDCMotor_getVelocity(motor)*360;
        float velocity = BLDCMotor_getVelocity(motor);
        motor->angle = motor->foc_motor.Encoder.angle;
        // float degree = motor->angle * 180 / _PI;
        float degree = motor->angle / _2PI;


        float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
        motor->e_angle = e_angle;
        // BLDCMotor_getCurrentDQ(motor);
        // motor->degree = LowPassFilter(&motor->lpf_degree,degree);
        motor->degree = degree;

        if (velocityLoop_counter++ % velocityLoop_period == 0)
        {
            // motor->velocity =lpf_process(&motor->filter_velocity, velocity);

            target_iq = PIDController_update(&motor->PID_degree, target - motor->degree);
            // target_iq = target_iq > 0 ? target_iq + motor->Iq_ref_ff: target_iq - motor->Iq_ref_ff;
            if (target_iq > 0) {
                target_iq = target_iq + motor->Iq_ref_ff;
            }
            else {
                target_iq = target_iq - motor->Iq_ref_ff;
            }
            // target_iq = target_iq + motor->Iq_ref_ff;
            motor->target_iq = target_iq;
        }
        motor->foc_motor.voltage_q = PIDController_update(&motor->PID_iq, target_iq - motor->currentSense->i_q);
        motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);
        // motor->foc_motor.voltage_d   = 0;
        // BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);
        BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);
        motor->currentSense->isReady = false;
    }
    return 0;
}
int BLDCMotor_currentVelocityRPMClosedLoopBandwith(BLDCMotor_t *motor, float target)
{
    while (!motor->currentSense->isReady){};

    {
        static int velocityLoop_counter = 0;
        const int velocityLoop_period = 4;
        static float target_iq = 0;
        float velocity = BLDCMotor_getVelocityRPM(motor);
        motor->angle = motor->foc_motor.Encoder.angle;

        float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
        motor->e_angle = e_angle;
        Clark_Park_Transform(motor->currentSense->i_a, motor->currentSense->i_b, &motor->currentSense->i_d, &motor->currentSense->i_q, e_angle);
        motor->currentSense->i_d = LowPassFilter(&motor->lpf_id, motor->currentSense->i_d);
        motor->currentSense->i_q = LowPassFilter(&motor->lpf_iq, motor->currentSense->i_q);
        motor->CurrentSense.i_d = motor->currentSense->i_d;
        motor->CurrentSense.i_q = motor->currentSense->i_q;
        motor->velocity = LowPassFilter(&motor->lpf_velocity, velocity);

        if (velocityLoop_counter++ % velocityLoop_period == 0)
        {
            target_iq = PIDController_update(&motor->PID_velocity, target - motor->velocity);
        }
        motor->foc_motor.voltage_q = PIDController_update(&motor->PID_iq, target_iq - motor->currentSense->i_q);
        motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);
        BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);
    }
    return 0;
}
int BLDCMotor_currentVelocityAngleClosedLoopBandwith(BLDCMotor_t *motor, float target)
{

    // while (!motor->currentSense->isReady){};
    if (motor->currentSense->isReady)
    {
       // int t0 = getUs();
        static int velocityLoop_counter = 0;
        const int velocityLoop_period = 2;
        const int angleLoop_period = 2;
        static int angleLoop_counter = 0;

        static float target_iq = 0;
        static float target_velocity = 0;
        // float velocity = BLDCMotor_getVelocity(motor)*_2PI;
        float velocity = BLDCMotor_getVelocity(motor)*360;
        motor->angle = motor->foc_motor.Encoder.angle;
        float degrees = motor->angle * 180 / _PI;
        // float degrees = motor->angle;
        // float degrees = motor->angle / _2PI;

        float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
        motor->e_angle = e_angle;
        // target_velocity = target;
        motor->velocity = LowPassFilter(&motor->lpf_velocity, velocity);

        if (velocityLoop_counter++ % velocityLoop_period == 0) {
            if (angleLoop_counter++ % angleLoop_period == 0)
            {
            // motor->degree = LowPassFilter(&motor->lpf_degree, degrees);
            motor->degree = degrees;
            target_velocity = PIDController_update(&motor->PID_degree, target - motor->degree);
            }
            motor->target_velocity = target_velocity;

            target_iq = PIDController_update(&motor->PID_velocity, target_velocity - motor->velocity);
            motor->target_iq = target_iq;
        }
        motor->foc_motor.voltage_q = PIDController_update(&motor->PID_iq, target_iq - motor->currentSense->i_q);
        motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);
        // motor->foc_motor.voltage_d = 0;
        // BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);
        // e_angle = LowPassFilter(&motor->lpf_eAngle, e_angle);
        BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);
        // int t1 = getUs();
        // int delta_t= t1 - t0;
        // float dt = (float)delta_t;
        // VOFA_SendJustFloat_DMA(&dt, 1);
        // VOFA_SendJustFloat(&dt, 1);


    }
    return 0;
}
// int BLDCMotor_currentAngleClosedLoopBandwith(BLDCMotor_t *motor, float target)
// {
//
//     // while (!motor->currentSense->isReady){};
//     if (motor->currentSense->isReady)
//     {
//         static int velocityLoop_counter = 0;
//         const int velocityLoop_period = 3;
//         static int angleLoop_counter = 0;
//
//         static float target_iq = 0;
//         static float target_velocity = 0;
//         // float velocity = BLDCMotor_getVelocity(motor)*_2PI;
//         float velocity = BLDCMotor_getVelocity(motor)*360;
//         motor->velocity = velocity;
//         motor->angle = motor->foc_motor.Encoder.angle;
//         float degrees = motor->angle * 180 / _PI;
//         // float degrees = motor->angle;
//         // float degrees = motor->angle / _2PI;
//
//         float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
//         motor->e_angle = e_angle;
//         // target_velocity = target;
//
//         //1.
//         //     const int angleLoop_period = 5;
//         //     if (angleLoop_counter++ % angleLoop_period == 0)
//         //     {
//         // motor->degree = LowPassFilter(&motor->lpf_degree, degrees);
//         // target_iq = PIDController_update(&motor->PID_degree, target - motor->degree);
//         // target_iq = _constrain(target_iq, -1.5, 1.5);
//         //     }
//         // motor->foc_motor.voltage_q = target_iq * CONFIG_MOTOR2804_RS;
//         // motor->foc_motor.voltage_d = -motor->velocity*_2PI*CONFIG_MOTOR2804_LS*target_iq;
//
//         const int angleLoop_period = 1;
//         if (angleLoop_counter++ % angleLoop_period == 0)
//         {
//             motor->degree = LowPassFilter(&motor->lpf_degree, degrees);
//             target_velocity = PIDController_update(&motor->PID_degree, target - motor->degree);
//         }
//         motor->velocity = LowPassFilter(&motor->lpf_velocity, velocity);
//         // motor->velocity  = BLDCMotor_outputVelocityRamp(motor, 360*100);
//         target_iq = PIDController_update(&motor->PID_velocity, target_velocity - motor->velocity);
//         motor->foc_motor.voltage_q = _constrain(target_iq * CONFIG_MOTOR2804_RS, -6.8, 6.8);
//         motor->foc_motor.voltage_d = 0;
//
//
//         BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);
//     }
//     return 0;
// }
float BLDCMotor_outputVelocityRamp(BLDCMotor_t *motor, float output_ramp) {

    static float output_prev = 0;
    float output = motor->velocity;
    static uint32_t timestamp_prev = 0;
    unsigned int delta_us;
    unsigned int timestamp_now = getUs();
    if (timestamp_now > timestamp_prev) {
        delta_us = (timestamp_now - timestamp_prev);
    }
    else {
        delta_us = (0xFFFFFFFF - timestamp_prev) + timestamp_now + 1;
    }
    float Ts = (float)delta_us / 1000000.0f;
    // float Ts = PID_UPDATE_T * 1e-3;
    // quick fix for strange cases (micros overflow)
    if (Ts <= 0 || Ts > 0.5)
        Ts = 1e-3f;

    float output_rate = (output - output_prev) / Ts;
    if (output_rate > output_ramp)
        output = output_prev + output_ramp * Ts;
    else if (output_rate < -output_ramp)
        output = output_prev - output_ramp * Ts;
    return output;
}
int BLDCMotor_positionCascade_with_bandwidth(BLDCMotor_t *motor, float target_degree)
{
    static float target_velocity = 0;
    static float target_iq = 0;
    motor->velocity = BLDCMotor_getVelocity(motor);
    motor->angle = motor->foc_motor.Encoder.AS5600.angle;
    motor->degree = motor->angle / _PI * 180.0f;

    float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
    motor->e_angle = e_angle;
    motor->currentSense->i_d = LowPassFilter(&motor->lpf_id, motor->currentSense->i_d);
    motor->currentSense->i_q = LowPassFilter(&motor->lpf_iq, motor->currentSense->i_q);
    motor->velocity = LowPassFilter(&motor->lpf_velocity, motor->velocity);
    motor->degree = LowPassFilter(&motor->lpf_degree, motor->degree);
    // ...
    if (MultiRate_Controller.pos_enable)
    {
        target_velocity = PIDController_update(&motor->PID_degree, target_degree - motor->degree);
    // UART_SendDataFloat(&motor->degree, 1);
    }

    // if (MultiRate_Controller.speed_enable || MultiRate_Controller.pos_enable)
    if (MultiRate_Controller.speed_enable)
    {
        target_iq = PIDController_update(&motor->PID_velocity, target_velocity - motor->velocity);
    }
    if (MultiRate_Controller.current_enable)
    {
        motor->foc_motor.voltage_q = PIDController_update(&motor->PID_iq, target_iq - motor->currentSense->i_q);
        motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0.0f - motor->currentSense->i_d);
        BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);
    }

    return 0;
}
int BLDCMotor_currentVelocityClosedLoop(BLDCMotor_t *motor, float target)
{
    motor->velocity = BLDCMotor_getVelocity(motor);
    motor->angle = BLDCMotor_getAngle(motor);
    float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
    motor->e_angle = e_angle;
    motor->currentSense->i_d = LowPassFilter(&motor->lpf_id, motor->currentSense->i_d);
    motor->currentSense->i_q = LowPassFilter(&motor->lpf_iq, motor->currentSense->i_q);
    motor->velocity = LowPassFilter(&motor->lpf_velocity, motor->velocity);
    motor->CurrentSense.i_d = motor->currentSense->i_d;
    motor->CurrentSense.i_q = motor->currentSense->i_q;

    float target_iq = PIDController_update(&motor->PID_velocity, target - motor->velocity);
    motor->foc_motor.voltage_q = PIDController_update(&motor->PID_iq, target_iq - motor->currentSense->i_q);
    motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);
    //1. for id, iq pid
    BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);
    //2. for id pid
    // motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);
    // BLDCMotor_SVPWM(motor, 2, motor->foc_motor.voltage_d, e_angle);
    return 0;
}
int BLDCMotor_angleClosedLoop(BLDCMotor_t *motor, float target)
{
    if (!motor)
        return -1;

    float position_target = (target != NOT_SET) ? target : motor->target;
    motor->target = position_target;

    motor->angle = BLDCMotor_getAngle(motor);
    motor->velocity = BLDCMotor_getVelocity(motor);
    float e_angle = _electricalAngle_calibrated(motor->direction,
                                               motor->angle,
                                               motor->foc_motor.pole_pairs,
                                               motor->foc_motor.zero_electric_angle);
    motor->e_angle = e_angle;

    motor->degree = motor->angle / _PI * 180.0f;
    // float angle_measured = motor->degree;
    float angle_measured = LowPassFilter(&motor->lpf_degree, motor->degree);
    motor->degree = angle_measured;
    float velocity_measured = LowPassFilter(&motor->lpf_velocity, motor->velocity);
    motor->velocity = velocity_measured;

    float velocity_target_deg = PIDController_update(&motor->PID_degree, position_target - angle_measured);
    //1.
    // const float DEG2RAD = _PI / 180.0f;
    // float velocity_target = velocity_target_deg * DEG2RAD;
    //2.
    float velocity_target = velocity_target_deg;

    float vq_cmd = PIDController_update(&motor->PID_velocity, velocity_target - velocity_measured);

    float v_limit = motor->Driver.voltage_limit;
    if (v_limit > 0.0f)
    {
        vq_cmd = _constrain(vq_cmd, -v_limit, v_limit);
    }

    motor->foc_motor.voltage_q = vq_cmd;
    motor->foc_motor.voltage_d = 0.0f;

    BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);
    return 0;
}
int BLDCMotor_currentVelocityAngleClosedLoop(BLDCMotor_t *motor, float target)
{
    motor->velocity = BLDCMotor_getVelocity(motor);
    motor->angle = BLDCMotor_getAngle(motor);
    float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
    motor->e_angle = e_angle;

    motor->degree = motor->angle / _PI * 180.0f;

    motor->currentSense->i_d = LowPassFilter(&motor->lpf_id, motor->currentSense->i_d);
    motor->currentSense->i_q = LowPassFilter(&motor->lpf_iq, motor->currentSense->i_q);
    motor->velocity = LowPassFilter(&motor->lpf_velocity, motor->velocity);
    // motor->degree = LowPassFilter(&motor->lpf_degree, motor->degree);

    float target_velocity = PIDController_update(&motor->PID_degree, target - motor->degree);
    float target_iq = PIDController_update(&motor->PID_velocity, target_velocity - motor->velocity);
    motor->foc_motor.voltage_q = PIDController_update(&motor->PID_iq, target_iq - motor->currentSense->i_q);
    motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);
    //1. for id, iq pid
    BLDCMotor_SVPWM(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, e_angle);
    //2. for id pid
    // motor->foc_motor.voltage_d = PIDController_update(&motor->PID_id, 0 - motor->currentSense->i_d);
    // BLDCMotor_SVPWM(motor, 2, motor->foc_motor.voltage_d, e_angle);
    return 0;
}
void BLDCMotor_ControlRunByKey(BLDCMotor_t *motor) {
    static int key_flag = 0;
    if (Key_Scan(0) == KEY_ON) {
        if (key_flag == 0) {
            key_flag = 1;
        }
        else if (key_flag == 1) {
            key_flag = 0;
        }
    }
    if (key_flag == 1) {
        motor->isEnable = 1;
    }
    else if (key_flag == 0) {
        motor->isEnable = 0;
    }
}
void BLDCMotor_KeyControl(BLDCMotor_t *motor) {
    if (g_keyFlag == 1) {
        motor->isEnable = 1;
        BLDCMotor_enable(motor);
    }
    else if (g_keyFlag == 0) {
        motor->isEnable = 0;
        BLDCMotor_disable(motor);
    }
}
void BLDCMotor_run(BLDCMotor_t *motor)
{
    // BLDCMotor_velocityClosedLoop(motor, motor->target);
    BLDCMotor_move(motor, motor->target);
}