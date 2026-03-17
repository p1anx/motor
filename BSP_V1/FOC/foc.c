//
// Created by xwj on 1/13/26.
//

#include "foc.h"
#include <arm_math.h>
#include <stdio.h>

float _sin(float a) {
  return arm_sin_f32(a);
}
float _cos(float a) {
  return arm_cos_f32(a);
}

void Clarke_Transform(float Ia, float Ib, float *I_alpha, float *I_beta)
{
  *I_alpha = Ia;
  *I_beta = _1_SQRT3 * (Ia + 2.0f * Ib);
}

/**
 * @brief Park变换：两相静止坐标系转换为两相旋转坐标系
 */
void Park_Transform(float I_alpha, float I_beta, float theta, float *Id, float *Iq)
{
  float cos_theta = _cos(theta);
  float sin_theta = _sin(theta);

  *Id = I_alpha * cos_theta + I_beta * sin_theta;
  *Iq = -I_alpha * sin_theta + I_beta * cos_theta;
  // *Iq = I_alpha * cos_theta + I_beta * sin_theta;
  // *Id =-I_alpha * sin_theta + I_beta * cos_theta;
}

void Clark_Park_Transform(float Ia, float Ib, float *Id, float *Iq, float e_angle)
{
  float I_alpha, I_beta, theta = e_angle;
  float cos_theta = _cos(theta);
  float sin_theta = _sin(theta);
  I_alpha = Ia;
  I_beta = _1_SQRT3 * (Ia + 2.0f * Ib);

  *Id = I_alpha * cos_theta + I_beta * sin_theta;
  *Iq = -I_alpha * sin_theta + I_beta * cos_theta;

}

void Clarke_Park(float*Ia, float* Ib,float* Ic, float *Id, float *Iq, float e_angle)
{
  float I_alpha, I_beta, theta = e_angle;
  float cos_theta = _cos(theta);
  float sin_theta = _sin(theta);
  float i_a, i_b, i_c;

  if (!Ia) {
    i_a = -(*Ib + *Ic);
    i_b = *Ib;
    I_alpha = i_a;
    I_beta = _1_SQRT3 * (i_a + 2.0f * i_b);

  }
  else if (!Ib) {
    i_a = *Ia;
    i_b = -(*Ia + *Ic);
    I_alpha = i_a;
    I_beta = _1_SQRT3 * (i_a + 2.0f * i_b);
  }
  else if (!Ic) {
    i_a = *Ia;
    i_b = *Ib;
    I_alpha = *Ia;
    I_beta = _1_SQRT3 * (i_a + 2.0f * i_b);
  }
  else {
    i_a = *Ia;
    i_b = *Ib;
    i_c = *Ic;
    I_alpha = i_a;
    I_beta = _1_SQRT3 * (i_b - i_c);
  }


  *Id = I_alpha * cos_theta + I_beta * sin_theta;
  *Iq = -I_alpha * sin_theta + I_beta * cos_theta;

}

void Clark_Park_TransformIaIb(float Ia, float Ib, float *Id, float *Iq, float e_angle)
{
  float I_alpha, I_beta, theta = e_angle;
  float cos_theta = _cos(theta);
  float sin_theta = _sin(theta);
  I_alpha = Ia;
  I_beta = _1_SQRT3 * (Ia + 2.0f * Ib);

  *Id = I_alpha * cos_theta + I_beta * sin_theta;
  *Iq = -I_alpha * sin_theta + I_beta * cos_theta;

}
void Clark_Park_TransformIaIc(float Ia, float Ic, float *Id, float *Iq, float e_angle)
{
  float Ib = -(Ia + Ic);
  Clark_Park_Transform(Ia, Ib, Id, Iq, e_angle);
}
void Clark_Park_TransformIbIc(float Ib, float Ic, float *Id, float *Iq, float e_angle)
{
  float Ia = -(Ib + Ic);
  Clark_Park_Transform(Ia, Ib, Id, Iq, e_angle);
}

void Debug_Transformations(void)
{
  printf("=== Coordinate Transform Check ===\n");

  // 测试：给定已知三相电流
  float Ia = 1.0f, Ib = -0.5f, Ic = -0.5f, i_alpha, i_beta, i_d, i_q;

  // Clarke变换
  Clarke_Transform(Ia, Ib, &i_alpha, &i_beta);
  printf("Clarke: alpha=%.3f, beta=%.3f\n", i_alpha, i_beta);

  // Park变换（angle=0时，d=alpha, q=beta）
  Park_Transform(i_alpha, i_beta, 0, &i_d, &i_q);
  printf("Park (angle=0): d=%.3f, q=%.3f\n", i_d, i_q);

  // 验证：angle=0时，d应该≈alpha, q应该≈-beta或beta
  if (fabsf(i_d - i_alpha) > 0.1f)
  {
    printf("⚠️  Park transform may be wrong!\n");
  }
}

// normalizing radian angle to [0,2PI]
float _normalizeAngle(float angle)
{
  float a = fmod(angle, _2PI);
  return a >= 0 ? a : (a + _2PI);
}
// Electrical angle calculation
float _electricalAngle(const float shaft_angle, const int pole_pairs)
{
  return _normalizeAngle(shaft_angle * (float)pole_pairs);
}

float _electricalAngle_calibrated(const int direction, const float shaft_angle, const int pole_pairs, const float zero_electric_angle)
{
  return _normalizeAngle((float)direction * shaft_angle * (float)pole_pairs - zero_electric_angle);
}
