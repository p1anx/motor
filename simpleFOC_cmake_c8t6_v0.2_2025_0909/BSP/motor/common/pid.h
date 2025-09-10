/*
 * pid.h
 * Ported from: https://github.com/simplefoc/Arduino-FOC/blob/master/src/common/pid.h
 * Converted from C++ to C
 */

#ifndef PID_H_
#define PID_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "time_utils.h"
#include "foc_utils.h"

/**
 * PID controller structure
 */
typedef struct {
    float P; //!< Proportional gain
    float I; //!< Integral gain
    float D; //!< Derivative gain
    float output_ramp; //!< Maximum speed of change of the output value
    float limit; //!< Maximum output value
    
    float integral_prev; //!< last integral component value
    float error_prev; //!< last tracking error value
    unsigned long timestamp_prev; //!< Last execution timestamp
    float output_prev;  //!< last pid output value
} PIDController;

/**
 * Initialize PID controller
 * @param pid - Pointer to PID controller structure
 * @param P - Proportional gain
 * @param I - Integral gain
 * @param D - Derivative gain
 * @param ramp - Maximum speed of change of the output value
 * @param limit - Maximum output value
 */
void PIDController_init(PIDController* pid, float P, float I, float D, float ramp, float limit);

/**
 * Calculate PID controller output
 * @param pid - Pointer to PID controller structure
 * @param error - Current error value
 * @return PID controller output
 */
float PIDController_calculate(PIDController* pid, float error);

#ifdef __cplusplus
}
#endif

#endif /* PID_H_ */
