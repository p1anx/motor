/*
 * BLDCDriver3PWM.c
 * Ported from:
 * https://github.com/simplefoc/Arduino-FOC/blob/master/src/drivers/BLDCDriver3PWM.cpp
 */

#include "BLDCDriver3PWM.h"
#include "main.h"

void BLDCDriver3PWM_init(BLDCDriver3PWM_t *driver, int phA, int phB, int phC,
                         int en) {
  // Pin initialization
  driver->pwmA = phA;
  driver->pwmB = phB;
  driver->pwmC = phC;

  // enable_pin pin
  driver->enable_pin = en;

  // default power-supply value
  driver->voltage_power_supply = DEF_POWER_SUPPLY;
  driver->voltage_limit = NOT_SET;
}

// enable motor driver
void BLDCDriver3PWM_enable(BLDCDriver3PWM_t *driver) {
  // enable_pin the driver - if enable_pin pin available
  HAL_GPIO_WritePin(Pin8_Enable_GPIO_Port, Pin8_Enable_Pin, GPIO_PIN_SET);
  // set zero to PWM
  BLDCDriver3PWM_setPwm(driver, 0, 0, 0);
}

// disable motor driver
void BLDCDriver3PWM_disable(BLDCDriver3PWM_t *driver) {
  // set zero to PWM
  BLDCDriver3PWM_setPwm(driver, 0, 0, 0);
  // disable the driver - if enable_pin pin available
  HAL_GPIO_WritePin(Pin8_Enable_GPIO_Port, Pin8_Enable_Pin, GPIO_PIN_RESET);
}

// init hardware pins
int BLDCDriver3PWM_initHardware(BLDCDriver3PWM_t *driver) {
  // a bit of separation
  _delay(1000);

  // sanity check for the voltage limit configuration
  if (driver->voltage_limit == NOT_SET ||
      driver->voltage_limit > driver->voltage_power_supply)
    driver->voltage_limit = driver->voltage_power_supply;

  // Set the pwm frequency to the pins
  // hardware specific function - depending on driver and mcu
  // _configure3PWM(pwm_frequency, pwmA, pwmB, pwmC);
  return 0;
}

// Set voltage to the pwm pin
void BLDCDriver3PWM_setPwm(BLDCDriver3PWM_t *driver, float Ua, float Ub,
                           float Uc) {

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
  _writeDutyCycle3PWM(dc_a, dc_b, dc_c, driver->pwmA, driver->pwmB,
                      driver->pwmC);
}
