#ifndef __BLDC_DRIVER_6PWM_H
#define __BLDC_DRIVER_6PWM_H

#include "BLDCDriver.h"
#include "motor_config.h"
#include "mymain.h"

/**
 * 6PWM BLDC Driver structure
 * Extends the base BLDCDriver_t with 6PWM specific configuration
 */
typedef struct {
  BLDCDriver_t base;       //!< Base driver structure (must be first member)
  
  // Hardware pin configurations for 6PWM
  GPIO_TypeDef* pwmAh_port; //!< Phase A high side port
  uint16_t pwmAh_pin;       //!< Phase A high side pin
  GPIO_TypeDef* pwmAl_port; //!< Phase A low side port  
  uint16_t pwmAl_pin;       //!< Phase A low side pin
  
  GPIO_TypeDef* pwmBh_port; //!< Phase B high side port
  uint16_t pwmBh_pin;       //!< Phase B high side pin
  GPIO_TypeDef* pwmBl_port; //!< Phase B low side port
  uint16_t pwmBl_pin;       //!< Phase B low side pin
  
  GPIO_TypeDef* pwmCh_port; //!< Phase C high side port
  uint16_t pwmCh_pin;       //!< Phase C high side pin
  GPIO_TypeDef* pwmCl_port; //!< Phase C low side port
  uint16_t pwmCl_pin;       //!< Phase C low side pin
  
  float dead_time;          //!< Dead time in microseconds to prevent shoot-through
  
} BLDCDriver6PWM_t;

/**
 * Initialize 6PWM BLDC driver
 * @param driver - Driver instance
 * @param voltage_power_supply - Power supply voltage
 * @param voltage_limit - Voltage limit
 * @param dead_time - Dead time in microseconds
 * @return 0 on success, -1 on error
 */
int BLDCDriver6PWM_init(BLDCDriver6PWM_t *driver, 
                        float voltage_power_supply,
                        float voltage_limit, 
                        float dead_time);

/**
 * Enable 6PWM BLDC driver
 * @param driver - Base driver pointer
 */
void BLDCDriver6PWM_enable(BLDCDriver_t *driver);

/**
 * Disable 6PWM BLDC driver  
 * @param driver - Base driver pointer
 */
void BLDCDriver6PWM_disable(BLDCDriver_t *driver);

/**
 * Set PWM values for 6PWM driver
 * @param driver - Base driver pointer
 * @param Ua - Phase A voltage (-voltage_limit to +voltage_limit)
 * @param Ub - Phase B voltage (-voltage_limit to +voltage_limit)
 * @param Uc - Phase C voltage (-voltage_limit to +voltage_limit)
 */
void BLDCDriver6PWM_setPwm(BLDCDriver_t *driver, float Ua, float Ub, float Uc);

/**
 * Configure hardware pins for 6PWM driver
 * @param driver - 6PWM driver instance
 * @param pwmAh_port - Phase A high side GPIO port
 * @param pwmAh_pin - Phase A high side GPIO pin
 * @param pwmAl_port - Phase A low side GPIO port
 * @param pwmAl_pin - Phase A low side GPIO pin
 * @param pwmBh_port - Phase B high side GPIO port
 * @param pwmBh_pin - Phase B high side GPIO pin
 * @param pwmBl_port - Phase B low side GPIO port
 * @param pwmBl_pin - Phase B low side GPIO pin
 * @param pwmCh_port - Phase C high side GPIO port
 * @param pwmCh_pin - Phase C high side GPIO pin
 * @param pwmCl_port - Phase C low side GPIO port
 * @param pwmCl_pin - Phase C low side GPIO pin
 */
void BLDCDriver6PWM_setPins(BLDCDriver6PWM_t *driver,
                           GPIO_TypeDef* pwmAh_port, uint16_t pwmAh_pin,
                           GPIO_TypeDef* pwmAl_port, uint16_t pwmAl_pin,
                           GPIO_TypeDef* pwmBh_port, uint16_t pwmBh_pin,
                           GPIO_TypeDef* pwmBl_port, uint16_t pwmBl_pin,
                           GPIO_TypeDef* pwmCh_port, uint16_t pwmCh_pin,
                           GPIO_TypeDef* pwmCl_port, uint16_t pwmCl_pin);

#endif // __BLDC_DRIVER_6PWM_H