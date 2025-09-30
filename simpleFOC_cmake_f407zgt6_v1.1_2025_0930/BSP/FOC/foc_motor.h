#ifndef __FOC_MOTOR_H
#define __FOC_MOTOR_H
#include "BLDCDriver.h"
#include "encoder.h"
#include "lowpass_filter.h"
#include "mymain.h"
#include "pid.h"
#include "sensor.h"

typedef enum
{
    ControlType_voltage,  //!< Torque control using voltage
    ControlType_current,  //!< Torque control using voltage
    ControlType_velocity, //!< Velocity motion control
    ControlType_angle,    //!< Position/angle motion control
    ControlType_velocity_openloop,
    ControlType_angle_openloop
} ControlType_t;

/**
 * Forward declaration for motor function pointers
 */
typedef struct FOCMotor_t FOCMotor_t;

/**
 * FOC Motor function pointers structure
 */
typedef struct
{
    /** Motor hardware init function */
    void (*init)(FOCMotor_t *motor);
    /** Motor disable function */
    void (*disable)(FOCMotor_t *motor);
    /** Motor enable function */
    void (*enable)(FOCMotor_t *motor);
    /**
     * Function initializing FOC algorithm
     * and aligning sensor's and motors' zero position
     */
    int (*initFOC)(FOCMotor_t *motor, float zero_electric_offset, Direction_t sensor_direction);
    /**
     * Function running FOC algorithm in real-time
     * it calculates the gets motor angle and sets the appropriate voltages
     * to the phase pwm signals
     */
    void (*loopFOC)(FOCMotor_t *motor);
    /**
     * Function executing the control loops set by the controller parameter
     */
    void (*move)(FOCMotor_t *motor, float target);
} FOCMotorFunctions_t;

/**
 * Generic FOC motor structure
 */
// int pwmA, pwmB, pwmC;
// int pole_pairs = 7;
// float voltage_limit = 10;
// float voltage_power_supply = 12.0;
// float velocity_limit = 5;
// float shaft_angle = 0, open_loop_timestamp = 0;
// float zero_electric_angle = 0, Ualpha, Ubeta = 0, Ua = 0, Ub = 0, Uc = 0,
//       dc_a = 0, dc_b = 0, dc_c = 0;
//
// int foc_modulation = SinePWM;
struct FOCMotor_t
{
    FOCMotorFunctions_t *functions; //!< Function pointer table
    BLDCDriver_t *dirver;

    // state variables
    float target;            //!< current target value - depends of the controller
    float shaft_angle;       //!< current motor angle
    float shaft_velocity;    //!< current motor velocity
    float shaft_velocity_sp; //!< current target velocity
    float shaft_angle_sp;    //!< current target angle
    // float Ualpha, Ubeta, Ua, Ub, Uc, dc_a, dc_b,
    //     dc_c; //!< current voltage u_q set
    float voltage_q; //!< current voltage u_d set
    float voltage_d; //!< current voltage u_d set
    float i_q;       //!< current voltage u_d set
    float i_d;       //!< current voltage u_d set

    // motor configuration parameters
    float voltage_sensor_align;  //!< sensor and motor align voltage parameter
    float velocity_index_search; //!< target velocity for index search
    int pole_pairs;              //!< Motor pole pairs number

    // limiting variables
    float voltage_limit;  //!< Voltage limiting variable - global limit
    float velocity_limit; //!< Velocity limiting variable - global limit

    float zero_electric_angle; //!< absolute zero electric angle - if available

    // configuration structures
    ControlType_t controller;           //!< parameter determining the control loop to be used
    FOCModulationType_t foc_modulation; //!< parameter determining modulation algorithm
    PIDController PID_velocity;         //!< parameter determining the velocity PI configuration
    PIDController PID_current;          //!< parameter determining the velocity PI configuration
    PIDController *PID_current_d;       //!< parameter determining the velocity PI configuration
    PIDController *PID_current_q;       //!< parameter determining the velocity PI configuration
    PIDController P_angle;              //!< parameter determining the
    // position P configuration
    LowPassFilter_t LPF_velocity; //!< parameter
    // determining the velocity Low pass filter configuration

    /**
     * Sensor link:
     * - Encoder
     * - MagneticSensor*
     * - HallSensor
     */
    Sensor_t *sensor;
    Encoder_t *encoder;
};

void FOCMotor_init(FOCMotor_t *motor);

float FOCMotor_shaftVelocity(FOCMotor_t *motor);

float FOCMotor_shaftAngle(FOCMotor_t *motor);

void FOCMotor_linkSensor(FOCMotor_t *motor, Sensor_t *_sensor);
void FOCMotor_linkEncoder(FOCMotor_t *motor, Encoder_t *encoder);

void FOCMotor_linkAS5600(FOCMotor_t *focmotor, AS5600_t *as5600);
#endif // !__FOC_MOTOR_H
