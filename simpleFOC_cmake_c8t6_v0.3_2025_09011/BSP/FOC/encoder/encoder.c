#include "encoder.h"
#include "as5600.h"
#include "encoder_as5600.h"
float Encoder_getAngle(Encoder_t *enc) {
  enc->angle = AS5600_ReadAngleRadians(&enc->as5600);
  // return _2PI * (enc->pulse_counter) / ((float)enc->cpr);
  return enc->angle;
}

/*
  Shaft velocity calculation
  function using mixed time and frequency measurement technique
*/
float Encoder_getVelocity(Encoder_t *enc) {

  float speedRPM = AS5600_GetAngularSpeed(&enc->as5600, AS5600_MODE_RPM, 1);
  return speedRPM;
}
