#ifndef LOWPASS_FILTER_H_
#define LOWPASS_FILTER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "foc_base.h"
#include <stdbool.h>
// #include "time_utils.h"

/**
 * Low pass filter structure
 */
typedef struct {
  float Tf;                     //!< Low pass filter time constant
  unsigned long timestamp_prev; //!< Last execution timestamp
  float y_prev;                 //!< filtered value in previous execution step
  float filtered_value;    // 滤波后的值
  float alpha;            // 滤波系数 (0 < alpha < 1)
  float prev_filtered;    // 上一次滤波值
} LowPassFilter_t;

  typedef struct {
    float *buffer;          // 环形缓冲区
    uint16_t size;         // 滤波窗口大小
    uint16_t index;        // 当前写入索引
    float sum;             // 当前窗口内数值总和
    bool initialized;      // 缓冲区是否已初始化
  } MovingAverageFilter;
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
float lpf_process(LowPassFilter_t *lpf, float input);
void lpf_init(LowPassFilter_t *lpf, float cutoff_freq, float sampling_freq);
float process_moving_average(MovingAverageFilter *filter, float new_value);

  bool init_moving_average_filter(MovingAverageFilter *filter,
                                 float *buffer,
                                 uint16_t size);

#ifdef __cplusplus
}
#endif

#endif /* LOWPASS_FILTER_H_ */
