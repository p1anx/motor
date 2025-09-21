#include "as5600.h"
#include "foc_base.h"
#include "open_loop_FOC.h"
#include "stm32f1xx_hal.h"
#include <stdint.h>

#define ANGLE _2PI

extern float Ualpha, Ubeta, Ua, Ub, Uc;

void AS5600_test(void) {
  uint8_t buffer[2], angle_buffer[2];
  int raw_angle, int_angle;
  float angle;
  int addr = AS5600_ADDRESS_RD;
  static int last_time = 0;
  int now_time = HAL_GetTick();
  int run_time = now_time - last_time;
  last_time = now_time;
  HAL_I2C_Mem_Read(&hi2c2, AS5600_ADDRESS_RD, 0x0c, I2C_MEMADD_SIZE_8BIT,
                   buffer, 2, HAL_MAX_DELAY);
  HAL_I2C_Mem_Read(&hi2c2, AS5600_ADDRESS_RD, 0x0e, I2C_MEMADD_SIZE_8BIT,
                   angle_buffer, 2, HAL_MAX_DELAY);
  raw_angle = buffer[0] << 8 | buffer[1];
  angle = raw_angle * 360 / 4096;

  int_angle = angle_buffer[0] << 8 | angle_buffer[1];

  printf("i2c addr = %x\n", addr);
  printf("as56000 value = %d\n", raw_angle);
  printf("as56000 angle = %.2f\n", angle);
  printf("as56000 angle angle = %d\n", int_angle);
  printf("run time = %d\n", run_time);
  HAL_Delay(1000);
}

static float LowPassFilter(float x) {
  float Tf = 0.005f;
  static float last_time = 0;
  float last_y;
  unsigned long now_time = _micros();
  float dt = (now_time - last_time) * 1e-3;

  if (dt < 0.0f) {
    dt = 1e-3f;
  } else if (dt > 0.3f) {
    last_y = x;
    last_time = now_time;
    return x;
  }

  float alpha = Tf / (Tf + dt);
  float y = alpha * last_y + (1.0f - alpha) * x;
  last_y = y;
  last_time = now_time;
  return y;
}
float getAngle(void) {
  uint8_t as5600_readBuffer[2];
  int angle_buffer;
  float angle;
  HAL_I2C_Mem_Read(&hi2c2, AS5600_ADDRESS_RD, 0x0c, I2C_MEMADD_SIZE_8BIT,
                   as5600_readBuffer, 2, HAL_MAX_DELAY);
  angle_buffer = as5600_readBuffer[0] << 8 | as5600_readBuffer[1];
  angle = (float)angle_buffer * ANGLE / 4096;
  return angle;
}

int getCircle(void) {
  static int circle = 0;
  static float last_angle = 0;
  float now_angle = getAngle();
  float abs_delta_angle;

  float delta_angle = now_angle - last_angle;
  abs_delta_angle = delta_angle > 0 ? delta_angle : -delta_angle;

  if (abs_delta_angle > 0.8f * _2PI) {
    circle += (delta_angle > 0) ? 1 : -1;
  }
  printf("now angle  = %f\n", now_angle);
  printf("last angle = %f\n", last_angle);
  printf("delta angle = %f\n", delta_angle);
  printf("abs angle = %f\n", abs_delta_angle);
  last_angle = now_angle;
  return circle;
}

float getVelocity(void) {
  static int last_us = 0;
  float delta_t;

  static int circle = 0;
  static int last_circle = 0;
  static float last_angle = 0;
  float now_angle = getAngle();
  float abs_delta_angle;
  float delta_angle = now_angle - last_angle;
  abs_delta_angle = delta_angle > 0 ? delta_angle : -delta_angle;

  if (abs_delta_angle > 0.8f * _2PI) {
    circle += (delta_angle > 0) ? 1 : -1;
  }
  unsigned long now_us = _micros();
  delta_t = (now_us - last_us) * 1e-3; // ms to s

  float velocity =
      ((circle - last_circle) * _2PI + (now_angle - last_angle)) / delta_t;

  // printf("last circle = %d\n", last_circle);
  // printf("now  circle = %d\n", circle);
  // printf("last angle  = %f\n", last_angle);
  // printf("now  angle  = %f\n", now_angle);
  // printf("now  vel    = %f\n", velocity);
  // printf("delta t     = %f\n", delta_t);
  last_angle = now_angle;
  last_circle = circle;
  last_us = now_us;
  // velocity = LowPassFilter(velocity);
  return velocity;
}

typedef struct {
  float Kp;
  float Ki;
  float Kd;
  float integral;
  float integral_min;
  float integral_max;
  float prev_error;
  float output;
  float output_max;
  float output_min;

} PID_Controller;

float PID_Update(PID_Controller *pid, float setpoint, float measurement) {
  float error = setpoint - measurement;

  // 比例项
  float proportional = pid->Kp * error;

  // 积分项(带抗饱和)
  pid->integral += error;
  if (pid->integral > pid->integral_max)
    pid->integral = pid->integral_max;
  else if (pid->integral < pid->integral_min)
    pid->integral = pid->integral_min;
  float integral = pid->Ki * pid->integral;

  // 微分项
  float derivative = pid->Kd * (error - pid->prev_error);
  pid->prev_error = error;

  // 计算总输出
  pid->output = proportional + integral + derivative;

  // 输出限幅
  if (pid->output > pid->output_max)
    pid->output = pid->output_max;
  else if (pid->output < pid->output_min)
    pid->output = pid->output_min;

  static int time_count = 0;
  if (time_count == 100) {
    time_count = 0;
    printf("%.2f, %.2f\n", setpoint, measurement);
  }
  time_count++;

  return pid->output;
}

// void setTorque(float Uq, float angle_el) {
//   Uq = _constrain(Uq, -(voltage_power_supply) / 2, (voltage_power_supply) /
//   2); float Ud = 0; angle_el = _normalizeAngle(angle_el);
//   // 帕克逆变换
//   Ualpha = -Uq * sin(angle_el);
//   Ubeta = Uq * cos(angle_el);
//
//   // 克拉克逆变换
//   Ua = Ualpha + voltage_power_supply / 2;
//   Ub = (sqrt(3) * Ubeta - Ualpha) / 2 + voltage_power_supply / 2;
//   Uc = (-Ualpha - sqrt(3) * Ubeta) / 2 + voltage_power_supply / 2;
//   setPwm(Ua, Ub, Uc);
// }

// void test_velocity(void) {
//   float vel = getVelocity();
//   printf("vel = %f\n", vel);
// }
