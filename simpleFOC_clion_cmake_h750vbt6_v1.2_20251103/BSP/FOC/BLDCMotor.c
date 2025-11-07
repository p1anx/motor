
#include "BLDCMotor.h"
#include "encoder.h"
#include "foc_base.h"
#include "foc_motor.h"
#include "lowpass_filter.h"
#include "pid.h"
#include "as5600.h"
#include "stm32_hal.h"
#include <stdio.h>
#include "pwm.h"
#include "currentSense.h"

BLDCMotor_t motor;

int BLDCMotor_enable(BLDCMotor_t *motor)
{
    HAL_GPIO_WritePin(motor->enable_Port, motor->enable_Pin, SET);
    motor->isEnable = 1;
    return 0;
}
int BLDCMotor_disable(BLDCMotor_t *motor)
{
    HAL_GPIO_WritePin(motor->enable_Port, motor->enable_Pin, RESET);
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
void BLDCMotor_setPhaseVoltage(BLDCMotor_t *motor, float Uq, float Ud, float angle_el)
{
    if (!motor || !motor->driver)
    {
        PRINT_ERROR("setPhaseVoltage---motor or driver problem");
        return;
    }

    // Check if this is a 6PWM driver by checking function pointer
    // For 6PWM drivers, we can use centered modulation more effectively
    const int centered = 1;
    int sector;
    float _ca, _sa;

    switch (motor->foc_motor.foc_modulation)
    {
    case FOCModulationType_SinePWM:
        // Sinusoidal PWM modulation
        // Inverse Park + Clarke transformation

        // angle normalization in between 0 and 2pi
        // only necessary if using _sin and _cos - approximation functions
        angle_el = _normalizeAngle(angle_el + motor->foc_motor.zero_electric_angle);
        _ca = _cos(angle_el);
        _sa = _sin(angle_el);
        // Inverse park transform
        motor->Ualpha = _ca * Ud - _sa * Uq; // -sin(angle) * Uq;
        motor->Ubeta = _sa * Ud + _ca * Uq;  //  cos(angle) * Uq;

        // Clarke transform
        // Check if using 6PWM driver - can handle bipolar voltages better
        // For 6PWM, we can center around 0V instead of VCC/2
        motor->Ua = motor->Ualpha;
        motor->Ub = -0.5f * motor->Ualpha + _SQRT3_2 * motor->Ubeta;
        motor->Uc = -0.5f * motor->Ualpha - _SQRT3_2 * motor->Ubeta;

        // For compatibility with 3PWM drivers, add offset if needed
        if (centered)
        {
            motor->Ua += motor->driver->voltage_limit / 2;
            motor->Ub += motor->driver->voltage_limit / 2;
            motor->Uc += motor->driver->voltage_limit / 2;
        }

        if (!centered)
        {
            float Umin = fminf(motor->Ua, fminf(motor->Ub, motor->Uc));
            motor->Ua -= Umin;
            motor->Ub -= Umin;
            motor->Uc -= Umin;
        }

        // printf("it's in setPhaseVoltage\n");
        break;

    case FOCModulationType_SpaceVectorPWM:
        // Nice video explaining the SpaceVectorModulation (SVPWM) algorithm
        // https://www.youtube.com/watch?v=QMSWUMEAejg

        // if negative voltages change inverse the phase
        // angle + 180degrees
        if (Uq < 0)
            angle_el += _PI;
        Uq = fabsf(Uq);

        // angle normalisation in between 0 and 2pi
        // only necessary if using _sin and _cos - approximation functions
        angle_el = _normalizeAngle(angle_el + motor->foc_motor.zero_electric_angle + _PI_2);

        // find the sector we are in currently
        sector = (int)floorf(angle_el / _PI_3) + 1;
        // calculate the duty cycles
        float T1 = _SQRT3 * _sin(sector * _PI_3 - angle_el) * Uq / motor->driver->voltage_limit;
        float T2 = _SQRT3 * _sin(angle_el - (sector - 1.0f) * _PI_3) * Uq / motor->driver->voltage_limit;
        // two versions possible
        float T0 = 0; // pulled to 0 - better for low power supply voltage
        if (centered)
        {
            T0 = 1 - T1 - T2; // centered around driver->voltage_limit/2
        }

        // calculate the duty cycles(times)
        float Ta, Tb, Tc;
        switch (sector)
        {
        case 1:
            Ta = T1 + T2 + T0 / 2;
            Tb = T2 + T0 / 2;
            Tc = T0 / 2;
            break;
        case 2:
            Ta = T1 + T0 / 2;
            Tb = T1 + T2 + T0 / 2;
            Tc = T0 / 2;
            break;
        case 3:
            Ta = T0 / 2;
            Tb = T1 + T2 + T0 / 2;
            Tc = T2 + T0 / 2;
            break;
        case 4:
            Ta = T0 / 2;
            Tb = T1 + T0 / 2;
            Tc = T1 + T2 + T0 / 2;
            break;
        case 5:
            Ta = T2 + T0 / 2;
            Tb = T0 / 2;
            Tc = T1 + T2 + T0 / 2;
            break;
        case 6:
            Ta = T1 + T2 + T0 / 2;
            Tb = T0 / 2;
            Tc = T1 + T0 / 2;
            break;
        default:
            // possible error state
            Ta = 0;
            Tb = 0;
            Tc = 0;
        }

        // calculate the phase voltages and center
        motor->Ua = Ta * motor->driver->voltage_limit;
        motor->Ub = Tb * motor->driver->voltage_limit;
        motor->Uc = Tc * motor->driver->voltage_limit;
        break;
    }

    BLDCDriver6PWM_setPwm(motor->driver, motor->Ua, motor->Ub, motor->Uc);
}

void BLDCMotor_setPhaseVoltage_v1(BLDCMotor_t *motor, float Uq, float Ud, float angle_el)
{
    if (!motor || !motor->driver)
    {
        PRINT_ERROR("setPhaseVoltage---motor or driver problem");
        return;
    }

    // Check if this is a 6PWM driver by checking function pointer
    // For 6PWM drivers, we can use centered modulation more effectively
    const int centered = 1;
    int sector;
    float _ca, _sa;

    switch (motor->foc_motor.foc_modulation)
    {
    case FOCModulationType_Trapezoid_120:
        // see https://www.youtube.com/watch?v=InzXA7mWBWE Slide 5
        {
            static int trap_120_map[6][3] = {
                {0, 1, -1}, {-1, 1, 0}, {-1, 0, 1}, {0, -1, 1},
                {1, -1, 0}, {1, 0, -1} // each is 60 degrees with values for 3 phases
                                       // of 1=positive -1=negative 0=high-z
            };
            // static int trap_120_state = 0;
            sector = (int)(6 * (_normalizeAngle(angle_el + _PI / 6.0f + motor->foc_motor.zero_electric_angle) / _2PI)); // adding PI/6 to align with other modes

            motor->Ua = Uq + trap_120_map[sector][0] * Uq;
            motor->Ub = Uq + trap_120_map[sector][1] * Uq;
            motor->Uc = Uq + trap_120_map[sector][2] * Uq;

            if (centered)
            {
                motor->Ua += (motor->driver->voltage_limit) / 2 - Uq;
                motor->Ub += (motor->driver->voltage_limit) / 2 - Uq;
                motor->Uc += (motor->driver->voltage_limit) / 2 - Uq;
            }
        }
        break;

    case FOCModulationType_Trapezoid_150:
        // see https://www.youtube.com/watch?v=InzXA7mWBWE Slide 8
        {
            static int trap_150_map[12][3] = {
                {0, 1, -1}, {-1, 1, -1}, {-1, 1, 0},  {-1, 1, 1}, {-1, 0, 1}, {-1, -1, 1}, {0, -1, 1},
                {1, -1, 1}, {1, -1, 0},  {1, -1, -1}, {1, 0, -1}, {1, 1, -1} // each is 30 degrees with values for 3 phases
                                                                             // of 1=positive -1=negative 0=high-z
            };
            // static int trap_150_state = 0;
            sector = (int)(12 * (_normalizeAngle(angle_el + _PI / 6.0f + motor->foc_motor.zero_electric_angle) / _2PI)); // adding PI/6 to align with other modes

            motor->Ua = Uq + trap_150_map[sector][0] * Uq;
            motor->Ub = Uq + trap_150_map[sector][1] * Uq;
            motor->Uc = Uq + trap_150_map[sector][2] * Uq;

            // center
            if (centered)
            {
                motor->Ua += (motor->driver->voltage_limit) / 2 - Uq;
                motor->Ub += (motor->driver->voltage_limit) / 2 - Uq;
                motor->Uc += (motor->driver->voltage_limit) / 2 - Uq;
            }
        }
        break;

    case FOCModulationType_SinePWM:
        // Sinusoidal PWM modulation
        // Inverse Park + Clarke transformation

        // angle normalization in between 0 and 2pi
        // only necessary if using _sin and _cos - approximation functions
        angle_el = _normalizeAngle(angle_el + motor->foc_motor.zero_electric_angle);
        _ca = _cos(angle_el);
        _sa = _sin(angle_el);
        // Inverse park transform
        motor->Ualpha = _ca * Ud - _sa * Uq; // -sin(angle) * Uq;
        motor->Ubeta = _sa * Ud + _ca * Uq;  //  cos(angle) * Uq;

        // Clarke transform
        // Check if using 6PWM driver - can handle bipolar voltages better
        // For 6PWM, we can center around 0V instead of VCC/2
        motor->Ua = motor->Ualpha;
        motor->Ub = -0.5f * motor->Ualpha + _SQRT3_2 * motor->Ubeta;
        motor->Uc = -0.5f * motor->Ualpha - _SQRT3_2 * motor->Ubeta;

        // For compatibility with 3PWM drivers, add offset if needed
        if (centered)
        {
            motor->Ua += motor->driver->voltage_limit / 2;
            motor->Ub += motor->driver->voltage_limit / 2;
            motor->Uc += motor->driver->voltage_limit / 2;
        }

        if (!centered)
        {
            float Umin = fminf(motor->Ua, fminf(motor->Ub, motor->Uc));
            motor->Ua -= Umin;
            motor->Ub -= Umin;
            motor->Uc -= Umin;
        }

        // printf("it's in setPhaseVoltage\n");
        break;

    case FOCModulationType_SpaceVectorPWM:
        // Nice video explaining the SpaceVectorModulation (SVPWM) algorithm
        // https://www.youtube.com/watch?v=QMSWUMEAejg

        // if negative voltages change inverse the phase
        // angle + 180degrees
        if (Uq < 0)
            angle_el += _PI;
        Uq = fabsf(Uq);

        // angle normalisation in between 0 and 2pi
        // only necessary if using _sin and _cos - approximation functions
        angle_el = _normalizeAngle(angle_el + motor->foc_motor.zero_electric_angle + _PI_2);

        // find the sector we are in currently
        sector = (int)floorf(angle_el / _PI_3) + 1;
        // calculate the duty cycles
        float T1 = _SQRT3 * _sin(sector * _PI_3 - angle_el) * Uq / motor->driver->voltage_limit;
        float T2 = _SQRT3 * _sin(angle_el - (sector - 1.0f) * _PI_3) * Uq / motor->driver->voltage_limit;
        // two versions possible
        float T0 = 0; // pulled to 0 - better for low power supply voltage
        if (centered)
        {
            T0 = 1 - T1 - T2; // centered around driver->voltage_limit/2
        }

        // calculate the duty cycles(times)
        float Ta, Tb, Tc;
        switch (sector)
        {
        case 1:
            Ta = T1 + T2 + T0 / 2;
            Tb = T2 + T0 / 2;
            Tc = T0 / 2;
            break;
        case 2:
            Ta = T1 + T0 / 2;
            Tb = T1 + T2 + T0 / 2;
            Tc = T0 / 2;
            break;
        case 3:
            Ta = T0 / 2;
            Tb = T1 + T2 + T0 / 2;
            Tc = T2 + T0 / 2;
            break;
        case 4:
            Ta = T0 / 2;
            Tb = T1 + T0 / 2;
            Tc = T1 + T2 + T0 / 2;
            break;
        case 5:
            Ta = T2 + T0 / 2;
            Tb = T0 / 2;
            Tc = T1 + T2 + T0 / 2;
            break;
        case 6:
            Ta = T1 + T2 + T0 / 2;
            Tb = T0 / 2;
            Tc = T1 + T0 / 2;
            break;
        default:
            // possible error state
            Ta = 0;
            Tb = 0;
            Tc = 0;
        }

        // calculate the phase voltages and center
        motor->Ua = Ta * motor->driver->voltage_limit;
        motor->Ub = Tb * motor->driver->voltage_limit;
        motor->Uc = Tc * motor->driver->voltage_limit;
        break;
    }

    // set the voltages in driver
    // Use the driver's function pointer table for flexibility (3PWM, 6PWM, etc.)
    // if (motor->driver && motor->driver->functions && motor->driver->functions->setPwm)
    // {
    //     motor->driver->functions->setPwm(motor->driver, motor->Ua, motor->Ub, motor->Uc);
    // }
    // printf("set v = %f,%f,%f\n", motor->Ua, motor->Ub, motor->Uc);
    BLDCDriver6PWM_setPwm(motor->driver, motor->Ua, motor->Ub, motor->Uc);
}
void BLDCMotor_setPhaseVoltage_v0(BLDCMotor_t *motor, float Uq, float Ud, float angle_el)
{
    if (!motor || !motor->driver)
        return;

    // Check if this is a 6PWM driver by checking function pointer
    // For 6PWM drivers, we can use centered modulation more effectively
    const int centered = 1;
    int sector;
    float _ca, _sa;

    switch (motor->FOCMotor->foc_modulation)
    {
    case FOCModulationType_Trapezoid_120:
        // see https://www.youtube.com/watch?v=InzXA7mWBWE Slide 5
        {
            static int trap_120_map[6][3] = {
                {0, 1, -1}, {-1, 1, 0}, {-1, 0, 1}, {0, -1, 1},
                {1, -1, 0}, {1, 0, -1} // each is 60 degrees with values for 3 phases
                                       // of 1=positive -1=negative 0=high-z
            };
            // static int trap_120_state = 0;
            sector = (int)(6 * (_normalizeAngle(angle_el + _PI / 6.0f + motor->FOCMotor->zero_electric_angle) / _2PI)); // adding PI/6 to align with other modes

            motor->Ua = Uq + trap_120_map[sector][0] * Uq;
            motor->Ub = Uq + trap_120_map[sector][1] * Uq;
            motor->Uc = Uq + trap_120_map[sector][2] * Uq;

            if (centered)
            {
                motor->Ua += (motor->driver->voltage_limit) / 2 - Uq;
                motor->Ub += (motor->driver->voltage_limit) / 2 - Uq;
                motor->Uc += (motor->driver->voltage_limit) / 2 - Uq;
            }
        }
        break;

    case FOCModulationType_Trapezoid_150:
        // see https://www.youtube.com/watch?v=InzXA7mWBWE Slide 8
        {
            static int trap_150_map[12][3] = {
                {0, 1, -1}, {-1, 1, -1}, {-1, 1, 0},  {-1, 1, 1}, {-1, 0, 1}, {-1, -1, 1}, {0, -1, 1},
                {1, -1, 1}, {1, -1, 0},  {1, -1, -1}, {1, 0, -1}, {1, 1, -1} // each is 30 degrees with values for 3 phases
                                                                             // of 1=positive -1=negative 0=high-z
            };
            // static int trap_150_state = 0;
            sector = (int)(12 * (_normalizeAngle(angle_el + _PI / 6.0f + motor->FOCMotor->zero_electric_angle) / _2PI)); // adding PI/6 to align with other modes

            motor->Ua = Uq + trap_150_map[sector][0] * Uq;
            motor->Ub = Uq + trap_150_map[sector][1] * Uq;
            motor->Uc = Uq + trap_150_map[sector][2] * Uq;

            // center
            if (centered)
            {
                motor->Ua += (motor->driver->voltage_limit) / 2 - Uq;
                motor->Ub += (motor->driver->voltage_limit) / 2 - Uq;
                motor->Uc += (motor->driver->voltage_limit) / 2 - Uq;
            }
        }
        break;

    case FOCModulationType_SinePWM:
        // Sinusoidal PWM modulation
        // Inverse Park + Clarke transformation

        // angle normalization in between 0 and 2pi
        // only necessary if using _sin and _cos - approximation functions
        angle_el = _normalizeAngle(angle_el + motor->FOCMotor->zero_electric_angle);
        _ca = _cos(angle_el);
        _sa = _sin(angle_el);
        // Inverse park transform
        motor->Ualpha = _ca * Ud - _sa * Uq; // -sin(angle) * Uq;
        motor->Ubeta = _sa * Ud + _ca * Uq;  //  cos(angle) * Uq;

        // Clarke transform
        // Check if using 6PWM driver - can handle bipolar voltages better
        // For 6PWM, we can center around 0V instead of VCC/2
        motor->Ua = motor->Ualpha;
        motor->Ub = -0.5f * motor->Ualpha + _SQRT3_2 * motor->Ubeta;
        motor->Uc = -0.5f * motor->Ualpha - _SQRT3_2 * motor->Ubeta;

        // For compatibility with 3PWM drivers, add offset if needed
        if (centered)
        {
            motor->Ua += motor->driver->voltage_limit / 2;
            motor->Ub += motor->driver->voltage_limit / 2;
            motor->Uc += motor->driver->voltage_limit / 2;
        }

        if (!centered)
        {
            float Umin = fminf(motor->Ua, fminf(motor->Ub, motor->Uc));
            motor->Ua -= Umin;
            motor->Ub -= Umin;
            motor->Uc -= Umin;
        }

        // printf("it's in setPhaseVoltage\n");
        break;

    case FOCModulationType_SpaceVectorPWM:
        // Nice video explaining the SpaceVectorModulation (SVPWM) algorithm
        // https://www.youtube.com/watch?v=QMSWUMEAejg

        // if negative voltages change inverse the phase
        // angle + 180degrees
        if (Uq < 0)
            angle_el += _PI;
        Uq = fabsf(Uq);

        // angle normalisation in between 0 and 2pi
        // only necessary if using _sin and _cos - approximation functions
        angle_el = _normalizeAngle(angle_el + motor->FOCMotor->zero_electric_angle + _PI_2);

        // find the sector we are in currently
        sector = (int)floorf(angle_el / _PI_3) + 1;
        // calculate the duty cycles
        float T1 = _SQRT3 * _sin(sector * _PI_3 - angle_el) * Uq / motor->driver->voltage_limit;
        float T2 = _SQRT3 * _sin(angle_el - (sector - 1.0f) * _PI_3) * Uq / motor->driver->voltage_limit;
        // two versions possible
        float T0 = 0; // pulled to 0 - better for low power supply voltage
        if (centered)
        {
            T0 = 1 - T1 - T2; // centered around driver->voltage_limit/2
        }

        // calculate the duty cycles(times)
        float Ta, Tb, Tc;
        switch (sector)
        {
        case 1:
            Ta = T1 + T2 + T0 / 2;
            Tb = T2 + T0 / 2;
            Tc = T0 / 2;
            break;
        case 2:
            Ta = T1 + T0 / 2;
            Tb = T1 + T2 + T0 / 2;
            Tc = T0 / 2;
            break;
        case 3:
            Ta = T0 / 2;
            Tb = T1 + T2 + T0 / 2;
            Tc = T2 + T0 / 2;
            break;
        case 4:
            Ta = T0 / 2;
            Tb = T1 + T0 / 2;
            Tc = T1 + T2 + T0 / 2;
            break;
        case 5:
            Ta = T2 + T0 / 2;
            Tb = T0 / 2;
            Tc = T1 + T2 + T0 / 2;
            break;
        case 6:
            Ta = T1 + T2 + T0 / 2;
            Tb = T0 / 2;
            Tc = T1 + T0 / 2;
            break;
        default:
            // possible error state
            Ta = 0;
            Tb = 0;
            Tc = 0;
        }

        // calculate the phase voltages and center
        motor->Ua = Ta * motor->driver->voltage_limit;
        motor->Ub = Tb * motor->driver->voltage_limit;
        motor->Uc = Tc * motor->driver->voltage_limit;
        break;
    }

    // set the voltages in driver
    // Use the driver's function pointer table for flexibility (3PWM, 6PWM, etc.)
    // if (motor->driver && motor->driver->functions && motor->driver->functions->setPwm)
    // {
    //     motor->driver->functions->setPwm(motor->driver, motor->Ua, motor->Ub, motor->Uc);
    // }
    // printf("ua = %f, ub = %f, uc = %f\n", motor->Ua, motor->Ub, motor->Uc);
    BLDCDriver6PWM_setPwm(motor->driver, motor->Ua, motor->Ub, motor->Uc);
}

void BLDCMotor_init_v0(BLDCMotor_t *motor, int pp)
{
    if (!motor)
        return;

    // Initialize base FOCMotor
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
}
void BLDCMotor_init(BLDCMotor_t *motor, int pp)
{
    if (!motor)
        return;

    // Initialize base FOCMotor
    // 1. pointer FOCMotor
    // FOCMotor_init(motor->FOCMotor);
    // motor->FOCMotor->pole_pairs = pp;
    // 2.
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

    motor->foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;
    motor->enable_Port = GPIOC;
    motor->enable_Pin = GPIO_PIN_1;

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
    // return FOCMotor_shaftVelocity(motor->FOCMotor);
    return FOCMotor_shaftVelocity(&motor->foc_motor);
    // return AS5600_getVelocity(motor->as5600);
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
float BLDCMotor_getVelocityRPM(BLDCMotor_t *motor)
{
    // return FOCMotor_shaftVelocity(motor->FOCMotor);
    return FOCMotor_shaftVelocityRPM(&motor->foc_motor);
    // return AS5600_getVelocity(motor->as5600);
}
float BLDCMotor_getAngle(BLDCMotor_t *motor)
{
    // return _electricalAngle(FOCMotor_shaftAngle(&motor->foc_motor), motor->foc_motor.pole_pairs);
    // return FOCMotor_shaftAngle(motor->FOCMotor);
    return FOCMotor_shaftAngle(&motor->foc_motor);
    // return AS5600_getAngle(motor->as5600);
}
float BLDCMotor_getElectricalAngle(BLDCMotor_t *motor)
{
    motor->angle = 1 * BLDCMotor_getAngle(motor);
    motor->e_angle = _normalizeAngle(_electricalAngle(motor->angle, motor->foc_motor.pole_pairs));
    return motor->e_angle;
}

float BLDCMotor_getCurrentDQ(BLDCMotor_t *motor)
{
    float i_a, i_b, i_alpha, i_beta, i_q, i_d;

    // 1.
    // CurrentSense_get3Current(motor->currentSense);
    // i_a = motor->currentSense->i3[0];
    // i_b = motor->currentSense->i3[1];
    // 2.
    CurrentSense_read3Current(motor->currentSense);
    i_a = motor->currentSense->i_a;
    i_b = motor->currentSense->i_b;
    // i_a = 0;
    // i_b = 0;
    Clarke_Transform(i_a, i_b, &i_alpha, &i_beta);
    Park_Transform(i_alpha, i_beta, _normalizeAngle(motor->e_angle), &i_d, &i_q);
    motor->foc_motor.i_d = i_d;
    motor->foc_motor.i_q = i_q;
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

int BLDCMotor_alignSensor(BLDCMotor_t *motor)
{
    if (!motor)
        return 0;

    printf("[Align] Starting sensor alignment...\n");

    // ========== 步骤1：将电机转到固定电角度位置 ==========
    // 使用270度（3π/2）作为校准位置
    float target_electrical_angle = _3PI_2;  // 270度

    printf("[Align] Moving motor to alignment position (270 deg electrical)...\n");

    // 施加电压，让电机转到270度电角度位置
    for (int i = 0; i < 10; i++)
    {
        BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_sensor_align, 0, target_electrical_angle);
        _delay(100);  // 等待电机稳定
    }

    _delay(500);  // 额外延迟确保完全稳定

    // ========== 步骤2：读取此时的机械角度 ==========
    float shaft_angle = FOCMotor_shaftAngle(&motor->foc_motor);
    printf("[Align] Shaft angle at alignment position: %.4f rad (%.2f deg)\n",
           shaft_angle, shaft_angle * 180.0f / 3.14159265f);

    // ========== 步骤3：计算电角度零点 ==========
    // zero_electric_angle = 目标电角度 - (机械角度 × 极对数)
    float electrical_angle = _electricalAngle(shaft_angle, motor->foc_motor.pole_pairs);
    motor->foc_motor.zero_electric_angle = _normalizeAngle(target_electrical_angle - electrical_angle);

    printf("[Align]  zero_electric_angle = %.4f rad (%.2f deg)\n",
           motor->foc_motor.zero_electric_angle,
           motor->foc_motor.zero_electric_angle * 180.0f / 3.14159265f);

    // ========== 步骤4：测试方向（可选） ==========
    float start_angle = shaft_angle;

    printf("[Align] Testing motor direction...\n");

    // 正向转动60度
    for (int i = 0; i <= 5; i++)
    {
        float angle = _3PI_2 + _2PI * i / 6.0f;
        BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_sensor_align, 0, angle);
        _delay(200);
    }
    float end_angle = FOCMotor_shaftAngle(&motor->foc_motor);

    if (end_angle > start_angle)
    {
        printf("[Align] Motor direction: CW (Clockwise)\n");
        if (motor->foc_motor.sensor)
        {
            motor->foc_motor.sensor->natural_direction = Direction_CW;
        }
    }
    else if (end_angle < start_angle)
    {
        printf("[Align] Motor direction: CCW (Counter-Clockwise)\n");
        if (motor->foc_motor.sensor)
        {
            motor->foc_motor.sensor->natural_direction = Direction_CCW;
        }
    }
    else
    {
        printf("[Align] Warning: Sensor failed to notice movement!\n");
    }

    // ========== 步骤5：关闭电压，让电机自由停止 ==========
    BLDCMotor_setPhaseVoltage(motor, 0, 0, 0);

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
    // get angular velocity
    // motor->foc_motor.shaft_velocity = FOCMotor_shaftVelocity(&motor->foc_motor);
    // motor->foc_motor.shaft_velocity = LowPassFilter(&motor->foc_motor.LPF_velocity, motor->foc_motor.shaft_velocity);
    // choose control loop
    // printf("hello move\n");
    // switch (motor->FOCMotor->controller)
    switch (motor->foc_motor.controller)
    {
    case ControlType_voltage:
        break;
    case ControlType_current:
        // motor->foc_motor.i_q = motor->foc_motor.target;
        // motor->foc_motor.voltage_q = PIDController_update(motor->foc_motor.PID_current_q, motor->foc_motor.target - motor->foc_motor.i_q);
        // motor->foc_motor.voltage_d = PIDController_update(motor->foc_motor.PID_current_d, 0 - motor->foc_motor.i_d);
        // BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d * 0, _3PI_2);
        break;
    case ControlType_angle:
        // angle set point
        // include angle loop
        // motor->foc_motor.shaft_angle_sp = motor->foc_motor.target;
        // motor->foc_motor.shaft_velocity_sp = PIDController_update(&motor->foc_motor.P_angle, motor->foc_motor.shaft_angle_sp - motor->foc_motor.shaft_angle);
        // motor->foc_motor.voltage_q = PIDController_update(&motor->foc_motor.PID_velocity, motor->foc_motor.shaft_velocity_sp - motor->foc_motor.shaft_velocity);
        break;
    case ControlType_velocity:
        // velocity set point
        // include velocity loop
        // motor->foc_motor.shaft_velocity_sp = motor->foc_motor.target;
        // motor->foc_motor.voltage_q = PIDController_update(&motor->foc_motor.PID_velocity, motor->foc_motor.shaft_velocity_sp - motor->foc_motor.shaft_velocity);
        // 1.
        // motor->foc_motor.shaft_angle = FOCMotor_shaftAngle(&motor->foc_motor);
        // motor->foc_motor.voltage_q = PIDController_update(motor->pid, motor->foc_motor.target - motor->foc_motor.shaft_velocity);
        // BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, 0, _electricalAngle(motor->foc_motor.shaft_angle, motor->foc_motor.pole_pairs));

        // 2.
        // motor->angle = BLDCMotor_getAngle(motor);
        // motor->velocity = BLDCMotor_getVelocity(motor);
        // motor->foc_motor.voltage_q = PIDController_update(motor->pid, motor->target - motor->velocity);
        // BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, 0, _electricalAngle(motor->angle, motor->foc_motor.pole_pairs));
        // 3.
        BLDCMotor_velocityClosedLoop_without_i(motor, new_target);
        // motor->foc_motor.voltage_d = 0;
        break;
    case ControlType_velocity_openloop:
        // printf("open\n");
        BLDCMotor_velocityOpenloop(motor, new_target);
        // BLDCMotor_velocityOpenloop_with_timer(motor, new_target);

        break;
    case ControlType_angle_openloop:
        // angle control in open loop
        // loopFOC should not be called
        motor->foc_motor.shaft_angle_sp = motor->foc_motor.target;
        // BLDCMotor_angleOpenloop(motor, motor->foc_motor.shaft_angle_sp);
        break;
    case ControlType_velocityClosedLoop:
        BLDCMotor_velocityClosedLoop(motor, new_target);
        break;
    case ControlType_currentClosedLoop:
        BLDCMotor_currentClosedLoop(motor, new_target);
        break;
    default:
        printf("other controller type\n");
        // BLDCMotor_velocityOpenloop(motor, new_target);
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
    unsigned long now_us = _micros();
    // calculate the sample time from last call
    float Ts = (now_us - motor->open_loop_timestamp) * 1e-3f;

    // calculate the necessary angle to achieve target velocity

    float angle = _normalizeAngle(motor->angle + target_velocity * Ts);
    motor->angle = angle;

    // set the maximal allowed voltage (voltage_limit) with the necessary angle
    // 1.
    // float e_angle = _electricalAngle(motor->angle, motor->FOCMotor->pole_pairs);
    // BLDCMotor_setPhaseVoltage(motor, motor->FOCMotor->voltage_limit, 0, e_angle);
    // 2.
    // float e_angle = _electricalAngle(motor->angle, motor->foc_motor.pole_pairs);
    // 3.
    float e_angle = _normalizeAngle(_electricalAngle(angle, motor->foc_motor.pole_pairs));
    motor->e_angle = e_angle;
    BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_limit, 0, e_angle);
    motor->open_loop_timestamp = now_us;

    // DEBUG_PRINT("velocity open loop");

    // printf("velocity openloop\n");
    // 1.
    // printf("Ts = %f\n", Ts);
    // printf("motor->angle = %f\n", motor->angle);
    // printf("v limit = %f\n", motor->FOCMotor->voltage_limit);
    // printf("e angle = %f\n", e_angle);
    // printf("pp = %d\n", motor->FOCMotor->pole_pairs);

    // save timestamp for next call
}
int isFirst = 1;
int BLDCMotor_velocityClosedLoop(BLDCMotor_t *motor, float target)
{

    motor->velocity = BLDCMotor_getVelocityRPM(motor)/60;
    // motor->velocity = BLDCMotor_getVelocityRPM(motor)/60;
    // motor->angle = BLDCMotor_getAngle(motor);
    motor->angle = motor->foc_motor.encoder->as5600_Instance.angle;
    float e_angle = _electricalAngle(motor->angle, motor->foc_motor.pole_pairs);
    motor->e_angle = e_angle;
    motor->foc_motor.voltage_q = PIDController_update(motor->pid, target - motor->velocity);
    // BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, 0, _normalizeAngle(e_angle));
    BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, 0,(e_angle));
    if (isFirst)
    {
        printf("velocity = %f, angle = %f, vq = %f\n", motor->velocity, motor->angle, motor->foc_motor.voltage_q);
        isFirst = 0;

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
int BLDCMotor_currentClosedLoop(BLDCMotor_t *motor, float target)
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
}
void BLDCMotor_run(BLDCMotor_t *motor)
{
    // BLDCMotor_velocityClosedLoop(motor, motor->target);
    BLDCMotor_move(motor, motor->target);
}
