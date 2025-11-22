/*
 * Example usage of 6PWM BLDC Driver
 * This file demonstrates how to use the 6PWM driver with FOC control
 */

#include "BLDCDriver6PWM.h"
#include "BLDCMotor.h"

// Example function to initialize and use 6PWM driver
void example_6pwm_usage(void) {
    // Create 6PWM driver instance
    static BLDCDriver6PWM_t driver6pwm;
    static BLDCMotor_t motor;
    
    // Configure GPIO pins for 6PWM
    // Note: These are example pins - adjust according to your hardware
    BLDCDriver6PWM_setPins(&driver6pwm,
        GPIOA, GPIO_PIN_8,   // Phase A high side (TIM1_CH1)
        GPIOA, GPIO_PIN_7,   // Phase A low side  (TIM1_CH1N)
        GPIOA, GPIO_PIN_9,   // Phase B high side (TIM1_CH2)
        GPIOB, GPIO_PIN_0,   // Phase B low side  (TIM1_CH2N)
        GPIOA, GPIO_PIN_10,  // Phase C high side (TIM1_CH3)
        GPIOB, GPIO_PIN_1,   // Phase C low side  (TIM1_CH3N)
    );
    
    // Initialize 6PWM driver
    // Parameters: voltage_supply, voltage_limit, dead_time_us
    BLDCDriver6PWM_init(&driver6pwm, 12.0f, 6.0f, 1.0f);
    
    // Initialize BLDC motor (14 pole pairs as example)
    BLDCMotor_init(&motor, 7);
    
    // Link the 6PWM driver to the motor
    BLDCMotor_linkDriver(&motor, (BLDCDriver_t*)&driver6pwm);
    
    // Enable the driver
    driver6pwm.base.functions->enable((BLDCDriver_t*)&driver6pwm);
    
    // Set FOC modulation type - 6PWM works well with all types
    motor.foc_motor.foc_modulation = FOCModulationType_SinePWM;
    
    // Example: Set phase voltages directly
    // This will use the 6PWM complementary output with dead time
    BLDCMotor_setPhaseVoltage(&motor, 3.0f, 0.0f, 0.0f); // 3V Uq, 0V Ud, 0 rad angle
    
    printf("6PWM Driver example initialized successfully!\n");
    printf("Supply voltage: %.1fV, Voltage limit: %.1fV\n", 
           driver6pwm.base.voltage_power_supply, 
           driver6pwm.base.voltage_limit);
    printf("Dead time: %.1f us\n", driver6pwm.dead_time);
}

// Example for velocity control with 6PWM
void example_6pwm_velocity_control(void) {
    static BLDCDriver6PWM_t driver6pwm;
    static BLDCMotor_t motor;
    
    // Initialize hardware (same as above)
    BLDCDriver6PWM_setPins(&driver6pwm,
        GPIOA, GPIO_PIN_8, GPIOA, GPIO_PIN_7,    // Phase A
        GPIOA, GPIO_PIN_9, GPIOB, GPIO_PIN_0,    // Phase B  
        GPIOA, GPIO_PIN_10, GPIOB, GPIO_PIN_1    // Phase C
    );
    
    BLDCDriver6PWM_init(&driver6pwm, 24.0f, 12.0f, 0.5f); // 24V supply, 12V limit, 0.5μs dead time
    BLDCMotor_init(&motor, 7);
    BLDCMotor_linkDriver(&motor, (BLDCDriver_t*)&driver6pwm);
    
    // Configure for velocity control
    motor.foc_motor.controller = ControlType_velocity_openloop;
    motor.foc_motor.voltage_limit = 8.0f; // 8V max
    motor.foc_motor.velocity_limit = 10.0f; // 10 rad/s max
    
    // Enable driver
    driver6pwm.base.functions->enable((BLDCDriver_t*)&driver6pwm);
    
    // Example control loop
    float target_velocity = 5.0f; // 5 rad/s target
    
    // Move motor to target velocity
    BLDCMotor_move(&motor, target_velocity);
    
    printf("6PWM Velocity control example running at %.1f rad/s\n", target_velocity);
}

/*
 * Key advantages of 6PWM over 3PWM:
 * 
 * 1. Better voltage utilization - can output true bipolar voltages
 * 2. Reduced harmonic content - complementary switching reduces ripple
 * 3. Dead time protection - prevents shoot-through currents
 * 4. Higher efficiency - better control of power flow
 * 5. Better EMI performance - complementary switching reduces emissions
 * 
 * Hardware considerations for 6PWM:
 * - Need complementary PWM capable timer (like TIM1 on STM32)
 * - Require gate drivers for high and low side switches
 * - Dead time must be configured in hardware timer
 * - Need proper PCB layout for high current switching
 */