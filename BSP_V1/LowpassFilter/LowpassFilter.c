
#include "LowpassFilter.h"
#include "stm32_hal.h"

void LowPassFilter_init(LowPassFilter_t *lpf, float time_constant, const float* pUpdate_t) {
  lpf->Tf = time_constant;
  lpf->y_prev = 0.0f;
  lpf->timestamp_prev = getUs();
  lpf->dt = *pUpdate_t;
}

float LowPassFilter(LowPassFilter_t *lpf, float x) {

#ifdef IS_FIXED_TIME
  float dt = lpf->dt;
#else
  uint32_t timestamp = getUs();
  float dt = (timestamp - lpf->timestamp_prev) * 1e-6f;
  lpf->timestamp_prev = timestamp;
  lpf->dt = dt;

  if (dt < 0.0f || dt > 0.5f)
    dt = 1e-3f;
#endif


  float alpha = lpf->Tf / (lpf->Tf + dt);
  float y = alpha * lpf->y_prev + (1.0f - alpha) * x;

  lpf->y_prev = y;
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


