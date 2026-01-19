
#include "lowpass_filter.h"
#include "stm32_hal.h"

void LowPassFilter_init(LowPassFilter_t *lpf, float time_constant) {
  lpf->Tf = time_constant;
  lpf->y_prev = 0.0f;
  lpf->timestamp_prev = getUs();
}

float LowPassFilter(LowPassFilter_t *lpf, float x) {
  uint32_t timestamp = getUs();
  float dt = (timestamp - lpf->timestamp_prev) * 1e-6f;

  if (dt < 0.0f || dt > 0.5f)
    dt = 1e-3f;

  float alpha = lpf->Tf / (lpf->Tf + dt);
  float y = alpha * lpf->y_prev + (1.0f - alpha) * x;

  lpf->y_prev = y;
  lpf->timestamp_prev = timestamp;
  return y;
}
// 初始化一阶低通滤波器
void lpf_init(LowPassFilter_t *lpf, float cutoff_freq, float sampling_freq) {
  float rc = 1.0f / (2.0f * 3.14159f * cutoff_freq);
  lpf->alpha = 1.0f / (1.0f + rc * sampling_freq);
  lpf->filtered_value = 0.0f;
  lpf->prev_filtered = 0.0f;
}

// 一阶低通滤波函数
float lpf_process(LowPassFilter_t *lpf, float input) {
  lpf->filtered_value = lpf->alpha * input + (1.0f - lpf->alpha) * lpf->prev_filtered;
  lpf->prev_filtered = lpf->filtered_value;
  return lpf->filtered_value;
}
//=======================================================
//                    mean filter
//=======================================================
#include <stdint.h>
#include <stdbool.h>


/**
 * @brief 初始化移动均值滤波器
 * @param filter 滤波器实例指针
 * @param buffer 缓冲区数组
 * @param size 窗口大小
 * @return true成功，false失败
 */
bool init_moving_average_filter(MovingAverageFilter *filter,
                               float *buffer,
                               uint16_t size) {
  if (filter == NULL || buffer == NULL || size == 0) {
    return false;
  }

  filter->buffer = buffer;
  filter->size = size;
  filter->index = 0;
  filter->sum = 0.0f;
  filter->initialized = false;

  // 清空缓冲区
  for (uint16_t i = 0; i < size; i++) {
    buffer[i] = 0.0f;
  }

  return true;
}

/**
 * @brief 处理新采样值
 * @param filter 滤波器实例指针
 * @param new_value 新采样值
 * @return 滤波后的结果
 */
float process_moving_average(MovingAverageFilter *filter, float new_value) {
  if (filter == NULL || filter->buffer == NULL) {
    return 0.0f;
  }

  // 计算要移除的旧值
  float old_value = filter->buffer[filter->index];

  // 更新总和：减去旧值，加上新值
  filter->sum = filter->sum - old_value + new_value;

  // 将新值存入缓冲区
  filter->buffer[filter->index] = new_value;

  // 更新索引（环形缓冲）
  filter->index = (filter->index + 1) % filter->size;

  // 如果还未填满整个窗口，返回当前平均值
  if (!filter->initialized && filter->index == 0) {
    filter->initialized = true;
  }

  // 计算并返回平均值
  uint16_t valid_samples = filter->initialized ? filter->size : filter->index;
  if (valid_samples == 0) valid_samples = 1;

  return filter->sum / (float)valid_samples;
}
