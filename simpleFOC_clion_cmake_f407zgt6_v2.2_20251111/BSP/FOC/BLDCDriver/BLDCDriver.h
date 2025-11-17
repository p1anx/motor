#ifndef __BLDCDRIVER_H
#define __BLDCDRIVER_H

#include "motor_config.h"
#include "mymain.h"
#include "foc_base.h"

#define  BLDCDRIVER_ENABLE_PORT GPIOA
#define  BLDCDRIVER_ENABLE_PIN  GPIO_PIN_9

typedef struct BLDCDriver_t BLDCDriver_t;

/**
 * BLDC Driver function pointers structure
 */
typedef struct
{
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
struct BLDCDriver_t
{
    BLDCDriverFunctions_t *functions; //!< Function pointer table
    int pwm_frequency;                //!< pwm frequency value in hertz
    int pwm_resolution;
    float voltage_power_supply; //!< power supply voltage
    float voltage_limit;        //!< limiting voltage set to the motor
    float Ua, Ub, Uc;
    float Ualpha, Ubeta;
    float zero_electric_angle;
    float e_angle;


    FOCModulationType_t FOCModulationType;

};

// 3PWM driver functions
void BLDCDriver_init(BLDCDriver_t *driver, int pwm_Hz, int pwm_resolution, float voltage_power_supply, float voltage_limit);
int BLDCDriver3PWM_init(BLDCDriver_t *driver, float voltage_power_supply, float voltage_limit);
void BLDCDriver3PWM_setPwm(BLDCDriver_t *driver, float Ua, float Ub, float Uc);
void BLDCDriver3PWM_enable(BLDCDriver_t *driver);
void BLDCDriver3PWM_disable(BLDCDriver_t *driver);
int BLDCDriver6PWM_init(BLDCDriver_t *driver, float voltage_power_supply, float voltage_limit);
void BLDCDriverPWM_enable(BLDCDriver_t *driver);
void BLDCDriverPWM_diable(BLDCDriver_t *driver);
void BLDCDriver_setPwm(BLDCDriver_t *driver, float Ua, float Ub, float Uc);
void BLDCDriver_write6PWM(float dc_a, float dc_b, float dc_c);
void BLDCDriver_setPhaseVoltage(BLDCDriver_t *driver, float Uq, float Ud, float electrical_angle);
void BLDCDriver_SVPWM(BLDCDriver_t *driver, float Uq, float Ud, float electrical_angle);

#endif // !__BLDCDRIVER_H
