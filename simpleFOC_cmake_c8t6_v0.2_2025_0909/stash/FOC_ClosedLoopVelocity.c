#include "open_loop_FOC.h"
#include "as5600.h"
#include "math.h"

#define PP 7
#define DIR 1
int pwmA, pwmB, pwmC;
int pole_pairs = 7;
float voltage_limit = 10;
float voltage_power_supply = 12.0;
float velocity_limit = 5;
float shaft_angle = 0, open_loop_timestamp = 0;
float zero_electric_angle = 0, Ualpha, Ubeta = 0, Ua = 0, Ub = 0, Uc = 0,
      dc_a = 0, dc_b = 0, dc_c = 0;

int foc_modulation = SinePWM;

static unsigned long _micros() {
  return HAL_GetTick(); // get microseconds
}

static void _writeDutyCycle3PWM(float dc_a, float dc_b, float dc_c, int pinA,
                                int pinB, int pinC) {
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
// int array instead of float array
// 2x storage save (int 2Byte float 4 Byte )
// sin*10000
int sine_array[200] = {
    0,    79,   158,  237,  316,  395,  473,   552,  631,  710,  789,  867,
    946,  1024, 1103, 1181, 1260, 1338, 1416,  1494, 1572, 1650, 1728, 1806,
    1883, 1961, 2038, 2115, 2192, 2269, 2346,  2423, 2499, 2575, 2652, 2728,
    2804, 2879, 2955, 3030, 3105, 3180, 3255,  3329, 3404, 3478, 3552, 3625,
    3699, 3772, 3845, 3918, 3990, 4063, 4135,  4206, 4278, 4349, 4420, 4491,
    4561, 4631, 4701, 4770, 4840, 4909, 4977,  5046, 5113, 5181, 5249, 5316,
    5382, 5449, 5515, 5580, 5646, 5711, 5775,  5839, 5903, 5967, 6030, 6093,
    6155, 6217, 6279, 6340, 6401, 6461, 6521,  6581, 6640, 6699, 6758, 6815,
    6873, 6930, 6987, 7043, 7099, 7154, 7209,  7264, 7318, 7371, 7424, 7477,
    7529, 7581, 7632, 7683, 7733, 7783, 7832,  7881, 7930, 7977, 8025, 8072,
    8118, 8164, 8209, 8254, 8298, 8342, 8385,  8428, 8470, 8512, 8553, 8594,
    8634, 8673, 8712, 8751, 8789, 8826, 8863,  8899, 8935, 8970, 9005, 9039,
    9072, 9105, 9138, 9169, 9201, 9231, 9261,  9291, 9320, 9348, 9376, 9403,
    9429, 9455, 9481, 9506, 9530, 9554, 9577,  9599, 9621, 9642, 9663, 9683,
    9702, 9721, 9739, 9757, 9774, 9790, 9806,  9821, 9836, 9850, 9863, 9876,
    9888, 9899, 9910, 9920, 9930, 9939, 9947,  9955, 9962, 9969, 9975, 9980,
    9985, 9989, 9992, 9995, 9997, 9999, 10000, 10000};

// function approximating the sine calculation by using fixed size array
// ~40us (float array)
// ~50us (int array)
// precision +-0.005
// it has to receive an angle in between 0 and 2PI
float _sin(float a) {
  if (a < _PI_2) {
    // return sine_array[(int)(199.0*( a / (_PI/2.0)))];
    // return sine_array[(int)(126.6873* a)];           // float array optimized
    return 0.0001 * sine_array[_round(126.6873 * a)]; // int array optimized
  } else if (a < _PI) {
    // return sine_array[(int)(199.0*(1.0 - (a-_PI/2.0) / (_PI/2.0)))];
    // return sine_array[398 - (int)(126.6873*a)];          // float array
    // optimized
    return 0.0001 *
           sine_array[398 - _round(126.6873 * a)]; // int array optimized
  } else if (a < _3PI_2) {
    // return -sine_array[(int)(199.0*((a - _PI) / (_PI/2.0)))];
    // return -sine_array[-398 + (int)(126.6873*a)];           // float array
    // optimized
    return -0.0001 *
           sine_array[-398 + _round(126.6873 * a)]; // int array optimized
  } else {
    // return -sine_array[(int)(199.0*(1.0 - (a - 3*_PI/2) / (_PI/2.0)))];
    // return -sine_array[796 - (int)(126.6873*a)];           // float array
    // optimized
    return -0.0001 *
           sine_array[796 - _round(126.6873 * a)]; // int array optimized
  }
}

// function approximating cosine calculation by using fixed size array
// ~55us (float array)
// ~56us (int array)
// precision +-0.005
// it has to receive an angle in between 0 and 2PI
float _cos(float a) {
  float a_sin = a + _PI_2;
  a_sin = a_sin > _2PI ? a_sin - _2PI : a_sin;
  return _sin(a_sin);
}

// normalizing radian angle to [0,2PI]
float _normalizeAngle(float angle) {
  float a = fmod(angle, _2PI);
  return a >= 0 ? a : (a + _2PI);
}
// float _electricalAngle() {
//   return _normalizeAngle((float)(DIR * PP) * getMechanicalAngle(&S0) -
//                          zero_electric_angle);
// }
// Electrical angle calculation
float _electricalAngle(float shaft_angle, int pole_pairs) {
  // shaft_angle = getAngle();
  // return _normalizeAngle(shaft_angle * pole_pairs);
  return (shaft_angle * pole_pairs);
}
// Set voltage to the pwm pin
void setPwm(float Ua, float Ub, float Uc) {

  // limit the voltage in driver
  // printf("before ua = %f, ub = %f, uc = %f\n", Ua, Ub, Uc);
  Ua = _constrain(Ua, 0.0, voltage_limit);
  Ub = _constrain(Ub, 0.0, voltage_limit);
  Uc = _constrain(Uc, 0.0, voltage_limit);
  // calculate duty cycle
  // limited in [0,1]
  float dc_a = _constrain(Ua / voltage_power_supply, 0.0, 1.0);
  float dc_b = _constrain(Ub / voltage_power_supply, 0.0, 1.0);
  float dc_c = _constrain(Uc / voltage_power_supply, 0.0, 1.0);
  // printf("ua = %f, ub = %f, uc = %f\n", Ua, Ub, Uc);
  // printf("dc_a = %f, dc_b = %f, dc_c = %f\n", dc_a, dc_b, dc_c);

  // hardware specific writing
  // hardware specific function - depending on driver and mcu
  _writeDutyCycle3PWM(dc_a, dc_b, dc_c, pwmA, pwmB, pwmC);
}
// Method using FOC to set Uq and Ud to the motor at the optimal angle
// Function implementing Space Vector PWM and Sine PWM algorithms
//
// Function using sine approximation
// regular sin + cos ~300us    (no memory usaage)
// approx  _sin + _cos ~110us  (400Byte ~ 20% of memory)
void setPhaseVoltage(float Uq, float Ud, float angle_el) {

  const int centered = 1;
  int sector;
  float _ca, _sa;

  switch (foc_modulation) {
  case Trapezoid_120:
    // see https://www.youtube.com/watch?v=InzXA7mWBWE Slide 5
    static int trap_120_map[6][3] = {
        {0, 1, -1}, {-1, 1, 0}, {-1, 0, 1}, {0, -1, 1},
        {1, -1, 0}, {1, 0, -1} // each is 60 degrees with values for 3 phases of
                               // 1=positive -1=negative 0=high-z
    };
    // static int trap_120_state = 0;
    sector = 6 * (_normalizeAngle(angle_el + _PI / 6.0 + zero_electric_angle) /
                  _2PI); // adding PI/6 to align with other modes

    Ua = Uq + trap_120_map[sector][0] * Uq;
    Ub = Uq + trap_120_map[sector][1] * Uq;
    Uc = Uq + trap_120_map[sector][2] * Uq;

    if (centered) {
      Ua += (voltage_limit) / 2 - Uq;
      Ub += (voltage_limit) / 2 - Uq;
      Uc += (voltage_limit) / 2 - Uq;
    }
    break;

  case Trapezoid_150:
    // see https://www.youtube.com/watch?v=InzXA7mWBWE Slide 8
    static int trap_150_map[12][3] = {
        {0, 1, -1},  {-1, 1, -1}, {-1, 1, 0}, {-1, 1, 1}, {-1, 0, 1},
        {-1, -1, 1}, {0, -1, 1},  {1, -1, 1}, {1, -1, 0}, {1, -1, -1},
        {1, 0, -1},  {1, 1, -1} // each is 30 degrees with values for 3 phases
                                // of 1=positive -1=negative 0=high-z
    };
    // static int trap_150_state = 0;
    sector = 12 * (_normalizeAngle(angle_el + _PI / 6.0 + zero_electric_angle) /
                   _2PI); // adding PI/6 to align with other modes

    Ua = Uq + trap_150_map[sector][0] * Uq;
    Ub = Uq + trap_150_map[sector][1] * Uq;
    Uc = Uq + trap_150_map[sector][2] * Uq;

    // center
    if (centered) {
      Ua += (voltage_limit) / 2 - Uq;
      Ub += (voltage_limit) / 2 - Uq;
      Uc += (voltage_limit) / 2 - Uq;
    }

    break;

  case SinePWM:
    // Sinusoidal PWM modulation
    // Inverse Park + Clarke transformation

    // angle normalization in between 0 and 2pi
    // only necessary if using _sin and _cos - approximation functions
    angle_el = _normalizeAngle(angle_el + zero_electric_angle);
    _ca = _cos(angle_el);
    _sa = _sin(angle_el);
    // Inverse park transform
    Ualpha = _ca * Ud - _sa * Uq; // -sin(angle) * Uq;
    Ubeta = _sa * Ud + _ca * Uq;  //  cos(angle) * Uq;

    // Clarke transform
    Ua = Ualpha + voltage_limit / 2;
    Ub = -0.5 * Ualpha + _SQRT3_2 * Ubeta + voltage_limit / 2;
    Uc = -0.5 * Ualpha - _SQRT3_2 * Ubeta + voltage_limit / 2;

    if (!centered) {
      float Umin = fmin(Ua, fmin(Ub, Uc));
      Ua -= Umin;
      Ub -= Umin;
      Uc -= Umin;
    }

    break;

  case SpaceVectorPWM:
    // Nice video explaining the SpaceVectorModulation (SVPWM) algorithm
    // https://www.youtube.com/watch?v=QMSWUMEAejg

    // if negative voltages change inverse the phase
    // angle + 180degrees
    if (Uq < 0)
      angle_el += _PI;
    Uq = abs(Uq);

    // angle normalisation in between 0 and 2pi
    // only necessary if using _sin and _cos - approximation functions
    angle_el = _normalizeAngle(angle_el + zero_electric_angle + _PI_2);

    // find the sector we are in currently
    sector = floor(angle_el / _PI_3) + 1;
    // calculate the duty cycles
    float T1 = _SQRT3 * _sin(sector * _PI_3 - angle_el) * Uq / voltage_limit;
    float T2 =
        _SQRT3 * _sin(angle_el - (sector - 1.0) * _PI_3) * Uq / voltage_limit;
    // two versions possible
    float T0 = 0; // pulled to 0 - better for low power supply voltage
    if (centered) {
      T0 = 1 - T1 - T2; // centered around driver->voltage_limit/2
    }

    // calculate the duty cycles(times)
    float Ta, Tb, Tc;
    switch (sector) {
    case 1:
      Ta = T1 + T2 + T0 / 2;
      Tb = T2 + T0 / 2;
      Tc = T0 / 2;
      break;
    case 2:
      Ta = T1 + T0 / 2;
      Tb = T1 + T2 + T0 / 2;
      Tc = T0 / 2;
      break;
    case 3:
      Ta = T0 / 2;
      Tb = T1 + T2 + T0 / 2;
      Tc = T2 + T0 / 2;
      break;
    case 4:
      Ta = T0 / 2;
      Tb = T1 + T0 / 2;
      Tc = T1 + T2 + T0 / 2;
      break;
    case 5:
      Ta = T2 + T0 / 2;
      Tb = T0 / 2;
      Tc = T1 + T2 + T0 / 2;
      break;
    case 6:
      Ta = T1 + T2 + T0 / 2;
      Tb = T0 / 2;
      Tc = T1 + T0 / 2;
      break;
    default:
      // possible error state
      Ta = 0;
      Tb = 0;
      Tc = 0;
    }

    // calculate the phase voltages and center
    Ua = Ta * voltage_limit;
    Ub = Tb * voltage_limit;
    Uc = Tc * voltage_limit;
    break;
  }

  // set the voltages in driver
  setPwm(Ua, Ub, Uc);
}
void setPhaseVoltage_0(float Uq, float Ud, float angle_el) {
  angle_el = _normalizeAngle(angle_el + zero_electric_angle);
  // 帕克逆变换
  Ualpha = -Uq * _sin(angle_el);
  Ubeta = Uq * _cos(angle_el);

  // 克拉克逆变换
  Ua = Ualpha + voltage_power_supply / 2;
  Ub = _SQRT3_2 * Ubeta - Ualpha * 0.5 + voltage_power_supply / 2;
  Uc = _SQRT3_2 * Ubeta + Ualpha * 0.5 + voltage_power_supply / 2;
  printf("set phase v : ua = %f, ub = %f, uc = %f\n", Ua, Ub, Uc);
  setPwm(Ua, Ub, Uc);
}
void angleOpenloop(float target_angle) {
  // get current timestamp
  unsigned long now_ms = _micros();
  // calculate the sample time from last call
  float Ts = (now_ms - open_loop_timestamp) * 1e-3;

  // calculate the necessary angle to move from current position towards target
  // angle with maximal velocity (velocity_limit)
  if (abs(target_angle - shaft_angle) > abs(velocity_limit * Ts))
    shaft_angle += _sign(target_angle - shaft_angle) * abs(velocity_limit) * Ts;
  else
    shaft_angle = target_angle;

  // set the maximal allowed voltage (voltage_limit) with the necessary angle
  setPhaseVoltage_0(voltage_limit, 0,
                    _electricalAngle(shaft_angle, pole_pairs));

  // save timestamp for next call
  open_loop_timestamp = now_ms;
}

void setTorque(float Uq, float angle_el) {
  angle_el = _normalizeAngle(angle_el + zero_electric_angle);
  // 帕克逆变换
  Ualpha = -Uq * _sin(angle_el);
  Ubeta = Uq * _cos(angle_el);

  // 克拉克逆变换
  Ua = Ualpha + voltage_power_supply / 2;
  Ub = _SQRT3_2 * Ubeta - Ualpha * 0.5 + voltage_power_supply / 2;
  Uc = _SQRT3_2 * Ubeta + Ualpha * 0.5 + voltage_power_supply / 2;
  // printf("set phase v : ua = %f, ub = %f, uc = %f\n", Ua, Ub, Uc);
  setPwm(Ua, Ub, Uc);
}

// 开环速度函数
float velocityOpenloop(float target_velocity) {
  unsigned long now_us =
      _micros(); // 获取从开启芯片以来的微秒数，它的精度是 4 微秒。 micros()
                 // 返回的是一个无符号长整型（unsigned long）的值

  // 计算当前每个Loop的运行时间间隔
  float Ts = (now_us - open_loop_timestamp) * 1e-3f;
  printf("Ts = %f\n", Ts);

  // 由于 micros() 函数返回的时间戳会在大约 70
  // 分钟之后重新开始计数，在由70分钟跳变到0时，TS会出现异常，因此需要进行修正。如果时间间隔小于等于零或大于
  // 0.5 秒，则将其设置为一个较小的默认值，即 1e-3f
  if (Ts <= 0 || Ts > 0.5f)
    Ts = 1e-3f;

  printf("before shaft_angle = %f\n", shaft_angle);
  // 通过乘以时间间隔和目标速度来计算需要转动的机械角度，存储在 shaft_angle
  // 变量中。在此之前，还需要对轴角度进行归一化，以确保其值在 0 到 2π 之间。
  shaft_angle = _normalizeAngle(shaft_angle + target_velocity * Ts);
  printf("shaft_angle = %f\n", shaft_angle);
  // 以目标速度为 10 rad/s 为例，如果时间间隔是 1 秒，则在每个循环中需要增加 10
  // * 1 = 10 弧度的角度变化量，才能使电机转动到目标速度。 如果时间间隔是 0.1
  // 秒，那么在每个循环中需要增加的角度变化量就是 10 * 0.1 = 1
  // 弧度，才能实现相同的目标速度。因此，电机轴的转动角度取决于目标速度和时间间隔的乘积。

  // 使用早前设置的voltage_power_supply的1/3作为Uq值，这个值会直接影响输出力矩
  // 最大只能设置为Uq = voltage_power_supply/2，否则ua,ub,uc会超出供电电压限幅
  float Uq = voltage_power_supply / 3;

  // setPhaseVoltage(Uq, 0, _electricalAngle(shaft_angle, pole_pairs));
  setTorque(Uq, _electricalAngle(shaft_angle, pole_pairs));

  open_loop_timestamp = now_us; // 用于计算下一个时间间隔

  return Uq;
}
typedef struct {
  float P, I, D;
  float output_ramp;
  float limit;
  float integral_prev;
  float error_prev;
  float output_prev;
  unsigned long timestamp_prev;
} PIDController;

// PID控制器函数
void PIDController_init(PIDController *pid, float P, float I, float D,
                        float ramp, float limit) {
  pid->P = P;
  pid->I = I;
  pid->D = D;
  pid->output_ramp = ramp;
  pid->limit = limit;
  pid->integral_prev = 0;
  pid->error_prev = 0;
  pid->output_prev = 0;
  pid->timestamp_prev = 0;
}

float FOC_PIDUpdate(PIDController_t *pid, float setpoint, float measurement,
                    float dt) {
  float error = setpoint - measurement;

  // 比例项
  float p_term = pid->kp * error;

  // 积分项
  pid->integral += error * dt;
  if (pid->integral > pid->integral_limit) {
    pid->integral = pid->integral_limit;
  } else if (pid->integral < -pid->integral_limit) {
    pid->integral = -pid->integral_limit;
  }
  float i_term = pid->ki * pid->integral;

  // 微分项
  float d_term = pid->kd * (error - pid->prev_error) / dt;
  pid->prev_error = error;

  // 总输出
  float output = p_term + i_term + d_term;

  // 输出限幅
  if (output > pid->output_limit) {
    output = pid->output_limit;
  } else if (output < -pid->output_limit) {
    output = -pid->output_limit;
  }

  return output;
}
// float PIDController_calculate(PIDController *pid, float error) {
//   unsigned long timestamp = _micros(); // 需要替换为实际时间获取函数
//   float dt = (timestamp - pid->timestamp_prev) / 1000.0f;
//
//   if (dt <= 0.0f)
//     dt = 1e-3f;
//
//   // 比例项
//   float proportional = pid->P * error;
//
//   // 积分项
//   pid->integral_prev += pid->I * error;
//   pid->integral_prev = _constrain(pid->integral_prev, -pid->limit,
//   pid->limit);
//
//   // 微分项
//   float derivative = pid->D * (error - pid->error_prev);
//
//   // 总输出
//   float output = proportional + pid->integral_prev + derivative;
//
//   // 输出限制
//   output = _constrain(output, -pid->limit, pid->limit);
//
//   // 输出变化率限制
//   float output_rate = (output - pid->output_prev);
//   if (output_rate > pid->output_ramp) {
//     output = pid->output_prev + pid->output_ramp;
//   } else if (output_rate < -pid->output_ramp) {
//     output = pid->output_prev - pid->output_ramp;
//   }
//
//   pid->error_prev = error;
//   pid->output_prev = output;
//   pid->timestamp_prev = timestamp;
//   printf("output = %f\n", output);
//   printf("error  = %f\n", error);
//   printf("proportional = %f\n", proportional);
//   printf("pid integer = %f\n", pid->integral_prev);
//
//   return output;
// }
// float getMechanicalAngle(void) { return getAngle(); }

// float _electricalAngle(void) {
//   return _normalizeAngle((float)(DIR * PP) * getMechanicalAngle() -
//                          zero_electric_angle);
// }
PIDController vel_loop_M0;
// PID设置函数
void DFOC_M0_SET_VEL_PID(float P, float I, float D, float ramp) {
  vel_loop_M0.P = P;
  vel_loop_M0.I = I;
  vel_loop_M0.D = D;
  vel_loop_M0.output_ramp = ramp;
  vel_loop_M0.limit = 100;
}
// PID接口函数
float DFOC_M0_VEL_PID(float error) {
  return PIDController_calculate(&vel_loop_M0, error);
}
// float DFOC_M0_ANGLE_PID(float error) {
//   return PIDController_calculate(&angle_loop_M0, error);
// }
// void DFOC_M0_set_Velocity_Angle(float Target) {
//   setTorque(
//       DFOC_M0_VEL_PID(DFOC_M0_ANGLE_PID((Target - getAngle()) * 180.0f /
//       _PI)), _electricalAngle(shaft_angle, pole_pairs));
// }
void FOC_alignSensor(int _PP, int _DIR) {
  // PP = _PP;
  // DIR = _DIR;
  setTorque(3, _3PI_2);

  // delay(1000); // 需要实现延时函数

  zero_electric_angle = _electricalAngle(shaft_angle, pole_pairs);
  setTorque(0, _3PI_2);

  printf("0 eAngle = %.4f\n", zero_electric_angle);
}
void DFOC_M0_setVelocity(float Target) {
  float vel = getVelocity();
  printf("vel = %f\n", vel);
  setTorque(DFOC_M0_VEL_PID((Target - vel)),
            _electricalAngle(shaft_angle, pole_pairs));
}
// ==================== 坐标变换 ====================

void FOC_Clarke(float a, float b, float c, ClarkeTransform_t *clarke) {
  clarke->alpha = a;
  clarke->beta = (a + 2.0f * b) / SQRT3;
}

void FOC_InverseClarke(float alpha, float beta, float *a, float *b, float *c) {
  *a = alpha;
  *b = -0.5f * alpha + SQRT3_2 * beta;
  *c = -0.5f * alpha - SQRT3_2 * beta;
}

void FOC_Park(float alpha, float beta, float theta, ParkTransform_t *park) {
  float cos_theta = _cos(theta);
  float sin_theta = _sin(theta);

  park->d = alpha * cos_theta + beta * sin_theta;
  park->q = -alpha * sin_theta + beta * cos_theta;
}

void FOC_InversePark(float d, float q, float theta, float *alpha, float *beta) {
  float cos_theta = _cos(theta);
  float sin_theta = _sin(theta);

  *alpha = d * cos_theta - q * sin_theta;
  *beta = d * sin_theta + q * cos_theta;
}

void FOC_InitPID(PIDController_t *pid, float kp, float ki, float kd,
                 float out_limit, float int_limit) {
  pid->kp = kp;
  pid->ki = ki;
  pid->kd = kd;
  pid->integral = 0;
  pid->prev_error = 0;
  pid->output_limit = out_limit;
  pid->integral_limit = int_limit;
}

void FOC_CurrentControl(FOCMotor_t *motor) {
  // Clarke变换
  FOC_Clarke(motor->current_a, motor->current_b, motor->current_c,
             &motor->clarke);

  // Park变换
  FOC_Park(motor->clarke.alpha, motor->clarke.beta, motor->angle_elec,
           &motor->park);

  // 电流环PID控制
  float target_id = 0;                    // d轴电流设定为0（最大转矩控制）
  float target_iq = motor->target_torque; // q轴电流控制转矩

  motor->v_d =
      FOC_PIDUpdate(&motor->pid_id, target_id, motor->park.d, 0.00005f);
  motor->v_q =
      FOC_PIDUpdate(&motor->pid_iq, target_iq, motor->park.q, 0.00005f);
}

void FOC_VelocityControl(FOCMotor_t *motor) {
  // 速度环PID控制
  motor->target_torque = FOC_PIDUpdate(
      &motor->pid_velocity, motor->target_velocity, motor->velocity, 0.001f);

  // 执行电流控制
  FOC_CurrentControl(motor);
}

void FOC_SetVelocity(FOCMotor_t *motor, float velocity) {
  motor->target_velocity = velocity;
}

void FOC_SetTorque(FOCMotor_t *motor, float torque) {
  motor->target_torque = torque;
}
