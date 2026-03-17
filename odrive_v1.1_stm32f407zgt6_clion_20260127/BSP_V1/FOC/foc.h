//
// Created by xwj on 1/13/26.
//

#ifndef ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_FOC_H
#define ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_FOC_H

#define _constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

// utility defines
#define _2_SQRT3 1.15470053838
#define _SQRT3 1.73205080757
#define _1_SQRT3 0.57735026919
#define _SQRT3_2 0.86602540378
#define _SQRT2 1.41421356237
#define _120_D2R 2.09439510239
#define _PI 3.14159265359
#define _PI_2 1.57079632679
#define _PI_3 1.0471975512
#define _2PI 6.28318530718
#define _3PI_2 4.71238898038

float _sin(float a);
float _cos(float a);

void Clarke_Transform(float Ia, float Ib, float *I_alpha, float *I_beta);
void Park_Transform(float I_alpha, float I_beta, float theta, float *Id, float *Iq);
void Clark_Park_Transform(float Ia, float Ib, float *Id, float *Iq, float e_angle);
void Clark_Park_TransformIaIc(float Ia, float Ic, float *Id, float *Iq, float e_angle);
float _normalizeAngle(float angle);
float _electricalAngle(const float shaft_angle, const int pole_pairs);
float _electricalAngle_calibrated(const int direction, const float shaft_angle, const int pole_pairs, const float zero_electric_angle);
void Clarke_Park(float*Ia, float* Ib,float* Ic, float *Id, float *Iq, float e_angle);
#endif // ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_FOC_H
