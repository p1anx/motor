#include "main_foc.h"

unsigned long _micros() {
  return HAL_GetTick(); // get microseconds
}

void _writeDutyCycle3PWM(float dc_a, float dc_b, float dc_c, int pinA, int pinB,
                         int pinC) {
  // transform duty cycle from [0,1] to [0,4095]
  // TIM1->CCR1 = (int)(dc_a * _PWM_RANGE);
  //	TIM1->CCR2 = (int)(dc_b * _PWM_RANGE);
  //	TIM4->CCR4 = (int)(dc_c * _PWM_RANGE);

  int pwm_range = htim1.Instance->ARR;
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, dc_a * pwm_range);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, dc_b * pwm_range);
  pwm_range = htim4.Instance->ARR;
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, dc_c * pwm_range);
}

void main_foc(void) {
  BLDCMotor motor = BLDCMotor(7); //,0.039);//,0.039); //(pp,phase_resistance)
  BLDCDriver3PWM driver = BLDCDriver3PWM(5, 9, 6, 8);
  Encoder sensor =
      Encoder(2, 3, 2048, 11); // these pins, and values are actually hardcoded
                               // 14, angle register address = 0x3fff); //these
                               // pins are all hardcoded
}
