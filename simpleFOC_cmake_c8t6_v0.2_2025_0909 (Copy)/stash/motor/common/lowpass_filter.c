/*
 * lowpass_filter.c
 * Ported from:
 * https://github.com/simplefoc/Arduino-FOC/blob/master/src/common/lowpass_filter.cpp
 */

#ifndef LOWPASS_FILTER_C_
#define LOWPASS_FILTER_C_

#include "lowpass_filter.h"

void LowPassFilter_init(LowPassFilter *lpf, float time_constant) {
  lpf->Tf = time_constant;
  lpf->y_prev = 0.0f;
  lpf->timestamp_prev = _micros();
}

float LowPassFilter_filter(LowPassFilter *lpf, float x) {
  unsigned long timestamp = _micros();
  float dt = (timestamp - lpf->timestamp_prev) * 1e-6f;

  if (dt < 0.0f || dt > 0.5f)
    dt = 1e-3f;

  float alpha = lpf->Tf / (lpf->Tf + dt);
  float y = alpha * lpf->y_prev + (1.0f - alpha) * x;

  lpf->y_prev = y;
  lpf->timestamp_prev = timestamp;
  return y;
}

#endif /* LOWPASS_FILTER_C_ */
