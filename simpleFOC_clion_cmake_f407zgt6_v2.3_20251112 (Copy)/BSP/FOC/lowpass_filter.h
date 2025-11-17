#ifndef LOWPASS_FILTER_H_
#define LOWPASS_FILTER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "foc_base.h"
// #include "time_utils.h"

/**
 * Low pass filter structure
 */
typedef struct {
  float Tf;                     //!< Low pass filter time constant
  unsigned long timestamp_prev; //!< Last execution timestamp
  float y_prev;                 //!< filtered value in previous execution step
} LowPassFilter_t;

/**
 * Initialize low pass filter
 * @param filter - Pointer to low pass filter structure
 * @param Tf - Low pass filter time constant
 */
void LowPassFilter_init(LowPassFilter_t *filter, float Tf);

/**
 * Calculate filtered value
 * @param filter - Pointer to low pass filter structure
 * @param x - Input value to filter
 * @return Filtered output value
 */
float LowPassFilter(LowPassFilter_t *filter, float x);

#ifdef __cplusplus
}
#endif

#endif /* LOWPASS_FILTER_H_ */
