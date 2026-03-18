//
// Created by xwj on 1/21/26.
//

#include "QAM.h"

#include "config.h"

#include "my_uart.h"
#include <stdio.h>
#include <string.h>

extern char s_line_buf[UART_RX_LINE_MAX];
extern Motor_t motor;
#define QAM_SIZE 4
void QAM_run(Motor_t *motor) {
  float QAM_angle[QAM_SIZE] = {100, 40, 130, 20, 150};
  float QAM_P[QAM_SIZE]     = {10, 20, 30, 40, 50};
  static int QAM_index = 0;


  if (!motor->isEnabled) return;

  if (GetInterval_ms(1000)) {
    motor->ref = QAM_angle[QAM_index] ;
    motor->PID_iVelDegree.P = QAM_P[QAM_index];
    QAM_index++;
    if (QAM_index >= 5) QAM_index = 0;
  }
}
void QAM_run4QAM(Motor_t *motor) {
  float QAM_angle[QAM_SIZE] = {33.5, 275.4, 97.5, 214.3};
  float QAM_P[QAM_SIZE]     = {30, 20, 20, 20};
  // int qam_time[QAM_SIZE*2] = {2000, 2000, 2000, 2000, 2000, 1000, 3000, 2000};
  // int qam_time[QAM_SIZE*2] = {2000, 2000, 2000, 2000, 2000, 1500, 2500, 2000};
  int qam_time[QAM_SIZE*2] = {2000, 2000, 2000, 2000, 2000, 750, 3250, 2000};
  static int QAM_index = 0;
  static int t_index = 0;


  if (!motor->isEnabled) return;


  if (GetInterval_ms(qam_time[t_index])) {
    motor->ref = QAM_angle[QAM_index] ;
    motor->PID_iVelDegree.P = QAM_P[QAM_index];
    QAM_index++;
    if (QAM_index >= QAM_SIZE) QAM_index = 0;
    t_index++;
    if (t_index >= QAM_SIZE*2) {
      t_index = 0;
    }
  }
}

#define QAM_AMP_POINTS 100
void QAM_MeanAmp0(Motor_t *motor) {

  float QAM_angle[QAM_AMP_POINTS];
  float Amp_slice = 360.f / QAM_AMP_POINTS;
  float QAM_P = 30;
  // float QAM_P[QAM_SIZE]   = {10, 20, 30, 40, 50};
  static int QAM_index = 0;


  if (!motor->isEnabled) return;

  if (GetInterval_ms(1000)) {
    motor->ref += Amp_slice ;
    if (motor->ref >= 360.f * 0.99)
      motor->ref = 360.f * 0.01;

    motor->PID_iVelDegree.P = QAM_P;
    // QAM_index++;
    // if (QAM_index >= 5) QAM_index = 0;
  }
}
int start_run = 0;
void QAM_MeanAmpByUart(Motor_t *motor) {

  float QAM_angle[QAM_AMP_POINTS];
  float Amp_slice = 360.f / QAM_AMP_POINTS;
  float QAM_P = 20;
  // float QAM_P[QAM_SIZE]     = {10, 20, 30, 40, 50};
  static int QAM_index = 0;


  if (!motor->isEnabled) return;
  // if (UART_GetCMD("start")) {
  //   // printf("start motor\n");
  //   start_run = 1;
  // }
  // if (UART_GetCMD("stop")) {
  //   start_run = 0;
  // }
  if (strcmp(s_line_buf, "a") == 0) start_run = 1;

  if (start_run) {
    if (GetInterval_ms(500)) {
      motor->ref += Amp_slice ;
      if (motor->ref >= 360.f * 0.99)
        motor->ref = 360.f * 0.01;

      motor->PID_iVelDegree.P = QAM_P;
      // QAM_index++;
      // if (QAM_index >= 5) QAM_index = 0;
    }
  }
}

void QAM_example(void) {
  printf("===================================\r\n");
  printf("             QAM Debug             \r\n");
  printf("===================================\r\n");
  Motor_InitConfig(&motor);
  motor.ref = 0;
  motor.PID_iVelDegree.P = 0;

  while (1) {

    QAM_run(&motor);
    Motor_move(&motor, motor.ref);

  }

}
int isInitialized = 0;
int Position_init(Motor_t* motor, float position_init) {
  // motor->ref = 360.0f * 0.99f;
  if (!motor->isEnabled) return 0;
  if (!isInitialized) {
    motor->ref = position_init;
    isInitialized = 1;
  }
  return 1;
}
void QAM_Amp_Test(void) {

  printf("===================================\r\n");
  printf("             QAM Debug             \r\n");
  printf("===================================\r\n");
  Motor_InitConfig(&motor);
  motor.ref = 0;
  motor.PID_iVelDegree.P = 0;

  while (1) {
    // Position_init(&motor, 360*0.01);
    // if (isInitialized) {
    QAM_MeanAmpByUart(&motor);
    // }
    Motor_move(&motor, motor.ref);

  }
}

void test_uart(void) {
  UART_RxIT_Start(&CONFIG_UART_PRINTF);
  while (1) {
    if (UART_GetCMD_Once("start")) {
      printf("hello\n");
    }
  }

}
void QAM_Test0(void) {
  printf("===================================\r\n");
  printf("             QAM Debug             \r\n");
  printf("===================================\r\n");
  Motor_InitConfig(&motor);
  motor.ref = 0;
  motor.PID_iVelDegree.P = 0;

  while (1) {

    QAM_run4QAM(&motor);
    Motor_move(&motor, motor.ref);

  }

}
void QAM_main(void) {
  UART_RxIT_Start(&CONFIG_UART_PRINTF);
  // QAM_Amp_Test();
  // test_uart();

  QAM_Test0();

}
