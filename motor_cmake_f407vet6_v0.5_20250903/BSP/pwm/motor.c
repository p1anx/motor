#include "motor.h"
// #include "pid_init.h"
#include "bit_stream.h"
#include "motor_config.h"
#include "pid_init.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "tb6612.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  float position;
  int time;
} Calc_Struct;
// PID_Controller pid_pos_init = {
//     .Kp = 0.01f,
//     .Ki = 0.00f,
//     .Kd = 0.00f,
//     .output_max = 500.0f,
//     .output_min = -500.0f,
//     .integral_max = 100.0f,
//     .integral_min = -100.0f,
//     .target_position = 10000.0f
// };
// it's ok but shift position
// PID_Controller pid_pos_init = {
//     .Kp = 0.8f, //0.6
//     .Ki = 0.00f,
//     .Kd = 3.50f, //1.5f
//     .output_max = 500.0f,
//     .output_min = -500.0f,
//     .integral_max = 100.0f,
//     .integral_min = -100.0f,
//     .target_position = 10000.0f
// };
PID_Controller pid_pos_init = {.Kp = 0.8f, // 0.6
                               .Ki = 0.00f,
                               .Kd = 3.50f, // 1.5f
                               .output_max = 500.0f,
                               .output_min = -500.0f,
                               .integral_max = 100.0f,
                               .integral_min = -100.0f,
                               .target_position = 10000.0f};
// motor2 PID value
PID_Controller pid_vel_init = {
    .Kp = 100.0f, // 200.0f
    .Ki = 1.2f,   // 1.2f
    .Kd = 00.00f, // 50.0f
    .output_max = 500.0f,
    .output_min = -500.0f,
    .integral_max = 100.0f,
    .integral_min = -100.0f,
    .target_velocity = 2
    // .target_velocity = -2
};
PID_Controller pid_angle_init = {.Kp = 0.8f, // 0.6
                                 .Ki = 0.05f,
                                 .Kd = 0.00f, // 1.5f
                                 .output_max = 500.0f,
                                 .output_min = -500.0f,
                                 .integral_max = 100.0f,
                                 .integral_min = -100.0f,
                                 .target_angle = 90};
PID_Controller inc_pid_pos_init = {.Kp = 0.8f, // 0.6
                                   .Ki = 0.00f,
                                   .Kd = 0.00f, // 1.5f
                                   .prev_error_2 = 0,
                                   .prev_error = 0,
                                   .output_max = 500.0f,
                                   .output_min = -500.0f,
                                   .integral_max = 100.0f,
                                   .integral_min = -100.0f,
                                   .target_position = 10000.0f};
PID_Controller inc_pid_vel_init = {.Kp = 100.0f, // 0.6
                                   .Ki = 10.00f,
                                   .Kd = 0.00f, // 1.5f
                                   .prev_error_2 = 0,
                                   .prev_error = 0,
                                   .output_max = 500.0f,
                                   .output_min = -500.0f,
                                   .integral_max = 100.0f,
                                   .integral_min = -100.0f,
                                   .target_velocity = 2};
#if MOTOR1_ON
Motor_InitStruct motor1 = {.encoder_tim = &encoder_htim,
                           .pwm_tim = &tb6612_pwm1_htim,
                           .pwm_channel = TB6612_PWM1_CHANNEL_x,
                           .pid_update_tim = &pid_htim,
                           .pulse_count = -1,
                           .AIN1_GPIO_Port = TB6612_AIN1_GPIO_PORT,
                           .AIN1_GPIO_Pin = TB6612_AIN1_GPIO_PIN,
                           .AIN2_GPIO_Port = TB6612_AIN2_GPIO_PORT,
                           .AIN2_GPIO_Pin = TB6612_AIN2_GPIO_PIN,
                           .pid_pos_init = &pid_pos_init,
                           .pid_vel_init = &pid_vel_init,
                           .inc_pid_vel_init = &inc_pid_vel_init,
                           .inc_pid_pos_init = &inc_pid_pos_init

};
#endif
#if MOTOR2_ON
Motor_InitStruct motor2 = {.encoder_tim = &encoder2_htim,
                           .pwm_tim = &tb6612_pwm2_htim,
                           .pwm_channel = TB6612_PWM2_CHANNEL_x,
                           .pid_update_tim = &pid_htim,
                           .pulse_count = 0,
                           // .AIN1_GPIO_Port = TB6612_MOTOR2_AIN1_GPIO_PORT,
                           // .AIN1_GPIO_Pin  = TB6612_MOTOR2_AIN1_GPIO_PIN,
                           // .AIN2_GPIO_Port = TB6612_MOTOR2_AIN2_GPIO_PORT,
                           // .AIN2_GPIO_Pin  = TB6612_MOTOR2_AIN2_GPIO_PIN

                           .AIN2_GPIO_Port = TB6612_MOTOR2_AIN1_GPIO_PORT,
                           .AIN2_GPIO_Pin = TB6612_MOTOR2_AIN1_GPIO_PIN,
                           .AIN1_GPIO_Port = TB6612_MOTOR2_AIN2_GPIO_PORT,
                           .AIN1_GPIO_Pin = TB6612_MOTOR2_AIN2_GPIO_PIN,
                           .pid_pos_init = &pid_pos_init,
                           .pid_vel_init = &pid_vel_init};
#endif

// spend time to rotate a angle
GlobalVar_Struct gTime = {.time_flag = 0};

int timer_increase_position(Motor_InitStruct *motor) {
  static int tim_count = 0;
  int now_time;
  positional_pid_position_loop(motor);
  if (tim_count > 1 / PID_UPDATE_TIME) {
    // now_time = HAL_GetTick();
    motor->pid_pos_init->target_position += POSITION_INCREMENT;
    tim_count = 0;
  }
  tim_count++;
  return now_time;
}
int timer_increase_position_inc(Motor_InitStruct *motor) {
  static int tim_count = 0;
  int now_time;
  incremental_pid_position_loop(motor);
  if (tim_count > 1 / PID_UPDATE_TIME) {
    // now_time = HAL_GetTick();
    motor->inc_pid_pos_init->target_position += POSITION_INCREMENT;
    tim_count = 0;
  }
  tim_count++;
  return now_time;
}
int timer_increase_angle(Motor_InitStruct *motor) {
  static int tim_count = 0;
  //   int now_time;
  positional_pid_angle_loop(motor);
  if (tim_count > 1 / PID_UPDATE_TIME) {
    // now_time = HAL_GetTick();
    motor->pid_angle_init->target_angle += ANGLE_INC;
    tim_count = 0;
  }
  tim_count++;
  //   return now_time;
}
void qam_modulate(void) {
  // dual_pid_position(&motor1);

  // 1. 4qam
  positional_pid_position_loop(&motor1);
  // timer_control_position_qam(&motor1);
  // timer_control_position_qam_adaptive(&motor1);

  // timer_control_position_qam_sequence(&motor1);

  timer_control_position_qam16_sequence(&motor1);
  // 2. 16qam
  // timer_control_position_qam16(&motor1);
  // calc_rotating_time();
  // 3. incremental qam
  //  incremental_pid_position_loop(&motor1);
  //  timer_control_position_qam_inc(&motor1);
}

// int timer_increase_position(Motor_InitStruct* motor) {
//   static int tim_count = 0;
//   if(tim_count > 1/PID_UPDATE_TIME){
//         motor->pid_pos_init->target_position += POSITION_INCREMENT;
//         tim_count = 0;
//   }
//   tim_count++;
//   return 0;
// }

// typedef enum ;
enum STATUS_FLAG {
  // DEFAULT,
  START_CALC,
  END_CALC
} status_flag;
// status_flag = END_CALC;
int calc_rotating_time(void) {
  int trigger_time = 1; // s
  Motor_InitStruct *motor = &motor1;

  static float last_location = 0;
  static int timer_count = 0;
  int current_t;
  int end_t, delta_t;
  static int start_flag = 0;

  float current_location = read_motor_position(motor);
  float delta_loc = current_location - last_location;
  delta_loc = delta_loc > 0 ? delta_loc : -delta_loc;
  printf("now location = %.2f, last location  = %.2f, delta location = %.2f\n",
         current_location, last_location, delta_loc);
  last_location = current_location;
  // if (start_flag == 0) {
  //     if (delta_loc > 0) {
  //         start_flag = 1;
  //         current_t = HAL_GetTick();
  //     }

  // }
  // if (start_flag == 1) {
  //     if (delta_loc == 0) {
  //       start_flag = 0;
  //       end_t = HAL_GetTick();
  //     }
  // }
  // printf("current time = %d, end time = %d\n", current_t, end_t);

  // float current_speed = read_motor_speed(motor);
  // if (current_speed > 0) {
  // printf("speed = %.2f\n", current_speed);

  // }

  // if (start_flag == 0) {
  //     if(delta_loc == 0){
  //     // if (current_speed > 0) {
  //             // printf("speed = %.2f\n", current_speed);
  //             current_t = HAL_GetTick();
  //             printf("curret time = %d\n", current_t);
  //             start_flag = 1;

  //     }
  // // }
  // // printf("now location = %.2f, delta location = %.2f\n", current_location,
  // delta_loc);
  // // if (start_flag == 1) {

  //     if(delta_loc > 0)
  //     // if (current_speed == 0)
  //     {
  //         start_flag = 0;
  //         end_t = HAL_GetTick();
  //         printf("start time = %d, end time = %d\n", current_t, end_t);
  //         delta_t = end_t - current_t;
  //         current_t = end_t;
  //         last_location = current_location;
  //         // return 0;
  //         printf("now location = %.2f, delta location = %.2f\n",
  //         current_location, delta_loc); printf("spend time = %d ms\n",
  //         delta_t); printf("\n"); delta_loc = 0;
  //     }
  // }

  return 0;
}
int calc_time_use_timer(void) {
  int trigger_time = 1; // s
  Motor_InitStruct *motor = &motor1;

  static float last_location = 0;
  static int timer_count = 0;
  int current_t;
  current_t = HAL_GetTick();
  MG513_Init(motor);
  while (1) {
    int end_t, delta_t;
    // printf("in while\n");
    if (gTime.time_flag > trigger_time / PID_UPDATE_TIME) {
      gTime.time_flag = 0;
      current_t = HAL_GetTick();
      motor->pid_pos_init->target_position += POSITION_INCREMENT;
      printf("overflow = %d\n", motor->pulse_count);
    }
    float current_location = read_motor_position(motor);
    float delta_loc = current_location - last_location;
    delta_loc = delta_loc > 0 ? delta_loc : -delta_loc;
    if (delta_loc > POSITION_INCREMENT) {
      end_t = HAL_GetTick();
      printf("start time = %d, end time = %d\n", current_t, end_t);
      delta_t = end_t - current_t;
      current_t = end_t;
      last_location = current_location;
      // return 0;
      printf("now location = %.2f, delta location = %.2f\n", current_location,
             delta_loc);
      printf("spend time = %d ms\n", delta_t);
      printf("\n");
      delta_loc = 0;
    }
    // key_control_position(&motor1);
  }

  return 0;
}
int time_test(void) {
  static float last_location = 0;
  int current_t;
  current_t = HAL_GetTick();
  MG513_Init(&motor1);
  while (1) {
    int end_t, delta_t;
    float current_location = read_motor_position(&motor1);
    float delta_loc = current_location - last_location;
    delta_loc = delta_loc > 0 ? delta_loc : -delta_loc;
    // printf("in while\n");
    if (delta_loc > POSITION_INCREMENT) {
      end_t = HAL_GetTick();
      printf("start time = %d\n", current_t);
      delta_t = end_t - current_t;
      current_t = end_t;
      last_location = current_location;
      delta_loc = 0;
      // return 0;
      printf("now location = %.2f, delta location = %.2f\n", current_location,
             delta_loc);
      printf("spend time = %d, end time = %d\n", delta_t, end_t);
      printf("\n");
    }
    if (Key_Scan(1) == KEY_ON) {
      my_printf("key1 is pressed\n");
      current_t = HAL_GetTick();
      motor1.pid_pos_init->target_position += POSITION_INCREMENT;
      printf("now time = %d\n", current_t);
    }
    // key_control_position(&motor1);
  }

  return 0;
}
void motor_run(void) {
  printf("hello motor\n");
#if MOTOR1_ON
  MG513_Init(&motor1);
#endif

#if MOTOR2_ON
  MG513_Init(&motor2);
#endif
  while (1) {

    if (Key_Scan(1) == KEY_ON) {
      LED1_TOGGLE();
      printf("key is pressed \n");
      // HAL_Delay(1000);
    }
    key_control_position(&motor1);
    // key_control_velocity();
  }
}

void pid(void) {
  Motor_InitStruct *motor;
  motor = &motor1;
  TB6612_ENABLE();

  // #if DEBUG_PID_VALUE
  //     get_pid_value_from_uart(motor);
  // #endif
  // positional_pid_velocity_loop(motor);
  // positional_pid_position_loop(motor);
  // static int tim_count= 0;
  // if (tim_count == 100) {
  //     tim_count = 0;
  //     printf("position = %.2f\n", read_motor_position(motor));
  // }
  // tim_count++;

  // 1. for qam modulate
  qam_modulate();

  // 2. increase the position in the cycle
  // timer_increase_position(&motor1);
  // calc_rotating_time();

  // dual_pid_position(motor);

  // 3. angle loop
  // positional_pid_angle_loop(motor);
  // timer_increase_angle(&motor1);

  /*********************************
           4. incremental pid
  **********************************/
  // incremental_pid_velocity_loop(motor);
  // incremental_pid_position_loop(motor);
  // timer_increase_position_inc(motor);
#ifdef DEBUG_PLOT_PID
  timer_increase_position(motor);
#endif

#if MOTOR1_ON * 0
  // qam_test();
  // positional_pid_velocity_loop(&motor1);
  positional_pid_position_loop(&motor1);
  // tim_control_position_qam_test();
  char_to_bit_stream_uart(&motor1);

#endif
#if MOTOR2_ON * 0
  positional_pid_velocity_loop(&motor2);
  // positional_pid_position_loop(&motor2);
  // char_to_bit_stream_uart(&motor2);
#endif
  // positional_pid_position_loop(&motor1);
}

int motor_main(void) {
  printf("motor is starting\n");
  Motor_InitStruct *motor = &motor1;
  MG513_Init(motor);
  HAL_Delay(10);
  // time_test();

  // time_test();
  // calc_time_use_timer();
  // calc_rotating_time();
  // motor_run();
#ifdef DEBUG_PLOT_PID
  motor_run();
#else
  // calc_time_use_timer();
#endif
  return 0;
}
