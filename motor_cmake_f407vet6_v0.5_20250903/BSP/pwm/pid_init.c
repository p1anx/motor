#include "pid_init.h"
#include "motor_config.h"
#include "stm32f4xx_hal_tim.h"
#include "sys/types.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PWM_SCALE_FACTOR 2
float g_target_position = 10000.0f;
// PID_Controller pid_pos = {
//     .Kp = 0.01f,
//     .Ki = 0.00f,
//     .Kd = 0.00f,
//     .output_max = 500.0f,
//     .output_min = -500.0f,
//     .integral_max = 100.0f,
//     .integral_min = -100.0f,
//     .target_position = 1000
// };

// PID_Controller pid_vel_init = {
//     .Kp = 1.0f,
//     .Ki = 0.05f,
//     .Kd = 0.00f,
//     .output_max = 500.0f,
//     .output_min = -500.0f,
//     .integral_max = 100.0f,
//     .integral_min = -100.0f,
//     .target_velocity = 2
//     // .target_velocity = -2
// };
void MG513_Init(Motor_InitStruct *motor) {
  int direction = 1;
  int speed = 100;

  HAL_TIM_PWM_Start(motor->pwm_tim, motor->pwm_channel);
  __HAL_TIM_SET_PRESCALER(motor->pwm_tim, PWM_TIM_PSC - 1);
  __HAL_TIM_SET_AUTORELOAD(motor->pwm_tim, PWM_RESOLUTION - 1);
  // HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
  HAL_TIM_Encoder_Start(motor->encoder_tim, TIM_CHANNEL_ALL);
  HAL_TIM_Base_Start_IT(motor->encoder_tim);

  __HAL_TIM_ENABLE_IT(motor->encoder_tim, TIM_IT_UPDATE);
  __HAL_TIM_SetAutoreload(motor->encoder_tim, ENCODER_COUNT_PERIOD);
  __HAL_TIM_SET_COUNTER(motor->encoder_tim, 0);

  HAL_TIM_Base_Start_IT(motor->pid_update_tim);
  __HAL_TIM_SetAutoreload(motor->pid_update_tim, PID_TIM_PERIOD - 1);

  // TB6612_ENABLE();
  // tb6612_pwm_setup(TB6612_PWM1_PERIOD, speed);
  // tb6612_direction(direction);
  // tb6612_setDirection(motor, direction);
}

// int g_overflow_encoder_count = 0;

int encoder_overflow_cnt_fun(Motor_InitStruct *motor) {
  if (__HAL_TIM_IS_TIM_COUNTING_DOWN(motor->encoder_tim)) {
    motor->pulse_count++;
    // g_overflow_encoder_count++;
  } else {
    motor->pulse_count--;
    // g_overflow_encoder_count--;
  }
#ifdef DEBUG_PRINT
  // printf("overflow = %d\n", motor->pulse_count);
  // my_printf("overflow = %d\n", g_overflow_encoder_count);
#endif
  return 0;
}

#define TEST 1

#if METHOD0_IsAvailable
#if TEST
float read_motor_pulses(Motor_InitStruct *motor) {
  int count;
  //(float) is important for solution of negative value
  if (__HAL_TIM_IS_TIM_COUNTING_DOWN(motor->encoder_tim)) {
    // count = (float)(0xffff-__HAL_TIM_GET_COUNTER(motor->encoder_tim)) +
    // g_overflow_encoder_count*(ENCODER_COUNT_PERIOD);
    count = (float)(0xffff - __HAL_TIM_GET_COUNTER(motor->encoder_tim)) +
            (motor->pulse_count) * (ENCODER_COUNT_PERIOD);
    // count = (float)(__HAL_TIM_GET_COUNTER(motor->encoder_tim)) +
    // (motor->pulse_count) * (ENCODER_COUNT_PERIOD); count =
    // (float)(-__HAL_TIM_GET_COUNTER(motor->encoder_tim)) +
    // motor->pulse_count*(ENCODER_COUNT_PERIOD);
  } else {
    // count =  -(float)__HAL_TIM_GET_COUNTER(motor->encoder_tim) +
    // (g_overflow_encoder_count+1)*(ENCODER_COUNT_PERIOD);//(g_overflow_encoder_count+1)补偿启动时，由于--的起始值为-1,加1做补偿
    count =
        -(float)__HAL_TIM_GET_COUNTER(motor->encoder_tim) +
        (motor->pulse_count + 1) *
            (ENCODER_COUNT_PERIOD); //(g_overflow_encoder_count+1)补偿启动时，由于--的起始值为-1,加1做补偿
  }
  // my_printf("motor pulse count = %d, motor overflow = %d, count = %.2f\n",
  // __HAL_TIM_GetCounter(&encoder_htim), motor->pulse_count, count);
  return count;
}
#endif

// float read_motor_pulses(Motor_InitStruct* motor){
//   float count;
//   //(float) is important for solution of negative value
//   if(__HAL_TIM_IS_TIM_COUNTING_DOWN(motor->encoder_tim)){
//   // count = (float)(0xffff-__HAL_TIM_GET_COUNTER(motor->encoder_tim)) +
//   g_overflow_encoder_count*(ENCODER_COUNT_PERIOD);
//   // count = (float)(0xffff-__HAL_TIM_GET_COUNTER(motor->encoder_tim)) +
//   (motor->pulse_count) * (ENCODER_COUNT_PERIOD); count =
//   __HAL_TIM_GET_COUNTER(motor->encoder_tim);
//   // count = (float)(-__HAL_TIM_GET_COUNTER(motor->encoder_tim)) +
//   motor->pulse_count*(ENCODER_COUNT_PERIOD);
//   }
//   else{
//   // count =  -(float)__HAL_TIM_GET_COUNTER(motor->encoder_tim) +
//   (g_overflow_encoder_count+1)*(ENCODER_COUNT_PERIOD);//(g_overflow_encoder_count+1)补偿启动时，由于--的起始值为-1,加1做补偿
//   count =  -(float)__HAL_TIM_GET_COUNTER(motor->encoder_tim) +
//   (motor->pulse_count+1)*(ENCODER_COUNT_PERIOD);//(g_overflow_encoder_count+1)补偿启动时，由于--的起始值为-1,加1做补偿

//   }
//   // my_printf("motor pulse count = %d, motor overflow = %d, count = %.2f\n",
//   __HAL_TIM_GetCounter(&encoder_htim), motor->pulse_count, count); return
//   count;
// }
float read_motor_position(Motor_InitStruct *motor) {
  return read_motor_pulses(motor);
}

float read_motor_speed(Motor_InitStruct *motor) {
  static float motor_totalCount, motor_lastCount = 0, motor_speed, delta_count;
  motor_totalCount = read_motor_pulses(motor);
  delta_count = motor_totalCount - motor_lastCount;
  motor_speed = (float)((delta_count) / (4 * MOTOR_REDUCTION_RATIO *
                                         ENCODER_RESOLUTION * PID_UPDATE_TIME));
  motor_lastCount = motor_totalCount;
  // my_printf("total count = %.2f, delta = %.2f overflow_cnt = %d\n",
  // motor_totalCount, delta_count, motor->pulse_count);
  return motor_speed;
}
#endif

int read_motor_direction(Motor_InitStruct *motor) {
  return __HAL_TIM_IS_TIM_COUNTING_DOWN(motor->encoder_tim);
}
// 将角度归一化到[-180, 180]范围
float normalizeAngle(float angle) {
  angle = fmod(angle, 360.0f);
  if (angle > 180.0f) {
    angle -= 360.0f;
  } else if (angle < -180.0f) {
    angle += 360.0f;
  }
  return angle;
}

// 计算角度误差（考虑圆周特性）
float angleError(float current, float target) {
  float error = target - current;
  if (error > 180.0f) {
    error -= 360.0f;
  } else if (error < -180.0f) {
    error += 360.0f;
  }
  return error;
}
// 计算实际角度（根据编码器分辨率和机械减速比）
float calculateActualAngle(float encoder_value) {
  // 每转脉冲数 = 编码器分辨率 × 减速比 × 4(四倍频)
  float pulses_per_rev = ENCODER_RESOLUTION * MOTOR_REDUCTION_RATIO * 4.0f;

  // 计算角度（度）
  float angle = (encoder_value / pulses_per_rev) * 360.0f;

  return normalizeAngle(angle);
}
float read_motor_angle(Motor_InitStruct *motor) {

  float encode_value = read_motor_pulses(motor);
  return calculateActualAngle(encode_value);
}

// float anglePID_Update(PID_Controller *pid, float setpoint, float measurement)
// {

// }
float anglePID_Update(PID_Controller *pid, float setpoint, float measurement) {
  // 角度归一化
  measurement = normalizeAngle(measurement);
  setpoint = normalizeAngle(setpoint);

  // 计算角度误差（考虑圆周特性）
  float error = angleError(measurement, setpoint);

  // 比例项
  float P = pid->Kp * error;

  // 积分项（带限幅）
  pid->integral += error * pid->dt;

  // 抗积分饱和
  if (pid->integral > pid->integral_max)
    pid->integral = pid->integral_max;
  else if (pid->integral < -pid->integral_max)
    pid->integral = -pid->integral_max;

  float I = pid->Ki * pid->integral;

  // 微分项（基于角速度）
  float angular_velocity = (error - pid->prev_error) / pid->dt;
  float D = pid->Kd * angular_velocity;

  // 更新状态
  pid->prev_error = error;

  // 计算总输出
  float output = P + I + D;

  // 输出限幅
  if (output > pid->output_max)
    output = pid->output_max;
  if (output < pid->output_min)
    output = pid->output_min;

  return output;
}

float positional_pid_angle_loop(Motor_InitStruct *motor) {
  float actual_angle = read_motor_angle(motor);

  PID_Controller *pid_angle_init = motor->pid_angle_init;
  anglePID_Update(pid_angle_init, pid_angle_init->target_angle, actual_angle);

  //   /**********真负目标速度,方向切换处理**************/
  if (pid_angle_init->output > 0) {
    tb6612_setDirection(motor, 1);
  } else {
    tb6612_setDirection(motor, 0);
    pid_angle_init->output = -pid_angle_init->output;
  }
  /*********************************************/
  //   tb6612_setPWM(pid_pos_init.output*20);
  tb6612_setPWM(motor, pid_angle_init->output);
  // my_printf("pulses = %.2f\n", current_pulses);

  printf("angle = %.2f\n", actual_angle);
}

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

float Incremental_PID_Update(PID_Controller *pid, float setpoint,
                             float measurement) {
  float error = setpoint - measurement;

  // 比例项
  float proportional = pid->Kp * (error - pid->prev_error);

  // 积分项(带抗饱和)
  // pid->integral += error;
  // if(pid->integral > pid->integral_max) pid->integral = pid->integral_max;
  // else if(pid->integral < pid->integral_min) pid->integral =
  // pid->integral_min;
  float integral = pid->Ki * error;

  // 微分项
  float derivative =
      pid->Kd * (error - 2 * pid->prev_error + pid->prev_error_2);

  // 计算总输出
  pid->output += (proportional + integral + derivative);

  // update error
  pid->prev_error_2 = pid->prev_error;
  pid->prev_error = error;

  // 输出限幅
  if (pid->output > pid->output_max)
    pid->output = pid->output_max;
  else if (pid->output < pid->output_min)
    pid->output = pid->output_min;

#if DEBUG_PID_INC * 1

  static int tim_count = 0;
  if (tim_count == 1) {
    tim_count = 0;
    printf("%.2f, %.2f\n", setpoint, measurement);
    printf("pid error = %.2f, previous error = %.2f, pid output = %.2f\n",
           pid->prev_error, pid->prev_error_2, pid->output);
    printf("p = %.2f, i = %.2f, d = %.2f\n", pid->Kp, pid->Ki, pid->Kd);
    printf("pro = %.2f, integral = %.2f, derivative = %.2f\n", proportional,
           integral, derivative);
    printf("\n");
  }
  tim_count++;

#endif

  return pid->output;
}

float positional_pid_position_loop(Motor_InitStruct *motor) {
  // int direction = read_motor_direction();
  // float current_pulses = read_motor_pulses();

  // float actual_speed = read_motor_speed(motor);
  float actual_position = read_motor_position(motor);
  // float target_position = g_target_position;
  PID_Controller *pid_pos_init = motor->pid_pos_init;
  PID_Update(pid_pos_init, pid_pos_init->target_position, actual_position);

  //   /**********真负目标速度,方向切换处理**************/
  if (pid_pos_init->output > 0) {
    tb6612_setDirection(motor, 1);
  } else {
    tb6612_setDirection(motor, 0);
    pid_pos_init->output = -pid_pos_init->output;
  }
  /*********************************************/
  //   tb6612_setPWM(pid_pos_init.output*20);
  tb6612_setPWM(motor, pid_pos_init->output);
  // my_printf("pulses = %.2f\n", current_pulses);

#ifdef DEBUG_PRINT
  printf("\n");
  my_printf("actual_position = %.2f, target_position = %.2f\n", actual_position,
            pid_pos_init->target_position);
  my_printf("direction = %d\n", read_motor_direction(motor));
  my_printf("actual_speed = %.2f\n", actual_speed);
  my_printf("output = %.2f\n", pid_pos_init->output);
#endif

#ifdef DEBUG_PLOT_PID
  // float motor_velocity = read_motor_speed(motor);
  // float motor_postion = read_motor_position(motor);
  // // printf("%.2f\n", motor_postion);
  printf("%.2f\n", actual_position);
#endif
  // static int tim_count = 0;
  // if (tim_count == 100) {
  //     tim_count = 0;
  //     printf("%.2f\n", actual_position);
  //     printf("overflow = %d\n", motor->pulse_count);
  //     printf("direction = %d\n",
  //     __HAL_TIM_IS_TIM_COUNTING_DOWN(motor->encoder_tim)); printf("pulse
  //     count = %d\n", __HAL_TIM_GET_COUNTER(motor->encoder_tim));
  //     printf("\n");

  // }
  // tim_count++;

  return actual_position;
  // bit_stream_test();
  // char_to_bit_stream();
}
float incremental_pid_position_loop(Motor_InitStruct *motor) {
  float actual_position = read_motor_position(motor);
  // float target_position = g_target_position;
  PID_Controller *pid_pos_init = motor->inc_pid_pos_init;
  Incremental_PID_Update(pid_pos_init, pid_pos_init->target_position,
                         actual_position);

  //   /**********真负目标速度,方向切换处理**************/
  if (pid_pos_init->output > 0) {
    tb6612_setDirection(motor, 1);
  } else {
    tb6612_setDirection(motor, 0);
    pid_pos_init->output = -pid_pos_init->output;
  }
  /*********************************************/
  //   tb6612_setPWM(pid_pos_init.output*20);
  tb6612_setPWM(motor, pid_pos_init->output);
  // my_printf("pulses = %.2f\n", current_pulses);

#ifdef DEBUG_PRINT
  printf("\n");
  my_printf("actual_position = %.2f, target_position = %.2f\n", actual_position,
            pid_pos_init->target_position);
  my_printf("direction = %d\n", read_motor_direction(motor));
  my_printf("actual_speed = %.2f\n", actual_speed);
  my_printf("output = %.2f\n", pid_pos_init->output);
#endif

#ifdef DEBUG_PLOT_PID
  // float motor_velocity = read_motor_speed(motor);
  // float motor_postion = read_motor_position(motor);
  // // printf("%.2f\n", motor_postion);
  printf("%.2f\n", actual_position);
#endif
#if DEBUG_PID_INC * 1
  static int tim_count = 0;
  if (tim_count == 100) {
    tim_count = 0;
    printf("%.2f\n", actual_position);
    printf("overflow = %d\n", motor->pulse_count);
    printf("direction = %d\n",
           __HAL_TIM_IS_TIM_COUNTING_DOWN(motor->encoder_tim));
    printf("pulse count = %d\n", __HAL_TIM_GET_COUNTER(motor->encoder_tim));
    printf("\n");
  }
  tim_count++;
#endif

  printf("%.2f, %.2f\n", pid_pos_init->target_position, actual_position);
  return actual_position;
  // bit_stream_test();
  // char_to_bit_stream();
}

void positional_pid_velocity_loop(Motor_InitStruct *motor) {
  // int direction = read_motor_direction(motor);
  float actual_speed = read_motor_speed(motor);
  PID_Controller *pid_vel_init = motor->inc_pid_vel_init;

  PID_Update(pid_vel_init, pid_vel_init->target_velocity, actual_speed);
  // my_printf("out = %.2f\n", pid_vel_init.output);

  /**********真负目标速度,方向切换处理**************/
  if (pid_vel_init->output > 0) {
    tb6612_setDirection(motor, 1);
  } else {
    tb6612_setDirection(motor, 0);
    pid_vel_init->output = -pid_vel_init->output;
  }
  /*********************************************/
  tb6612_setPWM(motor, pid_vel_init->output);

#ifdef DEBUG_PRINT
  my_printf("actual_speed = %.2f, target_speed = %.2f\n", actual_speed,
            pid_vel_init->target_velocity);
  my_printf("direction = %d\n", direction);
  printf("overflow_count = %d\n", motor->pulse_count);
  // my_printf("delta count = %.2f\n", delta_count);
  my_printf("error = %.2f\n", pid_vel_init->prev_error);
  printf("output = %.2f\n", pid_vel_init->output);
  // my_printf("integral = %.2f\n", pid_vel_init.integral);
  my_printf("\n");
#endif

#ifdef DEBUG_PLOT_PID
  printf("%.2f\n", actual_speed);
#endif
  printf("%.2f\n", actual_speed);
}
PID_Controller dual_pid_pos_init = {.Kp = 0.4f,
                                    .Ki = 0.00f,
                                    .Kd = 2.00f,
                                    .output_max = 5000.0f,
                                    .output_min = -5000.0f,
                                    .integral_max = 100.0f,
                                    .integral_min = -100.0f,
                                    .dt = PID_UPDATE_TIME

};
void incremental_pid_velocity_loop(Motor_InitStruct *motor) {
  // int direction = read_motor_direction(motor);
  float actual_speed = read_motor_speed(motor);
  PID_Controller *pid_vel_init = motor->inc_pid_vel_init;

  Incremental_PID_Update(pid_vel_init, pid_vel_init->target_velocity,
                         actual_speed);
  // my_printf("out = %.2f\n", pid_vel_init.output);

  /**********真负目标速度,方向切换处理**************/
  if (pid_vel_init->output > 0) {
    tb6612_setDirection(motor, 1);
  } else {
    tb6612_setDirection(motor, 0);
    pid_vel_init->output = -pid_vel_init->output;
  }
  /*********************************************/
  tb6612_setPWM(motor, pid_vel_init->output);

#if DEBUG_PID_INC * 0

  static int tim_count = 0;
  if (tim_count == 100) {
    tim_count = 0;
    printf("position = %d, overflow = %d\n", read_motor_position(motor),
           motor->pulse_count);
    printf("\n");
  }
  tim_count++;

#endif
}

PID_Controller dual_pid_vel_init = {.Kp = 3.0,
                                    .Ki = 0.10f,
                                    .Kd = 0.00f,
                                    .output_max = 5000.0f,
                                    .output_min = -5000.0f,
                                    .integral_max = 100.0f,
                                    .integral_min = -100.0f,
                                    .dt = PID_UPDATE_TIME

};

// 双环位置控制3
void dual_pid_position(Motor_InitStruct *motor) {
  float current_pos = read_motor_position(motor);
  float current_vel = read_motor_speed(motor);
  float target_position = motor->pid_pos_init->target_position;

  // 位置环计算期望速度
  float target_vel =
      PID_Update(&dual_pid_pos_init, target_position, current_pos);

  // 速度环计算最终输出
  float output = PID_Update(&dual_pid_vel_init, target_vel, current_vel);
  if (output > 0) {
    tb6612_setDirection(motor, 1);
  } else {
    tb6612_setDirection(motor, 0);
    output = -output;
  }
  // 应用输出到执行机构
  tb6612_setPWM(motor, output);
#ifdef DEBUG_PRINT
  my_printf("targe_pos = %.2f, current_pos = %.2f, output = %.2f\n",
            target_position, current_pos, output);
  my_printf("target_speed = %.2f,current_speed = %.2f\n", target_vel,
            current_vel);
  my_printf("innerIntegral = %.2f\n", dual_pid_vel_init.integral);
  // my_printf("innerError = %.2f\n", InnerError0);
  // my_printf("outerTarget = %.2f, outerActual = %.2f\n", OuterTarget,
  // OuterActual); my_printf("outerError = %.2f\n", OuterError0);
  // my_printf("outerIntegral = %.2f\n", OuterErrorInt);
  // my_printf("outerOut = %.2f\n", OuterOut);
  // my_printf("deltaCount = %.2f\n", delta_count);
  my_printf("\n");
#endif
#ifdef DEBUG_PLOT_PID
  printf("%.2f, %.2f\n", target_vel, current_pos);
#endif
  printf("%.2f, %.2f\n", target_vel, current_pos);
}

#define BIT_0000 0000.0f + 5000.0f
#define BIT_0001 625.0f * 1 + 5000.0f
#define BIT_0010 625.0f * 2 + 5000.0f
#define BIT_0011 625.0f * 3 + 5000.0f
#define BIT_0110 625.0f * 4 + 5000.0f
#define BIT_1010 625.0f * 5 + 5000.0f
#define BIT_1110 625.0f * 6 + 5000.0f
#define BIT_0100 625.0f * 7 + 5000.0f
#define BIT_1000 0000.0f
#define BIT_1100 625.0f * 1
#define BIT_0101 625.0f * 2
#define BIT_1001 625.0f * 3
#define BIT_1101 625.0f * 4
#define BIT_0111 625.0f * 5
#define BIT_1011 625.0f * 6
#define BIT_1111 625.0f * 7
// #define   BIT_00                POSITION_INCREMENT_00
// #define   BIT_01                POSITION_INCREMENT_01
// #define   BIT_11                POSITION_INCREMENT_11
// #define   BIT_10                POSITION_INCREMENT_10
void timer_control_position_qam16(Motor_InitStruct *motor) {
  static int tim_count = 0;
  static float bit[16] = {BIT_0000, BIT_0001, BIT_0010, BIT_0011,
                          BIT_0110, BIT_1010, BIT_1110, BIT_0100,
                          BIT_1000, BIT_1100, BIT_0101, BIT_1001,
                          BIT_1101, BIT_0111, BIT_1011, BIT_1111};
  static int bit_count = 0;
  if (tim_count > 2 / PID_UPDATE_TIME) {
    tim_count = 0;
    int a[16];
    for (int i = 0; i < 16; i++) {
      int b = rand() % 16;
      a[i] = b;
    }
    motor->pid_pos_init->target_position = bit[a[bit_count++]];
    // bit_count++;
    if (bit_count == 16) {
      bit_count = 0;
    }
  }
  tim_count++;
}
// 16 QAM 依次增加1250位置
void timer_control_position_qam16_sequence(Motor_InitStruct *motor) {
  static int tim_count = 0;
  static int bit_base = 0;
  static int qam16_base = 1250;
  static int bit_count = 0;
  static int circle_count = 0;
  static int circle_base = 20000;
  if (tim_count > 1 / PID_UPDATE_TIME) {
    tim_count = 0;
    motor->pid_pos_init->target_position =
        bit_base + qam16_base * (bit_count++) + circle_base * circle_count;
    // bit_count++;
    if (bit_count == 16) {
      bit_count = 0;
      circle_count++;
    }
  }
  tim_count++;
}
#if 1
#define BIAS 0
#define POSITION_INCREMENT_00 0000.0f
#define POSITION_INCREMENT_01 0000.0f + (5000.0f * 4 + BIAS) / 4
#define POSITION_INCREMENT_11                                                  \
  0000.0f + (5000.0f * 4 + BIAS) / 2 + (5000.0f * 4 + BIAS) / 4
#define POSITION_INCREMENT_10 0000.0f + (5000.0f * 4 + BIAS) / 2
#define BIT_00 POSITION_INCREMENT_00
#define BIT_01 POSITION_INCREMENT_01
#define BIT_11 POSITION_INCREMENT_11
#define BIT_10 POSITION_INCREMENT_10
#endif

#if 1
#define BIAS 0
#define POSITION_INCREMENT_00 0000.0f
#define POSITION_INCREMENT_01 0000.0f + (5000.0f * 4 + BIAS) / 4
#define POSITION_INCREMENT_11                                                  \
  0000.0f + (5000.0f * 4 + BIAS) / 2 + (5000.0f * 4 + BIAS) / 4
#define POSITION_INCREMENT_10 0000.0f + (5000.0f * 4 + BIAS) / 2
#define BIT_00_1 POSITION_INCREMENT_00
#define BIT_01_1 POSITION_INCREMENT_01
#define BIT_11_1 POSITION_INCREMENT_11
#define BIT_10_1 POSITION_INCREMENT_10
#endif
// #define ADD_BIAS      10
// int add_bias[4] = {-18, -10, -10, -0};
#define OFFSET_COMPASTION 41
int add_bias[4] = {OFFSET_COMPASTION, OFFSET_COMPASTION, OFFSET_COMPASTION,
                   OFFSET_COMPASTION};
void timer_control_position_qam_sequence(Motor_InitStruct *motor) {
  static int tim_count = 0;
  static float bit[4] = {BIT_00_1, BIT_01_1, BIT_10_1, BIT_11_1};
  static int bit_count = 0;
  static int bias_count = 0;
  static int circle_count = 0;
  static int circle_base = 20000;
  float bit_update_time = 0.5;

  if (tim_count > bit_update_time / PID_UPDATE_TIME) {

    if (bit[bit_count] == BIT_00_1) {
      motor->pid_pos_init->target_position =
          BIT_00_1 + add_bias[0] * bias_count + circle_count * circle_base;
    }
    if (bit[bit_count] == BIT_01_1) {
      motor->pid_pos_init->target_position =
          BIT_01_1 + add_bias[1] * bias_count + circle_count * circle_base;
    }
    if (bit[bit_count] == BIT_11_1) {
      motor->pid_pos_init->target_position =
          BIT_11_1 + add_bias[2] * bias_count + circle_count * circle_base;
    }
    if (bit[bit_count] == BIT_10_1) {
      motor->pid_pos_init->target_position =
          BIT_10_1 + add_bias[3] * bias_count + circle_count * circle_base;
    }
    bit_count++;
    bias_count++;
    // if (bias_count == 38) {
    //   bias_count = 0;
    // }
    if (bit_count == 4) {
      bit_count = 0;
      circle_count++;
    }

    tim_count = 0;
  }
  tim_count++;
}
void timer_control_position_qam_sequence_without_offset(
    Motor_InitStruct *motor) {
  static int tim_count = 0;
  static float bit[4] = {BIT_00, BIT_01, BIT_11, BIT_10};
  static int bit_count = 0;
  static int bias_count = 0;
  float bit_update_time = 1.0;

  if (tim_count > bit_update_time / PID_UPDATE_TIME) {

    if (bit[bit_count] == BIT_00) {
      motor->pid_pos_init->target_position = BIT_00;
    }
    if (bit[bit_count] == BIT_01) {
      motor->pid_pos_init->target_position = BIT_01;
    }
    if (bit[bit_count] == BIT_11) {
      motor->pid_pos_init->target_position = BIT_11;
    }
    if (bit[bit_count] == BIT_10) {
      motor->pid_pos_init->target_position = BIT_10;
    }
    bit_count++;
    bias_count++;
    // if (bias_count == 38) {
    //   bias_count = 0;
    // }
    if (bit_count == 4) {
      bit_count = 0;
    }

    tim_count = 0;
  }
  tim_count++;
}
void timer_control_position_qam(Motor_InitStruct *motor) {
  static int tim_count = 0;
  static float bit[4] = {BIT_00, BIT_01, BIT_11, BIT_10};
  static int bit_count = 0;
  static int bias_count = 0;
  float bit_update_time = 1.0;

  if (tim_count > bit_update_time / PID_UPDATE_TIME) {

    if (bit[bit_count] == BIT_00) {
      motor->pid_pos_init->target_position = BIT_00 + add_bias[0] * bias_count;
    }
    if (bit[bit_count] == BIT_01) {
      motor->pid_pos_init->target_position = BIT_01 + add_bias[1] * bias_count;
    }
    if (bit[bit_count] == BIT_11) {
      motor->pid_pos_init->target_position = BIT_11 + add_bias[2] * bias_count;
    }
    if (bit[bit_count] == BIT_10) {
      motor->pid_pos_init->target_position = BIT_10 + add_bias[3] * bias_count;
    }
    bit_count++;
    bias_count++;
    // if (bias_count == 38) {
    //   bias_count = 0;
    // }
    if (bit_count == 4) {
      bit_count = 0;
    }

    tim_count = 0;
  }
  tim_count++;
}

void timer_control_position_qam_adaptive(Motor_InitStruct *motor) {
  static int tim_count = 0;
  static float bit[4] = {BIT_00, BIT_01, BIT_11, BIT_10};
  static int bit_count = 0;
  static int bias_count = 0;
  static int delay_count = 0;
  // float bit_update_time[4] = {1.0, 1, 1, 3};
  float bit_update_time = 1;
  // float bit_delay_time[4] = {1-0.835, 1-1.115, 1-0.925, 1-1.125};
  // float bit_delay_time[4] = {1.5, 1, 0.5, 0.2};
  float bit_delay_time[4] = {1 - 0.96, 1 - 1.02, 1 - 1.02, 1 - 1.01};

  if (tim_count >=
      (bit_update_time - bit_delay_time[delay_count]) / PID_UPDATE_TIME) {
    // if (delay_count == 4) {
    //   delay_count = 0;
    // } else {
    //     delay_count++;
    //     }
    printf("delay time = %.2f, time_count = %d\n", bit_delay_time[delay_count],
           tim_count);
    delay_count = (delay_count == 3) ? 0 : delay_count + 1;

    if (bit[bit_count] == BIT_00) {
      motor->pid_pos_init->target_position = BIT_00 + add_bias[0] * bias_count;
    }
    if (bit[bit_count] == BIT_01) {
      motor->pid_pos_init->target_position = BIT_01 + add_bias[1] * bias_count;
    }
    if (bit[bit_count] == BIT_11) {
      motor->pid_pos_init->target_position = BIT_11 + add_bias[2] * bias_count;
    }
    if (bit[bit_count] == BIT_10) {
      motor->pid_pos_init->target_position = BIT_10 + add_bias[3] * bias_count;
    }
    bit_count++;
    bias_count++;
    // if (bias_count == 38) {
    //   bias_count = 0;
    // }
    if (bit_count == 4) {
      bit_count = 0;
    }

    tim_count = 0;
  }
  tim_count++;
}

void timer_control_position_qam_inc(Motor_InitStruct *motor) {
  static int tim_count = 0;
  static float bit[4] = {BIT_00, BIT_01, BIT_11, BIT_10};
  static int bit_count = 0;
  static int bias_count = 0;
  PID_Controller *pid_init = motor->inc_pid_pos_init;
  if (tim_count > 1 / PID_UPDATE_TIME) {

    if (bit[bit_count] == BIT_00) {
      pid_init->target_position = BIT_00 + add_bias[0] * bias_count;
    }
    if (bit[bit_count] == BIT_01) {
      pid_init->target_position = BIT_01 + add_bias[1] * bias_count;
    }
    if (bit[bit_count] == BIT_11) {
      pid_init->target_position = BIT_11 + add_bias[2] * bias_count;
    }
    if (bit[bit_count] == BIT_10) {
      pid_init->target_position = BIT_10 + add_bias[3] * bias_count;
    }
    bit_count++;
    bias_count++;
    if (bias_count == 38) {
      bias_count = 0;
    }
    if (bit_count == 4) {
      bit_count = 0;
    }

    tim_count = 0;
  }
  tim_count++;
}
void tim_control_position(void) {
  static int tim_count = 0;
  if (tim_count > 1 / PID_UPDATE_TIME) {
    g_target_position += POSITION_INCREMENT;
    tim_count = 0;
  }
  tim_count++;
}
void tim_control_position_qam4(void) {
  static int tim_count = 0;
  static float bit[4] = {BIT_00, BIT_01, BIT_11, BIT_10};
  static int bit_count = 0;
  if (tim_count > 2 / PID_UPDATE_TIME) {

    if (bit[bit_count] == BIT_00) {
      g_target_position = BIT_00;
    }
    if (bit[bit_count] == BIT_01) {
      g_target_position = BIT_01;
    }
    if (bit[bit_count] == BIT_11) {
      g_target_position = BIT_11;
    }
    if (bit[bit_count] == BIT_10) {
      g_target_position = BIT_10;
    }
    bit_count++;
    if (bit_count == 4) {
      bit_count = 0;
    }
    tim_count = 0;
  }
  tim_count++;
}
void tim_control_position_qam_test(void) {
  static int tim_count = 0;
  static float bit[4] = {BIT_00, BIT_01, BIT_11, BIT_10};
  static int bit_count = 0;
  if (tim_count > 2 / PID_UPDATE_TIME) {

    if (bit[bit_count] == BIT_00) {
      g_target_position = BIT_00;
    }
    if (bit[bit_count] == BIT_01) {
      g_target_position = BIT_01;
    }
    if (bit[bit_count] == BIT_11) {
      g_target_position = BIT_11;
    }
    if (bit[bit_count] == BIT_10) {
      g_target_position = BIT_10;
    }
    bit_count++;
    if (bit_count == 4) {
      bit_count = 0;
    }
    tim_count = 0;
  }
  tim_count++;
}

void tim_control_position_dual_direction(void) {
  static int tim_count = 0;
  static int rot_cnt = 0;
  static int rot_flag = 0;
  if (tim_count > 1 / PID_UPDATE_TIME) {
    tim_count = 0;
    rot_cnt++;
    rot_flag = 1;
  }
  if (rot_cnt == 10) {
    rot_flag = -1;
    rot_cnt = 0;
  }
  if (rot_flag == 1) {
    g_target_position += POSITION_INCREMENT;
    rot_flag = 0;
  } else if (rot_flag == -1) {
    g_target_position -= POSITION_INCREMENT;
    rot_flag = 0;
  }

  tim_count++;
}

void tim_bits_to_position(void) {
  static int tim_count = 0;
  if (tim_count > 1 / PID_UPDATE_TIME) {
    g_target_position += POSITION_INCREMENT;
    tim_count = 0;
  }
  tim_count++;
}

void tb6612_setDirection(Motor_InitStruct *motor, int direction) {
  tb6612_direction(motor, direction);
}

void tb6612_direction(Motor_InitStruct *motor, int direction) {
  switch (direction) {
  case CLOCKWISE:
    HAL_GPIO_WritePin(motor->AIN1_GPIO_Port, motor->AIN1_GPIO_Pin, SET);
    HAL_GPIO_WritePin(motor->AIN2_GPIO_Port, motor->AIN2_GPIO_Pin, RESET);
    // my_printf("counterclockwise\n");
    break;
  case NOT_CLOCKWISE:
    HAL_GPIO_WritePin(motor->AIN1_GPIO_Port, motor->AIN1_GPIO_Pin, RESET);
    HAL_GPIO_WritePin(motor->AIN2_GPIO_Port, motor->AIN2_GPIO_Pin, SET);
    // my_printf("clockwise\n");
    break;
  case 2:
    HAL_GPIO_WritePin(motor->AIN1_GPIO_Port, motor->AIN1_GPIO_Pin, RESET);
    HAL_GPIO_WritePin(motor->AIN2_GPIO_Port, motor->AIN2_GPIO_Pin, RESET);
    // my_printf("motor is stopped\n");
    break;
  default:
    HAL_GPIO_WritePin(motor->AIN1_GPIO_Port, motor->AIN1_GPIO_Pin, RESET);
    HAL_GPIO_WritePin(motor->AIN2_GPIO_Port, motor->AIN2_GPIO_Pin, RESET);
    break;
  }
}
void tb6612_setPWM(Motor_InitStruct *motor, float duty) {
  // __HAL_TIM_SET_PRESCALER(motor->pwm_tim, 3);
  // __HAL_TIM_SetAutoreload(motor->pwm_tim, 4095);
  __HAL_TIM_SET_PRESCALER(motor->pwm_tim, PWM_TIM_PSC - 1);
  __HAL_TIM_SetAutoreload(motor->pwm_tim, PWM_RESOLUTION - 1);
  __HAL_TIM_SetCompare(motor->pwm_tim, motor->pwm_channel, duty - 1);
  // __HAL_TIM_SetAutoreload(motor->pwm_tim, TB6612_PWM1_PERIOD - 1);
  // __HAL_TIM_SetCompare(motor->pwm_tim, motor->pwm_channel, duty - 1);
}
extern int g_tim_flag;
void key_control_position(Motor_InitStruct *motor) {
  if (Key_Scan(1) == KEY_ON) {
    my_printf("key2 is pressed\n");
    motor->pid_pos_init->target_position += POSITION_INCREMENT;
  }
  // else if(Key_Scan(1) == KEY_ON){
  //   my_printf("key1 is pressed\n");
  //   motor->pid_pos_init->target_position -= POSITION_INCREMENT;
  //   printf("target position = %.2f\n",motor->pid_pos_init->target_position);
  //   // g_tim_flag = -g_tim_flag;
  // }
}
void key_control_velocity(void) {
  if (Key_Scan(2) == 1) {
    // pid_vel_init.target_velocity = -pid_vel_init.target_velocity;
    // pid_vel_init.target_velocity += 1;
  }
}
