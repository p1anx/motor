/*
 * lowpass_filter.h
 * Ported from: https://github.com/simplefoc/Arduino-FOC/blob/master/src/common/lowpass_filter.h
 * Converted from C++ to C
 */

#ifndef LOWPASS_FILTER_H_
#define LOWPASS_FILTER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "time_utils.h"
#include "foc_utils.h"

/**
 * Low pass filter structure
 */
typedef struct {
    float Tf; //!< Low pass filter time constant
    unsigned long timestamp_prev;  //!< Last execution timestamp
    float y_prev; //!< filtered value in previous execution step
} LowPassFilter;

/**
 * Initialize low pass filter
 * @param filter - Pointer to low pass filter structure
 * @param Tf - Low pass filter time constant
 */
void LowPassFilter_init(LowPassFilter* filter, float Tf);

/**
 * Calculate filtered value
 * @param filter - Pointer to low pass filter structure
 * @param x - Input value to filter
 * @return Filtered output value
 */
float LowPassFilter_calculate(LowPassFilter* filter, float x);

#ifdef __cplusplus
}
#endif

#endif /* LOWPASS_FILTER_H_ */
