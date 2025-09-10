#include "open_loop_FOC.h"
#include "as5600.h"
#include "math.h"

#define PP 7
#define DIR 1
extern int pwmA, pwmB, pwmC;
extern int pole_pairs;
extern float voltage_limit;
extern float voltage_power_supply;
extern float velocity_limit;
extern float shaft_angle, open_loop_timestamp;
extern float zero_electric_angle, Ualpha, Ubeta, Ua, Ub, Uc, dc_a, dc_b, dc_c;

// int foc_modulation = SinePWM;
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
  setPhaseVoltage(voltage_limit, 0, _electricalAngle(shaft_angle, pole_pairs));

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

// void FOC_alignSensor(int _PP, int _DIR) {
//   // PP = _PP;
//   // DIR = _DIR;
//   setTorque(3, _3PI_2);
//
//   // delay(1000); // 需要实现延时函数
//
//   zero_electric_angle = _electricalAngle(shaft_angle, pole_pairs);
//   setTorque(0, _3PI_2);
//
//   printf("0 eAngle = %.4f\n", zero_electric_angle);
// }

// void DFOC_M0_setVelocity(float Target) {
//   float vel = getVelocity();
//   printf("vel = %f\n", vel);
//   setTorque(DFOC_M0_VEL_PID((Target - vel)),
//             _electricalAngle(shaft_angle, pole_pairs));
// }
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
