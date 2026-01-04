#ifndef __MOTOR_CONFIG_H
#define __MOTOR_CONFIG_H
// #include "gpio.h"
#include "mymain.h"
#include "stm32_hal.h"

#define CONFIG_FOR_MOTOR3508 1
#define CONFIG_FOR_MOTOR2804 0

#define CONFIG_VOFA_UART huart4

#define CONFIG_CUR_FREQ  10e3              //current loop frequency(Hz)
#define CONFIG_CUR_VEL_FREQ  (10e3/5)      //velocity loop frequency(Hz)
#define CONFIG_CUR_VEL_POS_FREQ  (10e3/10)      //position loop frequency(Hz)

//================================================
//             ODrive Sample R and Gain
//================================================
#define CONFIG_ODRIVE_R_SAMPLE 0.001
#define CONFIG_ODRIVE_GAIN     10

#define CONFIG_DRIVER_ENABLE_PORT GPIOA
#define CONFIG_DRIVER_ENABLE_PIN  GPIO_PIN_9
#define CONFIG_DRIVER_PWM_HTIM    htim1

#if 1
#define CONFIG_DRIVER_R_SAMPLE CONFIG_ODRIVE_R_SAMPLE //ohm 0.001
#define CONFIG_DRIVER_GAIN     CONFIG_ODRIVE_GAIN    // 50
#elif
#define CONFIG_DRIVER_R_SAMPLE 0.02 //ohm 0.001
#define CONFIG_DRIVER_GAIN     6    // 50

#endif

//================================================
//                  MT6835
//================================================
#define CONFIG_MT6835_SPI_CS_PORT  GPIOC
#define CONFIG_MT6835_SPI_CS_PIN   GPIO_PIN_4
#define CONFIG_MT6835_HSPI         hspi3

//================================================
//                  STM32 ADC
//================================================
#define CONFIG_STM32_ADC           hadc1
#define CONFIG_STM32_ADC_16BITS    65535
#define CONFIG_STM32_ADC_12BITS    4096
#define CONFIG_STM32_ADC_RESOLUTION CONFIG_STM32_ADC_12BITS

#define CONFIG_US_TIM              htim2

//================================================
//                  Filter
//================================================
//0.005
#define CONFIG_VELOCITY_FILTER_Tf 0.01    //!< default velocity filter time constant
#define CONFIG_CURRENT_FILTER_Tf  0.8 //!< default velocity filter time constant
#define CONFIG_DEGREE_FILTER_Tf  0.01 //!< default velocity filter time constant
#define CONFIG_IA_LPF_CUTOFF  50 //!< default velocity filter time constant
#define CONFIG_IB_LPF_CUTOFF  50 //!< default velocity filter time constant
#define CONFIG_ID_LPF_CUTOFF  5 //!< default velocity filter time constant
#define CONFIG_IQ_LPF_CUTOFF  5 //!< default velocity filter time constant


#define CONFIG_CURRENT_SAMPLE_MODE 1 // 0: High side Sample 1: Low side Sample

//=====================================================
//                   MOTOR DEBUG
//=====================================================
#define MOTOR_DEBUG_MODE  1
#define DEBUG_PID_CURRENT_ID 0 //option: `1` = debug id without iq; `0` = debug iq id


//=====================================================
//                   MOTOR 3508 CONFIG
//=====================================================
#define MOTOR3508_VoltageSupply 15
#define MOTOR3508_VoltageLimit  10
#define MOTOR3508_PWM_FREQUENCY 10e3
#define MOTOR3508_PWM_RESOLUTION 4096
#define MOTOR3508_PWMDriverType  DriverTye_6PWM
#define MOTOR3508_PP   7//pole pairs
#define MOTOR3508_MT6835_DIRECTION    0
//Aligh voltage
#define MOTOR3508_ALIGH_VOLTAGE      0.5f
#define MOTOR3508_OPENLOOP_VOLTAGE   0.5f

//=====================================================
//                   MOTOR 2804 CONFIG
//=====================================================
#define MOTOR2804_VoltageSupply 12
#define MOTOR2804_VoltageLimit  12
#define MOTOR2804_PWM_FREQUENCY 10e3
#define MOTOR2804_PWM_RESOLUTION 4096
#define MOTOR2804_PWMDriverType  DriverTye_6PWM
#define MOTOR2804_PP   7//pole pairs
#define MOTOR2804_MT6835_DIRECTION    0
#define MOTOR2804_ALIGH_VOLTAGE      2.f
#define MOTOR2804_OPENLOOP_VOLTAGE   2.f


//=====================================================
//                config for motor
//=====================================================
#define CONFIG_IS_TEST 1
#define CONFIG_IS_MAIN 0
#define CONFIG_FILTER_Tf 0.08    //!< default velocity filter time constant
#define CONFIG_MOTOR2804_RS 2.55f // omh phase resistence
#define CONFIG_MOTOR2804_LS 0.86 //mH
#define CONFIG_MOTOR2804_FLUX 0.0035 //Wb
#define CONFIG_OUPUT_DEG_VELOCITY_RAMP 3600

// choose motor type
#if CONFIG_FOR_MOTOR3508

#define CONFIG_VoltageSupply   MOTOR3508_VoltageSupply
#define CONFIG_VoltageLimit    MOTOR3508_VoltageLimit
#define CONFIG_PWM_HZ          MOTOR3508_PWM_FREQUENCY
#define CONFIG_PWM_RESOLUTION  MOTOR3508_PWM_RESOLUTION
#define CONFIG_PWM_DriverType  MOTOR3508_PWMDriverType // DriverTye_3PWM or DriverTye_6PWM  0:3pwm, 1:6pwm
#define CONFIG_PP              MOTOR3508_PP
#define CONFIG_MT6835_DIRECTION    MOTOR3508_MT6835_DIRECTION
#define CONFIG_ALIGH_VOLTAGE      MOTOR3508_ALIGH_VOLTAGE
#define CONFIG_OPENLOOP_VOLTAGE   MOTOR3508_OPENLOOP_VOLTAGE


#endif

#if CONFIG_FOR_MOTOR2804
#define CONFIG_VoltageSupply   MOTOR2804_VoltageSupply
#define CONFIG_VoltageLimit    MOTOR2804_VoltageLimit
#define CONFIG_PWM_HZ          MOTOR2804_PWM_FREQUENCY
#define CONFIG_PWM_RESOLUTION  MOTOR2804_PWM_RESOLUTION
#define CONFIG_PWM_DriverType  MOTOR2804_PWMDriverType // DriverTye_3PWM or DriverTye_6PWM  0:3pwm, 1:6pwm
#define CONFIG_PP              MOTOR2804_PP
#define CONFIG_MT6835_DIRECTION    MOTOR2804_MT6835_DIRECTION

#define CONFIG_ALIGH_VOLTAGE      MOTOR2804_ALIGH_VOLTAGE
#define CONFIG_OPENLOOP_VOLTAGE   MOTOR2804_OPENLOOP_VOLTAGE
#endif





#define DEF_POWER_SUPPLY 2 //!< default power supply voltage
// velocity PI controller params
#define DEF_PID_VEL_P 0.5         //!< default PID controller P value
#define DEF_PID_VEL_I 10.0        //!<  default PID controller I value
#define DEF_PID_VEL_D 0.0         //!<  default PID controller D value
#define DEF_PID_VEL_U_RAMP 10.0 //!< default PID controller voltage ramp value
// angle P params
#define DEF_P_ANGLE_P 10.0 //!< default P controller P value
#define DEF_VEL_LIM 2.0    //!< angle velocity limit default
// index search
#define DEF_INDEX_SEARCH_TARGET_VELOCITY 1.0 //!< default index search velocity
// align voltage
#define DEF_VOLTAGE_SENSOR_ALIGN 6.0 //!< default voltage for sensor and motor zero alignemt
// low pass filter velocity
#define DEF_VEL_FILTER_Tf 0.05    //!< default velocity filter time constant
#define DEF_CURRENT_FILTER_Tf 0.8 //!< default velocity filter time constant

#endif // !__MOTOR_CONFIG_H
