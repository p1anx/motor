#ifndef __OPEN_LOOP_FOC_H
#define __OPEN_LOOP_FOC_H
#include "mymain.h"
#include "tim.h"

// sign function
// #define _sign(a) (((a) < 0) ? -1 : ((a) > 0))
// #define _round(x) ((x) >= 0 ? (long)((x) + 0.5) : (long)((x) - 0.5))
// #define _constrain(amt, low, high) \
//   ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
//
// // utility defines
// #define _2_SQRT3 1.15470053838
// #define _SQRT3 1.73205080757
// #define _1_SQRT3 0.57735026919
// #define _SQRT3_2 0.86602540378
// #define _SQRT2 1.41421356237
// #define _120_D2R 2.09439510239
// #define _PI 3.14159265359
// #define _PI_2 1.57079632679
// #define _PI_3 1.0471975512
// #define _2PI 6.28318530718
// #define _3PI_2 4.71238898038
//
// #define NOT_SET -12345.0
//
// /**
//  *  Function approximating the sine calculation by using fixed size array
//  * - execution time ~40us (Arduino UNO)
//  *
//  * @param a angle in between 0 and 2PI
//  */
// float _sin(float a);
// /**
//  * Function approximating cosine calculation by using fixed size array
//  * - execution time ~50us (Arduino UNO)
//  *
//  * @param a angle in between 0 and 2PI
//  */
// float _cos(float a);
//
// /**
//  * normalizing radian angle to [0,2PI]
//  * @param angle - angle to be normalized
//  */
// float _normalizeAngle(float angle);
//
// static unsigned long _micros();
//
// /**
//  * Electrical angle calculation
//  *
//  * @param shaft_angle - shaft angle of the motor
//  * @param pole_pairs - number of pole pairs
//  */
//
// /**
//  *  FOC modulation type
//  */
// // enum FOCModulationType{
// //   SinePWM, //!< Sinusoidal PWM modulation
// //   SpaceVectorPWM, //!< Space vector modulation method
// //   Trapezoid_120,
// //   Trapezoid_150
// // };
//
// #define SinePWM 0        //!< Sinusoidal PWM modulation
// #define SpaceVectorPWM 1 //!< Space vector modulation method
// #define Trapezoid_120 2
// #define Trapezoid_150 3
// ==================== 常量定义 ====================
#define PI 3.14159265359f
#define TWO_PI 6.28318530718f
#define SQRT3 1.73205080757f
#define SQRT3_2 0.86602540378f
#define SQRT2 1.41421356237f

// PWM相关参数
#define PWM_FREQUENCY 20000 // 20kHz PWM频率
#define PWM_PERIOD 4200     // 84MHz / 20kHz
#define DEAD_TIME 100       // 死区时间

// 电机参数
#define MOTOR_POLE_PAIRS 7      // 极对数
#define MOTOR_RESISTANCE 0.5f   // 相电阻 (欧姆)
#define MOTOR_INDUCTANCE 0.001f // 相电感 (亨利)
// Clarke变换结构
typedef struct {
  float alpha;
  float beta;
} ClarkeTransform_t;

// Park变换结构
typedef struct {
  float d;
  float q;
} ParkTransform_t;
// PID控制器结构
typedef struct {
  float kp;
  float ki;
  float kd;
  float integral;
  float prev_error;
  float output_limit;
  float integral_limit;
} PIDController_t;
// FOC控制器主结构
typedef struct {
  // 硬件接口
  TIM_HandleTypeDef *htim_pwm;     // PWM定时器
  TIM_HandleTypeDef *htim_encoder; // 编码器定时器
  // ADC_HandleTypeDef* hadc;         // ADC用于电流采样

  // 电机状态
  float angle_elec;      // 电角度
  float angle_mech;      // 机械角度
  float velocity;        // 转速 (rad/s)
  float target_velocity; // 目标转速
  float target_torque;   // 目标转矩

  // 电流测量
  float current_a; // A相电流
  float current_b; // B相电流
  float current_c; // C相电流

  // 变换后的值
  ClarkeTransform_t clarke;
  ParkTransform_t park;

  // 电压输出
  float v_d;     // d轴电压
  float v_q;     // q轴电压
  float v_alpha; // alpha轴电压
  float v_beta;  // beta轴电压

  // PID控制器
  PIDController_t pid_velocity; // 速度环PID
  PIDController_t pid_id;       // d轴电流环PID
  PIDController_t pid_iq;       // q轴电流环PID

  // SVPWM
  // SVPWM_t svpwm;

  // 状态标志
  uint8_t enabled;
  uint8_t fault;
} FOCMotor_t;
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
float FOC_PIDUpdate(PIDController_t *pid, float setpoint, float measurement,
                    float dt);
#endif
