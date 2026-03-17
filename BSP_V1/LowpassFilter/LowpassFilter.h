#ifndef LOWPASS_FILTER_H_
#define LOWPASS_FILTER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "foc.h"
#include <stdbool.h>
// #include "time_utils.h"

/**
 * Low pass filter structure
 */
typedef struct {
  float Tf;                     //!< Low pass filter time constant
  unsigned int timestamp_prev; //!< Last execution timestamp
  float y_prev;                 //!< filtered value in previous execution step
  float filtered_value;    // 滤波后的值
  float alpha;            // 滤波系数 (0 < alpha < 1)
  float prev_filtered;    // 上一次滤波值
  float dt;
} LowPassFilter_t;

  /**
 * Initialize low pass filter
 * @param filter - Pointer to low pass filter structure
 * @param Tf - Low pass filter time constant
 */
// void LowPassFilter_init(LowPassFilter_t *filter, float Tf);
void LowPassFilter_init(LowPassFilter_t *lpf, float time_constant, const float* pUpdate_t);

/**
 * Calculate filtered value
 * @param filter - Pointer to low pass filter structure
 * @param x - Input value to filter
 * @return Filtered output value
 */
float LowPassFilter(LowPassFilter_t *filter, float x);
float lpf_process(LowPassFilter_t *lpf, float input);
void lpf_init(LowPassFilter_t *lpf, float cutoff_freq, float sampling_freq);

#ifdef __cplusplus
}
#endif

#endif /* LOWPASS_FILTER_H_ */
