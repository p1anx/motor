#ifndef __OPEN_LOOP_FOC_H
#define __OPEN_LOOP_FOC_H
#include "mymain.h"
#include "tim.h"

// sign function
#define _sign(a) (((a) < 0) ? -1 : ((a) > 0))
#define _round(x) ((x) >= 0 ? (long)((x) + 0.5) : (long)((x) - 0.5))
#define _constrain(amt, low, high)                                             \
  ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

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

#define NOT_SET -12345.0

/**
 *  Function approximating the sine calculation by using fixed size array
 * - execution time ~40us (Arduino UNO)
 *
 * @param a angle in between 0 and 2PI
 */
float _sin(float a);
/**
 * Function approximating cosine calculation by using fixed size array
 * - execution time ~50us (Arduino UNO)
 *
 * @param a angle in between 0 and 2PI
 */
float _cos(float a);

/**
 * normalizing radian angle to [0,2PI]
 * @param angle - angle to be normalized
 */
float _normalizeAngle(float angle);

unsigned long _micros();

/**
 * Electrical angle calculation
 *
 * @param shaft_angle - shaft angle of the motor
 * @param pole_pairs - number of pole pairs
 */

/**
 *  FOC modulation type
 */
// enum FOCModulationType{
//   SinePWM, //!< Sinusoidal PWM modulation
//   SpaceVectorPWM, //!< Space vector modulation method
//   Trapezoid_120,
//   Trapezoid_150
// };

#define SinePWM 0        //!< Sinusoidal PWM modulation
#define SpaceVectorPWM 1 //!< Space vector modulation method
#define Trapezoid_120 2
#define Trapezoid_150 3
// FOCModulationType foc_modulation;//!<  parameter derterniming modulation
// algorithm
float _electricalAngle(float shaft_angle, int pole_pairs);
// float _electricalAngle(void);
void setPwm(float Ua, float Ub, float Uc);
float velocityOpenloop(float target_velocity);
void angleOpenloop(float target_angle);
void test_pid(void);
void setTorque(float Uq, float angle_el);
void FOC_M0_setVelocity(float Target);
#endif
