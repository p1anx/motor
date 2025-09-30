#ifndef __BLDCDRIVER_H
#define __BLDCDRIVER_H

#include "motor_config.h"
#include "mymain.h"

typedef struct BLDCDriver_t BLDCDriver_t;

/**
 * BLDC Driver function pointers structure
 */
typedef struct {
  /** Initialise hardware */
  int (*init)(BLDCDriver_t *driver);
  /** Enable hardware */
  void (*enable)(BLDCDriver_t *driver);
  /** Disable hardware */
  void (*disable)(BLDCDriver_t *driver);
  /**
   * Set phase voltages to the hardware
   * @param driver - Driver instance
   * @param Ua - phase A voltage
   * @param Ub - phase B voltage
   * @param Uc - phase C voltage
   */
  void (*setPwm)(BLDCDriver_t *driver, float Ua, float Ub, float Uc);
} BLDCDriverFunctions_t;

/**
 * BLDC Driver base structure
 * Each driver implementation should include this as the first member
 */
struct BLDCDriver_t {
  BLDCDriverFunctions_t *functions; //!< Function pointer table
  long pwm_frequency;               //!< pwm frequency value in hertz
  float voltage_power_supply;       //!< power supply voltage
  float voltage_limit;              //!< limiting voltage set to the motor
};

// 3PWM driver functions
int BLDCDriver3PWM_init(BLDCDriver_t *driver, float voltage_power_supply,
                        float voltage_limit);
void BLDCDriver3PWM_setPwm(BLDCDriver_t *driver, float Ua, float Ub, float Uc);
void BLDCDriver3PWM_enable(BLDCDriver_t *driver);
void BLDCDriver3PWM_disable(BLDCDriver_t *driver);

#endif // !__BLDCDRIVER_H
