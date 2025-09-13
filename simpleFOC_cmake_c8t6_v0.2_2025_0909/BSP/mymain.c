#include "mymain.h"
#include "as5600.h"
#include "foc_base.h"
#include "motor_config.h"
#include "open_loop_FOC.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"
#include "test/test.h"
#include "tim.h"
void calc_time(void) {
  static int last_time = 0;
  int now_time, delta_time;
  now_time = _micros();
  delta_time = now_time - last_time;
  last_time = now_time;
  printf("nowtime = %d, delta_time = %d\n", now_time, delta_time);
  HAL_Delay(1000);
}
int mymain(void) {
  float a = 1.2;
  int b = 10;
  float angle;
  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_Base_Start(&htim4);

  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
  __HAL_TIM_SET_PRESCALER(&htim1, 0);
  __HAL_TIM_SET_PRESCALER(&htim4, 0);
  __HAL_TIM_SET_AUTORELOAD(&htim1, PWM_RESOLUTION);
  __HAL_TIM_SET_AUTORELOAD(&htim4, PWM_RESOLUTION);
  while (1) {

    // calc_time();

    // printf("hello foc %lf\n", a);
    // HAL_Delay(1000);

    // angleOpenloop(_PI_2);

    //    led_blink(10);
    //    printf("\n");
    //    printf("hello makefile22\n");
    //    printf("hello gcc, %f, %d\n", a, b);

    //    UART_SendStr("hello stm32\n");

    // getVelocity();
    // printf("\n");

    // test_led();

    // velocityOpenloop(1);
    // test_velocityOpenLoop();
    // test_positionClosedLoop();

    // test_pid();
    // test_torque();

    // AS5600_test();
    // test_as5600_angle();
    // test_sensor();
    // test_motor();

    // test_alignSensor();
    // test_alignSensor_struct();
    // test_struct_openloop_velocity();
    // test_as5600_new();
    // test_as5600_vel1();
    // test_as5600_shaftAngle();
    test_pid();
    // test_pid_angle();
  }
}
