#include "encoder.h"
#include "encoder_as5600.h"
float Encoder_getAngle(Encoder_t *enc)
{
    enc->angle = AS5600_ReadAngleRadians(enc->as5600);
    // return _2PI * (enc->pulse_counter) / ((float)enc->cpr);
    return enc->angle;
}

/*
  Shaft velocity calculation
  function using mixed time and frequency measurement technique
*/
float Encoder_getVelocity(Encoder_t *enc)
{

    float speedRPM = AS5600_GetAngularSpeed(enc->as5600, AS5600_MODE_RPM, 1);
    return speedRPM;
}

void Encoder_init(Encoder_t *encoder)
{
    Encoder_AS5600_Init(encoder->as5600, encoder->as5600->hi2c);
}
void Encoder_linkAS5600(Encoder_t *encoder, AS5600_t *as5600)
{
    encoder->as5600 = as5600;
}
