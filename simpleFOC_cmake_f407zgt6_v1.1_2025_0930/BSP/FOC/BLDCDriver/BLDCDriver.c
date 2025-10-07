#include "BLDCDriver.h"
#include "motor_config.h"
// #include "stm32f4xx_hal_gpio.h"
#include "pwm.h"
#include "stm32_hal.h"

void BLDCDriver_init(BLDCDriver_t *driver, int pwm_Hz, int pwm_resolution, float voltage_power_supply, float voltage_limit)
{
    driver->pwm_frequency = pwm_Hz;
    driver->pwm_resolution = pwm_resolution;
    pwm_init(driver->pwm_frequency, driver->pwm_resolution);
    BLDCDriver3PWM_init(driver, voltage_power_supply, voltage_limit);
    PRINT_OK("PWM driver init");
}
// enable motor driver
void BLDCDriver3PWM_enable(BLDCDriver_t *driver)
{
    HAL_GPIO_WritePin(BLDCMOTOR_ENABLE_PORT, BLDCMOTOR_ENABLE_PIN, GPIO_PIN_SET);
    // set zero to PWM
    setPwm(0, 0, 0);
    printf("Motor is enabled!\n");
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

// void BLDCDriver3PWM_init(BLDCDriver_t *driver) {
//   // Pin initialization
//   // driver->pwmA = phA;
//   // driver->pwmB = phB;
//   // driver->pwmC = phC;
//   //
//   // // enable_pin pin
//   // driver->enable_pin = en;
//
//   // default power-supply value
//   driver->voltage_power_supply = DEF_POWER_SUPPLY;
//   driver->voltage_limit = NOT_SET;
// }
