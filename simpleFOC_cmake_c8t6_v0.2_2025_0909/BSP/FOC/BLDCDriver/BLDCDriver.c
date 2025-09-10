#include "BLDCDriver.h"
#include "motor_config.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal_gpio.h"

// // enable motor driver
// void BLDCDriver3PWM_enable(BLDCDriver_t *driver) {
//   // enable_pin the driver - if enable_pin pin available
//   // HAL_GPIO_WritePin(Pin8_Enable_GPIO_Port, Pin8_Enable_Pin, GPIO_PIN_SET);
//   HAL_GPIO_WritePin(Pin8_Enable_GPIO_Port, Pin8_Enable_Pin, GPIO_PIN_SET);
//   // set zero to PWM
//   BLDCDriver3PWM_setPwm(driver, 0, 0, 0);
// }
//
// // disable motor driver
// void BLDCDriver3PWM_disable(BLDCDriver_t *driver) {
//   // set zero to PWM
//   BLDCDriver3PWM_setPwm(driver, 0, 0, 0);
//   // disable the driver - if enable_pin pin available
//   HAL_GPIO_WritePin(Pin8_Enable_GPIO_Port, Pin8_Enable_Pin, GPIO_PIN_RESET);
// }
// enable motor driver
void BLDCDriver3PWM_enable(void) {
  // enable_pin the driver - if enable_pin pin available
  // HAL_GPIO_WritePin(Pin8_Enable_GPIO_Port, Pin8_Enable_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(BLDCMOTOR_ENABLE_PORT, BLDCMOTOR_ENABLE_PIN, GPIO_PIN_SET);
  // set zero to PWM
  setPwm(0, 0, 0);
  printf("Motor is enabled!\n");
}

// disable motor driver
void BLDCDriver3PWM_disable(void) {
  // set zero to PWM
  setPwm(0, 0, 0);
  // disable the driver - if enable_pin pin available
  HAL_GPIO_WritePin(BLDCMOTOR_ENABLE_PORT, BLDCMOTOR_ENABLE_PIN,
                    GPIO_PIN_RESET);
}
