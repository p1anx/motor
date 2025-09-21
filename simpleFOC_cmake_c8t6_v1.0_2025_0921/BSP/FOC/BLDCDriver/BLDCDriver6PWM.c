#include "BLDCDriver6PWM.h"
#include "motor_config.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal_gpio.h"
#include <math.h>

// Internal function declarations
static void setPwmPair(GPIO_TypeDef* high_port, uint16_t high_pin,
                      GPIO_TypeDef* low_port, uint16_t low_pin, 
                      float duty, float dead_time);
static uint32_t calculateDeadTimeCycles(float dead_time_us);

// Function table for 6PWM driver
static BLDCDriverFunctions_t BLDCDriver6PWM_functions = {
    .init = NULL, // Will be set during init
    .enable = BLDCDriver6PWM_enable,
    .disable = BLDCDriver6PWM_disable,
    .setPwm = BLDCDriver6PWM_setPwm
};

/**
 * Initialize 6PWM BLDC driver
 */
int BLDCDriver6PWM_init(BLDCDriver6PWM_t *driver, 
                        float voltage_power_supply,
                        float voltage_limit,
                        float dead_time) {
    if (driver == NULL) {
        return -1;
    }
    
    // Initialize base driver
    driver->base.functions = &BLDCDriver6PWM_functions;
    driver->base.voltage_power_supply = voltage_power_supply;
    driver->base.voltage_limit = voltage_limit;
    driver->base.pwm_frequency = 20000; // 20kHz default
    
    // Initialize 6PWM specific settings
    driver->dead_time = dead_time;
    
    // Initialize all pins to low
    setPwmPair(driver->pwmAh_port, driver->pwmAh_pin,
               driver->pwmAl_port, driver->pwmAl_pin, 0.0f, dead_time);
    setPwmPair(driver->pwmBh_port, driver->pwmBh_pin,
               driver->pwmBl_port, driver->pwmBl_pin, 0.0f, dead_time);
    setPwmPair(driver->pwmCh_port, driver->pwmCh_pin,
               driver->pwmCl_port, driver->pwmCl_pin, 0.0f, dead_time);
    
    printf("6PWM BLDC Driver initialized with dead time: %.2f us\n", dead_time);
    return 0;
}

/**
 * Configure hardware pins for 6PWM driver
 */
void BLDCDriver6PWM_setPins(BLDCDriver6PWM_t *driver,
                           GPIO_TypeDef* pwmAh_port, uint16_t pwmAh_pin,
                           GPIO_TypeDef* pwmAl_port, uint16_t pwmAl_pin,
                           GPIO_TypeDef* pwmBh_port, uint16_t pwmBh_pin,
                           GPIO_TypeDef* pwmBl_port, uint16_t pwmBl_pin,
                           GPIO_TypeDef* pwmCh_port, uint16_t pwmCh_pin,
                           GPIO_TypeDef* pwmCl_port, uint16_t pwmCl_pin) {
    if (driver == NULL) return;
    
    // Phase A pins
    driver->pwmAh_port = pwmAh_port;
    driver->pwmAh_pin = pwmAh_pin;
    driver->pwmAl_port = pwmAl_port;
    driver->pwmAl_pin = pwmAl_pin;
    
    // Phase B pins
    driver->pwmBh_port = pwmBh_port;
    driver->pwmBh_pin = pwmBh_pin;
    driver->pwmBl_port = pwmBl_port;
    driver->pwmBl_pin = pwmBl_pin;
    
    // Phase C pins
    driver->pwmCh_port = pwmCh_port;
    driver->pwmCh_pin = pwmCh_pin;
    driver->pwmCl_port = pwmCl_port;
    driver->pwmCl_pin = pwmCl_pin;
}

/**
 * Enable 6PWM BLDC driver
 */
void BLDCDriver6PWM_enable(BLDCDriver_t *driver) {
    if (driver == NULL) return;
    
    // Enable the motor driver using the enable pin
    HAL_GPIO_WritePin(BLDCMOTOR_ENABLE_PORT, BLDCMOTOR_ENABLE_PIN, GPIO_PIN_SET);
    
    // Set all PWM outputs to zero
    BLDCDriver6PWM_setPwm(driver, 0.0f, 0.0f, 0.0f);
    
    printf("6PWM Motor driver enabled!\n");
}

/**
 * Disable 6PWM BLDC driver
 */
void BLDCDriver6PWM_disable(BLDCDriver_t *driver) {
    if (driver == NULL) return;
    
    BLDCDriver6PWM_t *driver6pwm = (BLDCDriver6PWM_t*)driver;
    
    // Set all PWM outputs to zero
    BLDCDriver6PWM_setPwm(driver, 0.0f, 0.0f, 0.0f);
    
    // Force all pins low for safety
    HAL_GPIO_WritePin(driver6pwm->pwmAh_port, driver6pwm->pwmAh_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(driver6pwm->pwmAl_port, driver6pwm->pwmAl_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(driver6pwm->pwmBh_port, driver6pwm->pwmBh_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(driver6pwm->pwmBl_port, driver6pwm->pwmBl_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(driver6pwm->pwmCh_port, driver6pwm->pwmCh_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(driver6pwm->pwmCl_port, driver6pwm->pwmCl_pin, GPIO_PIN_RESET);
    
    // Disable the motor driver using the enable pin
    HAL_GPIO_WritePin(BLDCMOTOR_ENABLE_PORT, BLDCMOTOR_ENABLE_PIN, GPIO_PIN_RESET);
    
    printf("6PWM Motor driver disabled!\n");
}

/**
 * Set PWM values for 6PWM driver
 * In 6PWM mode, we control both high-side and low-side switches
 * with complementary signals and dead time
 */
void BLDCDriver6PWM_setPwm(BLDCDriver_t *driver, float Ua, float Ub, float Uc) {
    if (driver == NULL) return;
    
    BLDCDriver6PWM_t *driver6pwm = (BLDCDriver6PWM_t*)driver;
    
    // Limit voltages to the maximum allowed
    float voltage_limit = driver->voltage_limit;
    if (Ua >  voltage_limit) Ua =  voltage_limit;
    if (Ua < -voltage_limit) Ua = -voltage_limit;
    if (Ub >  voltage_limit) Ub =  voltage_limit;
    if (Ub < -voltage_limit) Ub = -voltage_limit;
    if (Uc >  voltage_limit) Uc =  voltage_limit;
    if (Uc < -voltage_limit) Uc = -voltage_limit;
    
    // Convert voltages to duty cycles (0.0 to 1.0)
    float duty_a = (Ua + voltage_limit) / (2.0f * voltage_limit);
    float duty_b = (Ub + voltage_limit) / (2.0f * voltage_limit);
    float duty_c = (Uc + voltage_limit) / (2.0f * voltage_limit);
    
    // Ensure duty cycles are within valid range
    duty_a = fmaxf(0.0f, fminf(1.0f, duty_a));
    duty_b = fmaxf(0.0f, fminf(1.0f, duty_b));
    duty_c = fmaxf(0.0f, fminf(1.0f, duty_c));
    
    // Set PWM for each phase pair
    setPwmPair(driver6pwm->pwmAh_port, driver6pwm->pwmAh_pin,
               driver6pwm->pwmAl_port, driver6pwm->pwmAl_pin, 
               duty_a, driver6pwm->dead_time);
               
    setPwmPair(driver6pwm->pwmBh_port, driver6pwm->pwmBh_pin,
               driver6pwm->pwmBl_port, driver6pwm->pwmBl_pin, 
               duty_b, driver6pwm->dead_time);
               
    setPwmPair(driver6pwm->pwmCh_port, driver6pwm->pwmCh_pin,
               driver6pwm->pwmCl_port, driver6pwm->pwmCl_pin, 
               duty_c, driver6pwm->dead_time);
}

/**
 * Set PWM for a complementary pair with dead time
 * This function implements the complementary PWM logic with dead time
 */
static void setPwmPair(GPIO_TypeDef* high_port, uint16_t high_pin,
                      GPIO_TypeDef* low_port, uint16_t low_pin, 
                      float duty, float dead_time) {
    
    // For safety, ensure duty cycle is within bounds
    if (duty < 0.0f) duty = 0.0f;
    if (duty > 1.0f) duty = 1.0f;
    
    // Simple complementary PWM implementation
    // In a real implementation, this would use hardware PWM with dead time
    // For now, we'll use a simplified approach with GPIO control
    
    if (duty > 0.5f) {
        // High side dominant
        HAL_GPIO_WritePin(high_port, high_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(low_port, low_pin, GPIO_PIN_RESET);
    } else if (duty < 0.5f) {
        // Low side dominant  
        HAL_GPIO_WritePin(high_port, high_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(low_port, low_pin, GPIO_PIN_SET);
    } else {
        // Center - both off for dead time
        HAL_GPIO_WritePin(high_port, high_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(low_port, low_pin, GPIO_PIN_RESET);
    }
    
    // Note: In a production system, you would use hardware PWM timers
    // with complementary outputs and hardware dead time insertion
    // This simplified version is for demonstration purposes
}

/**
 * Calculate dead time in timer cycles (placeholder implementation)
 */
static uint32_t calculateDeadTimeCycles(float dead_time_us) {
    // This would calculate the actual timer cycles needed for the dead time
    // Based on the system clock and timer prescaler
    // For now, return a placeholder value
    return (uint32_t)(dead_time_us * 72); // Assuming 72MHz system clock
}