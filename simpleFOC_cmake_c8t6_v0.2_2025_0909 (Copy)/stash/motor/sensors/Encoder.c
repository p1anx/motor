#include "Encoder.h"
#include "as5600.h"
#include "main_motor.h"
/*
 * Ported from:
 * https://github.com/simplefoc/Arduino-FOC/blob/master/src/sensors/Encoder.cpp
 *
 * Encoder_init(Encoder* enc, int encA, int encB, int cpr, int index)
 * - encA, encB     - encoder A and B pins,
 * - cpr            - counts per rotation number (cpr = ppr * 4)
 * - index pin      - (optional input)
 *
 * DEFAULTS:
 * - pullup::EXTERN;
 * - quadrature::ON;
 */

void Encoder_init(Encoder_t *enc, int _encA, int _encB, float _ppr,
                  int _index) {

  // Encoder measurement structure init
  // hardware pins
  enc->pinA = _encA;
  enc->pinB = _encB;
  // counter setup
  enc->pulse_counter = 0;
  enc->pulse_timestamp = 0;

  enc->cpr = _ppr;
  enc->A_active = 0;
  enc->B_active = 0;
  enc->I_active = 0;
  // index pin
  enc->index_pin = _index; // its 0 if not used
  enc->index_pulse_counter = 0;

  // velocity calculation variables
  enc->prev_Th = 0;
  enc->pulse_per_second = 0;
  enc->prev_pulse_counter = 0;
  enc->prev_timestamp_us = _micros();

  // extern pullup as default
  enc->pullup = Pullup_EXTERN;
  // enable quadrature encoder by default
  enc->quadrature = Quadrature_ON;
}

//  Encoder interrupt callback functions
// A channel
void Encoder_handleA(Encoder_t *enc) {
  // unsigned short A = HAL_GPIO_ReadPin(Pin2_EnA_GPIO_Port, Pin2_EnA_Pin);
  //
  // if (A != enc->A_active) {
  //   enc->pulse_counter += (enc->A_active == enc->B_active) ? 1 : -1;
  //   enc->pulse_timestamp = _micros();
  //   enc->A_active = A;
  // }
}

// B channel
void Encoder_handleB(Encoder_t *enc) {
  // unsigned short B = HAL_GPIO_ReadPin(Pin3_EnB_GPIO_Port, Pin3_EnB_Pin);
  //
  // if (B != enc->B_active) {
  //   enc->pulse_counter += (enc->A_active != enc->B_active) ? 1 : -1;
  //   enc->pulse_timestamp = _micros();
  //   enc->B_active = B;
  // }
}

/*
        Shaft angle calculation
*/
float Encoder_getAngle(Encoder_t *enc) {
  enc->angle = getAngle();
  // return _2PI * (enc->pulse_counter) / ((float)enc->cpr);
  return enc->angle;
}

/*
  Shaft velocity calculation
  function using mixed time and frequency measurement technique
*/
float Encoder_getVelocity(Encoder_t *enc) {
  // timestamp
  long timestamp_us = _micros();
  // sampling time calculation
  float Ts = (timestamp_us - enc->prev_timestamp_us) * 1e-6;
  // quick fix for strange cases (micros overflow)
  if (Ts <= 0 || Ts > 0.5)
    Ts = 1e-3;

  // time from last impulse
  float Th = (timestamp_us - enc->pulse_timestamp) * 1e-6;
  long dN = enc->pulse_counter - enc->prev_pulse_counter;

  // Pulse per second calculation (Eq.3.)
  // dN - impulses received
  // Ts - sampling time - time in between function calls
  // Th - time from last impulse
  // Th_1 - time from last impulse of the previous call
  // only increment if some impulses received
  float dt = Ts + enc->prev_Th - Th;
  enc->pulse_per_second =
      (dN != 0 && dt > Ts / 2) ? dN / dt : enc->pulse_per_second;

  // if more than 0.05 passed in between impulses
  if (Th > 0.1)
    enc->pulse_per_second = 0;

  // velocity calculation
  float velocity = enc->pulse_per_second / ((float)enc->cpr) * (_2PI);

  // save variables for next pass
  enc->prev_timestamp_us = timestamp_us;
  // save velocity calculation variables
  enc->prev_Th = Th;
  enc->prev_pulse_counter = enc->pulse_counter;
  return velocity;
}

// getter for index pin
// return -1 if no index
int Encoder_needsAbsoluteZeroSearch(Encoder_t *enc) {
  return enc->index_pulse_counter == 0;
}

// getter for index pin
int Encoder_hasAbsoluteZero(Encoder_t *enc) { return Encoder_hasIndex(enc); }

// initialize counter to zero
float Encoder_initRelativeZero(Encoder_t *enc) {
  long angle_offset = -enc->pulse_counter;
  enc->pulse_counter = 0;
  enc->pulse_timestamp = _micros();
  return _2PI * (angle_offset) / ((float)enc->cpr);
}

// initialize index to zero
float Encoder_initAbsoluteZero(Encoder_t *enc) {
  enc->pulse_counter -= enc->index_pulse_counter;
  enc->prev_pulse_counter = enc->pulse_counter;
  return (enc->index_pulse_counter) / ((float)enc->cpr) * (_2PI);
}

// private function used to determine if encoder has index
int Encoder_hasIndex(Encoder_t *enc) { return enc->index_pin != 0; }

// encoder initialisation of the hardware pins
// and calculation variables
void Encoder_init_hardware(Encoder_t *enc) {

  // counter setup
  enc->pulse_counter = 0;
  enc->pulse_timestamp = _micros();
  // velocity calculation variables
  enc->prev_Th = 0;
  enc->pulse_per_second = 0;
  enc->prev_pulse_counter = 0;
  enc->prev_timestamp_us = _micros();

  // initial cpr = PPR
  // change it if the mode is quadrature
  if (enc->quadrature == Quadrature_ON)
    enc->cpr = 4 * enc->cpr;
}
