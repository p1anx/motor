#ifndef __MOTOR_CONFIG_H
#define __MOTOR_CONFIG_H
// #include "gpio.h"
#include "mymain.h"

#define CONFIG_FOR_MOTOR3508 1
#define CONFIG_FOR_MOTOR2804 0

#define CONFIG_DRIVER_R_SAMPLE 0.001 //ohm
#define CONFIG_DRIVER_GAIN     50
//0.005
#define CONFIG_VELOCITY_FILTER_Tf 0.05    //!< default velocity filter time constant
#define CONFIG_CURRENT_FILTER_Tf 0.8 //!< default velocity filter time constant

#define CONFIG_CURRENT_SAMPLE_MODE 0 // 0: High side Sample 1: Low side Sample

//=====================================================
//                   MOTOR DEBUG
//=====================================================
#define MOTOR_DEBUG_MODE  1
#define DEBUG_PID_CURRENT_ID 0 //option: `1` = debug id without iq; `0` = debug iq id


//=====================================================
//                   MOTOR 3508 CONFIG
//=====================================================
#define MOTOR3508_VoltageSupply 15
#define MOTOR3508_VoltageLimit  15
#define MOTOR3508_PWM_FREQUENCY 10e3
#define MOTOR3508_PWM_RESOLUTION 4096
#define MOTOR3508_PWMDriverType  DriverTye_3PWM
#define MOTOR3508_PP   7//pole pairs

//=====================================================
//                   MOTOR 2804 CONFIG
//=====================================================
#define MOTOR2804_VoltageSupply 12
#define MOTOR2804_VoltageLimit  12
#define MOTOR2804_PWM_FREQUENCY 10e3
#define MOTOR2804_PWM_RESOLUTION 4096
#define MOTOR2804_PWMDriverType  DriverTye_3PWM
#define MOTOR2804_PP   7//pole pairs


#define CONFIG_IS_TEST 1
#define CONFIG_IS_MAIN 0
#define CONFIG_FILTER_Tf 0.08    //!< default velocity filter time constant
#define CONFIG_MOTOR2804_RS 2.55f // omh phase resistence
#define CONFIG_MOTOR2804_LS 0.86 //mH
#define CONFIG_MOTOR2804_FLUX 0.0035 //Wb
#define CONFIG_OUPUT_DEG_VELOCITY_RAMP 360*10

//config for motor 3508
#if CONFIG_FOR_MOTOR3508
#define CONFIG_VoltageSupply   MOTOR3508_VoltageSupply
#define CONFIG_VoltageLimit    MOTOR3508_VoltageLimit
#define CONFIG_PWM_HZ          MOTOR3508_PWM_FREQUENCY
#define CONFIG_PWM_RESOLUTION  MOTOR3508_PWM_RESOLUTION
#define CONFIG_PWM_DriverType  MOTOR3508_PWMDriverType // DriverTye_3PWM or DriverTye_6PWM  0:3pwm, 1:6pwm
#define CONFIG_PP              MOTOR3508_PP
#endif

#if CONFIG_FOR_MOTOR2804
#define CONFIG_VoltageSupply   MOTOR2804_VoltageSupply
#define CONFIG_VoltageLimit    MOTOR2804_VoltageLimit
#define CONFIG_PWM_HZ          MOTOR2804_PWM_FREQUENCY
#define CONFIG_PWM_RESOLUTION  MOTOR2804_PWM_RESOLUTION
#define CONFIG_PWM_DriverType  MOTOR2804_PWMDriverType // DriverTye_3PWM or DriverTye_6PWM  0:3pwm, 1:6pwm
#define CONFIG_PP              MOTOR2804_PP
#endif


#define BLDCMOTOR_ENABLE_PORT GPIOD
#define BLDCMOTOR_ENABLE_PIN GPIO_PIN_13



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
