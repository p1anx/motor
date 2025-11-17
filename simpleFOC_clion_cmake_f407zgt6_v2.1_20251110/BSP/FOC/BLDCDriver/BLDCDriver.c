#include "BLDCDriver.h"
#include "motor_config.h"
// #include "stm32f4xx_hal_gpio.h"
#include "pwm.h"
#include "stm32_hal.h"
#include <stdio.h>
#include <math.h>

BLDCDriver_t driver;

void BLDCDriver_init(BLDCDriver_t *driver, int pwm_Hz, int pwm_resolution, float voltage_power_supply, float voltage_limit)
{
    driver->pwm_frequency = pwm_Hz;
    driver->pwm_resolution = pwm_resolution;
    PWM6_Init(driver->pwm_frequency, driver->pwm_resolution);
    // BLDCDriver3PWM_init(driver, voltage_power_supply, voltage_limit);
    BLDCDriver6PWM_init(driver, voltage_power_supply, voltage_limit);
    // BLDCDriverPWM_enable(driver);

}

void BLDCDriver_setPhaseVoltage(BLDCDriver_t *driver, float Uq, float Ud, float electrical_angle)
{
    float angle_el = electrical_angle;
    if (!driver)
    {
        printf("[ERROR] BLDCDriver setPhaseVoltage---motor or driver problem");
        return;
    }

    // Check if this is a 6PWM driver by checking function pointer
    // For 6PWM drivers, we can use centered modulation more effectively
    const int centered = 1;
    int sector;
    float _ca, _sa;

    switch (driver->FOCModulationType)
    {
    case FOCModulationType_SinePWM:
        // Sinusoidal PWM modulation
        // Inverse Park + Clarke transformation

        // angle normalization in between 0 and 2pi
        // only necessary if using _sin and _cos - approximation functions
        //1.
        // angle_el = _normalizeAngle(angle_el + motor->foc_motor.zero_electric_angle);
        //2.
        driver->e_angle = angle_el;
        _ca = _cos(angle_el);
        _sa = _sin(angle_el);
        // Inverse park transform
        driver->Ualpha = _ca * Ud - _sa * Uq; // -sin(angle) * Uq;
        driver->Ubeta = _sa * Ud + _ca * Uq;  //  cos(angle) * Uq;

        // Clarke transform
        // Check if using 6PWM driver - can handle bipolar voltages better
        // For 6PWM, we can center around 0V instead of VCC/2
        driver->Ua = driver->Ualpha;
        driver->Ub = -0.5f * driver->Ualpha + _SQRT3_2 * driver->Ubeta;
        driver->Uc = -0.5f * driver->Ualpha - _SQRT3_2 * driver->Ubeta;

        // For compatibility with 3PWM drivers, add offset if needed
        if (centered)
        {
            driver->Ua += driver->voltage_limit / 2;
            driver->Ub += driver->voltage_limit / 2;
            driver->Uc += driver->voltage_limit / 2;
        }

        if (!centered)
        {
            float Umin = fminf(driver->Ua, fminf(driver->Ub, driver->Uc));
            driver->Ua -= Umin;
            driver->Ub -= Umin;
            driver->Uc -= Umin;
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
        angle_el = _normalizeAngle(angle_el + driver->zero_electric_angle + _PI_2);

        // find the sector we are in currently
        sector = (int)floorf(angle_el / _PI_3) + 1;
        // calculate the duty cycles
        float T1 = _SQRT3 * _sin(sector * _PI_3 - angle_el) * Uq / driver->voltage_limit;
        float T2 = _SQRT3 * _sin(angle_el - (sector - 1.0f) * _PI_3) * Uq / driver->voltage_limit;
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
        driver->Ua = Ta * driver->voltage_limit;
        driver->Ub = Tb * driver->voltage_limit;
        driver->Uc = Tc * driver->voltage_limit;
        break;
    }

    BLDCDriver_setPwm(driver, driver->Ua, driver->Ub, driver->Uc);
}

void BLDCDriver_setPwm(BLDCDriver_t *driver, float Ua, float Ub, float Uc)
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

    BLDCDriver_write6PWM(dc_a, dc_b, dc_c);
}

void BLDCDriver_write6PWM(float dc_a, float dc_b, float dc_c)
{

    int pwm_range = pwm_tim.Instance->ARR;
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_1, dc_a * pwm_range);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_2, dc_b * pwm_range);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_3, dc_c * pwm_range);
}
void BLDCDriverPWM_enable(BLDCDriver_t *driver)
{
    HAL_GPIO_WritePin(BLDCDRIVER_ENABLE_PORT, BLDCDRIVER_ENABLE_PIN, GPIO_PIN_SET);
}
void BLDCDriverPWM_diable(BLDCDriver_t *driver)
{
    HAL_GPIO_WritePin(BLDCDRIVER_ENABLE_PORT, BLDCDRIVER_ENABLE_PIN, GPIO_PIN_RESET);
}

// enable motor driver
void BLDCDriver3PWM_enable(BLDCDriver_t *driver)
{
    HAL_GPIO_WritePin(BLDCDRIVER_ENABLE_PORT, BLDCDRIVER_ENABLE_PIN, GPIO_PIN_SET);
}

// disable motor driver
void BLDCDriver3PWM_disable(BLDCDriver_t *driver)
{
    setPwm(0, 0, 0);
    HAL_GPIO_WritePin(BLDCMOTOR_ENABLE_PORT, BLDCMOTOR_ENABLE_PIN, GPIO_PIN_RESET);
}
// enable motor driver

//================================================================================================
// void BLDCDriver3PWM_enable(void) {
//   // enable_pin the driver - if enable_pin pin available
//   // HAL_GPIO_WritePin(Pin8_Enable_GPIO_Port, Pin8_Enable_Pin, GPIO_PIN_SET);
//   HAL_GPIO_WritePin(BLDCMOTOR_ENABLE_PORT, BLDCMOTOR_ENABLE_PIN,
//   GPIO_PIN_SET);
//   // set zero to PWM
//   setPwm(0, 0, 0);
//   printf("Motor is enabled!\n");
// }
//
// // disable motor driver
// void BLDCDriver3PWM_disable(void) {
//   // set zero to PWM
//   setPwm(0, 0, 0);
//   // disable the driver - if enable_pin pin available
//   HAL_GPIO_WritePin(BLDCMOTOR_ENABLE_PORT, BLDCMOTOR_ENABLE_PIN,
//                     GPIO_PIN_RESET);
// }
//================================================================================================
static BLDCDriverFunctions_t FOCdriver3PWM_functions = {.init = NULL, // init function pointer not used in this driver
                                                        .enable = BLDCDriver3PWM_enable,
                                                        .disable = BLDCDriver3PWM_disable,
                                                        .setPwm = BLDCDriver3PWM_setPwm};

int BLDCDriver3PWM_init(BLDCDriver_t *driver, float voltage_power_supply, float voltage_limit)
{
    driver->functions = &FOCdriver3PWM_functions;
    driver->voltage_power_supply = voltage_power_supply;
    driver->voltage_limit = voltage_limit;
    return 0;
}
int BLDCDriver6PWM_init(BLDCDriver_t *driver, float voltage_power_supply, float voltage_limit)
{
    // driver->functions = &FOCdriver3PWM_functions;
    driver->voltage_power_supply = voltage_power_supply;
    driver->voltage_limit = voltage_limit;
    return 0;
}
