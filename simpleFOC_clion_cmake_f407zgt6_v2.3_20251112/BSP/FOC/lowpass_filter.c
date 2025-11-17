
#include "lowpass_filter.h"

void LowPassFilter_init(LowPassFilter_t *lpf, float time_constant) {
  lpf->Tf = time_constant;
  lpf->y_prev = 0.0f;
  lpf->timestamp_prev = _micros();
}

float LowPassFilter(LowPassFilter_t *lpf, float x) {
  unsigned long timestamp = _micros();
  float dt = (timestamp - lpf->timestamp_prev) * 1e-3f;

  if (dt < 0.0f || dt > 0.5f)
    dt = 1e-3f;

  float alpha = lpf->Tf / (lpf->Tf + dt);
  float y = alpha * lpf->y_prev + (1.0f - alpha) * x;

  lpf->y_prev = y;
  lpf->timestamp_prev = timestamp;
  return y;
}
