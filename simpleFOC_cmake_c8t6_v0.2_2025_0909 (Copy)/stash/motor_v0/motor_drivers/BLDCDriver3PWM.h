/*
 * BLDCDriver3PWM.h
 * Ported from: https://github.com/simplefoc/Arduino-FOC/blob/master/src/drivers/BLDCDriver3PWM.h
 * Converted from C++ to C
 */

#ifndef BLDCDRIVER3PWM_H_
#define BLDCDRIVER3PWM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "../common/base_classes/BLDCDriver.h"
#include "../common/foc_utils.h"
#include "../common/time_utils.h"
#include "../common/defaults.h"

/**
 * 3 PWM BLDC driver structure
 */
typedef struct {
    BLDCDriver_t driver; //!< Base driver structure (must be first member)
    
    // Hardware variables
    int pwmA; //!< phase A pwm pin number
    int pwmB; //!< phase B pwm pin number
    int pwmC; //!< phase C pwm pin number
    int enable_pin; //!< enable pin number
} BLDCDriver3PWM_t;

/**
 * Initialize BLDCDriver3PWM
 * @param driver - Pointer to BLDCDriver3PWM structure
 * @param phA - A phase pwm pin
 * @param phB - B phase pwm pin
 * @param phC - C phase pwm pin
 * @param en - enable pin (use NOT_SET if not available)
 */
void BLDCDriver3PWM_init(BLDCDriver3PWM_t* driver, int phA, int phB, int phC, int en);

/**
 * Motor hardware init function
 * @param driver - Pointer to BLDCDriver3PWM structure
 * @return 0 on success, error code otherwise
 */
int BLDCDriver3PWM_init_hardware(BLDCDriver3PWM_t* driver);

/**
 * Motor disable function
 * @param driver - Pointer to BLDCDriver3PWM structure
 */
void BLDCDriver3PWM_disable(BLDCDriver3PWM_t* driver);

/**
 * Motor enable function
 * @param driver - Pointer to BLDCDriver3PWM structure
 */
void BLDCDriver3PWM_enable(BLDCDriver3PWM_t* driver);

/**
 * Set phase voltages to the hardware
 * @param driver - Pointer to BLDCDriver3PWM structure
 * @param Ua - phase A voltage
 * @param Ub - phase B voltage
 * @param Uc - phase C voltage
 */
void BLDCDriver3PWM_setPwm(BLDCDriver3PWM_t* driver, float Ua, float Ub, float Uc);

#ifdef __cplusplus
}
#endif

#endif /* BLDCDRIVER3PWM_H_ */
