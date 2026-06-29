//
// Created by xwj on 1/21/26.
//

#include "QAM.h"
#include "config.h"

#include "my_uart.h"
#include <stdio.h>
#include <string.h>
#include <tim.h>
#include "timer.h"

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
// This is 8 qam
void QAM_run4QAM(Motor_t *motor) {
  float QAM_angle[QAM_SIZE] = {33.5, 275.4, 97.5, 214.3};
  // float QAM_P[QAM_SIZE]     = {30, 20, 20, 20};
  // int qam_time[QAM_SIZE*2] = {2000, 2000, 2000, 2000, 2000, 1000, 3000, 2000};
  // int qam_time[QAM_SIZE*2] = {2000, 2000, 2000, 2000, 2000, 1500, 2500, 2000};
  // int qam_time[QAM_SIZE*2] = {2000, 2000, 2000, 2000, 2000, 750, 3250, 2000};

  float QAM_P[QAM_SIZE]     = {50, 50, 50, 50};
  int qam_time[QAM_SIZE*2] = {2000, 1250, 2750, 1250, 2750, 2000, 2000, 2000};
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
//
#define  QAM16_SIZE 4
void QAM_run16QAM_v0(Motor_t *motor) {
  // float QAM_angle[QAM16_SIZE] = {17.9f, 282.4f, 46.2f, 255.7f, 69.0f, 231.6f, 95.3f, 203.6f};
  float QAM_angle[QAM16_SIZE] = {17.9f, 46.2f, 69.0f, 95.3f};
  // int qam_time[QAM_SIZE*2] = {2000, 2000, 2000, 2000, 2000, 1000, 3000, 2000};
  // int qam_time[QAM_SIZE*2] = {2000, 2000, 2000, 2000, 2000, 1500, 2500, 2000};
  // int qam_time[QAM_SIZE*2] = {2000, 2000, 2000, 2000, 2000, 750, 3250, 2000};

   // float QAM_P[QAM16_SIZE]     = {50, 50, 50, 50, 50};
   float QAM_P[QAM16_SIZE];
  for (int i = 0;i < QAM16_SIZE; i++) {
    QAM_P[i] = 50;
  }
  // int qam_time[QAM16_SIZE*2] = {2000, 1250, 2750, 1250, 2750, 2000, 2000, 2000};
  // int qam_time[QAM16_SIZE*2] = {1250, 2750, 1250, 2750, 2000, 2000, 2000, 2000};
  int qam_time[QAM16_SIZE*4];
  int phase_a[QAM16_SIZE] = {1250, 1500, 2750, 2500};
  int phase_b[QAM16_SIZE] = {2750, 2500, 1250, 1500};
  int phase_c[QAM16_SIZE] = {1500, 1250, 2500, 2750};
  int phase_d[QAM16_SIZE] = {2500, 2750, 1500, 1250};
  for (int i = 0;i < QAM16_SIZE;i++) {
    qam_time[4*i] =   phase_a[i];
    qam_time[4*i+1] = phase_b[i];
    qam_time[4*i+2] = phase_c[i];
    qam_time[4*i+3] = phase_d[i];
  }

  static int QAM_index = 0;
  static int t_index = 0;


  if (!motor->isEnabled) return;


  if (GetInterval_ms(qam_time[t_index])) {
    motor->ref = QAM_angle[QAM_index] ;
    // motor->PID_iVelDegree.P = QAM_P[QAM_index];
    motor->PID_iVelDegree.P = 50;
    QAM_index++;
    if (QAM_index >= QAM16_SIZE) QAM_index = 0;
    t_index++;
    if (t_index >= QAM16_SIZE*4) {
      t_index = 0;
    }
  }
}

void QAM_run16QAM(Motor_t *motor) {
  // float QAM_angle[QAM16_SIZE] = {17.9f, 282.4f, 46.2f, 255.7f, 69.0f, 231.6f, 95.3f, 203.6f};
  float QAM_angle[QAM16_SIZE] = {17.9f, 46.2f, 69.0f, 95.3f};
  // int qam_time[QAM_SIZE*2] = {2000, 2000, 2000, 2000, 2000, 1000, 3000, 2000};
  // int qam_time[QAM_SIZE*2] = {2000, 2000, 2000, 2000, 2000, 1500, 2500, 2000};
  // int qam_time[QAM_SIZE*2] = {2000, 2000, 2000, 2000, 2000, 750, 3250, 2000};

  // float QAM_P[QAM16_SIZE]     = {50, 50, 50, 50, 50};
  float QAM_P[QAM16_SIZE];
  for (int i = 0;i < QAM16_SIZE; i++) {
    QAM_P[i] = 50;
  }
  // int qam_time[QAM16_SIZE*2] = {2000, 1250, 2750, 1250, 2750, 2000, 2000, 2000};
  // int qam_time[QAM16_SIZE*2] = {1250, 2750, 1250, 2750, 2000, 2000, 2000, 2000};
  int qam_time[QAM16_SIZE*4];
  int phase_a[QAM16_SIZE] = {250, 500, 750, 0};
  int phase_b[QAM16_SIZE] = {750, 500, 1250, 1500};
  int phase_c[QAM16_SIZE] = {1500, 1250, 2500, 2750};
  int phase_d[QAM16_SIZE] = {2500, 2750, 1500, 1250};
  for (int i = 0;i < QAM16_SIZE;i++) {
    qam_time[4*i] =   phase_a[i];
    qam_time[4*i+1] = phase_b[i];
    qam_time[4*i+2] = phase_c[i];
    qam_time[4*i+3] = phase_d[i];
  }

  static int QAM_index = 0;
  static int t_index = 0;


  if (!motor->isEnabled) return;


  if (GetInterval_ms(qam_time[t_index])) {
    motor->ref = QAM_angle[QAM_index] ;
    // motor->PID_iVelDegree.P = QAM_P[QAM_index];
    motor->PID_iVelDegree.P = 50;
    QAM_index++;
    if (QAM_index >= QAM16_SIZE) QAM_index = 0;
    t_index++;
    if (t_index >= QAM16_SIZE*4) {
      t_index = 0;
    }
  }
}

void QAM_runSinAmplitude(Motor_t *motor) {
  // float QAM_angle[QAM16_SIZE] = {17.9f, 282.4f, 46.2f, 255.7f, 69.0f, 231.6f, 95.3f, 203.6f};
  float QAM_angle[QAM16_SIZE] = {17.9f, 46.2f, 69.0f, 95.3f};
  float QAM_P[QAM16_SIZE];
  for (int i = 0;i < QAM16_SIZE; i++) {
    QAM_P[i] = 50;
  }
  // int qam_time[QAM16_SIZE*2] = {2000, 1250, 2750, 1250, 2750, 2000, 2000, 2000};
  // int qam_time[QAM16_SIZE*2] = {1250, 2750, 1250, 2750, 2000, 2000, 2000, 2000};
  int qam_time[QAM16_SIZE*4];
  int phase_a[QAM16_SIZE] = {250, 500, 750, 0};
  int phase_b[QAM16_SIZE] = {750, 500, 1250, 1500};
  int phase_c[QAM16_SIZE] = {1500, 1250, 2500, 2750};
  int phase_d[QAM16_SIZE] = {2500, 2750, 1500, 1250};
  for (int i = 0;i < QAM16_SIZE;i++) {
    qam_time[4*i] =   phase_a[i];
    qam_time[4*i+1] = phase_b[i];
    qam_time[4*i+2] = phase_c[i];
    qam_time[4*i+3] = phase_d[i];
  }

  static int QAM_index = 0;
  static int t_index = 0;


  if (!motor->isEnabled) return;


  if (GetInterval_ms(qam_time[t_index])) {
    motor->ref = QAM_angle[QAM_index] ;
    // motor->PID_iVelDegree.P = QAM_P[QAM_index];
    motor->PID_iVelDegree.P = 50;
    QAM_index++;
    if (QAM_index >= QAM16_SIZE) QAM_index = 0;
    t_index++;
    if (t_index >= QAM16_SIZE*4) {
      t_index = 0;
    }
  }
}
extern int qam_phase0;
extern int qam_time[Q_SIZE];
extern int flag_phase[Q_SIZE];
void QAM_run4QAM_ok(Motor_t *motor) {
  float QAM_angle[Q_SIZE] = {282.4f, 17.9f,282.4f, 17.9f,282.4f, 17.9f,282.4f, 17.9f};
  float QAM_P[Q_SIZE];
  for (int i = 0;i <Q_SIZE; i++) {
    QAM_P[i] = 50;
  }
  // int qam_time[QAM16_SIZE*4];

  static int QAM_index = 0;
  static int t_index = 0;


  if (!motor->isEnabled) return;
  uint32_t t_start = HAL_GetTick();
  uint32_t t_period = 1000;

  static int isFirst = 1;
  if (qam_phase0 == flag_phase[QAM_index]) {
    motor->ref = QAM_angle[QAM_index] ;
    // motor->PID_iVelDegree.P = QAM_P[QAM_index];
    motor->PID_iVelDegree.P = 50;
    QAM_index++;
    if (QAM_index >=Q_SIZE) QAM_index = 0;
    t_index++;
    if (t_index >=Q_SIZE*4) {
      t_index = 0;
    }
  }
}
void QAM_run4QAM_ok2(Motor_t *motor) {
  float QAM_angle[Q_SIZE] = {282.4f, 17.9f,282.4f, 17.9f,282.4f, 17.9f,282.4f, 17.9f};
  float QAM_P[Q_SIZE];
  for (int i = 0;i <Q_SIZE; i++) {
    QAM_P[i] = 50;
  }
  // int qam_time[QAM16_SIZE*4];

  static int QAM_index = 0;
  static int t_index = 0;


  if (!motor->isEnabled) return;
  uint32_t t_start = HAL_GetTick();
  uint32_t t_period = 1000;

  static int isFirst = 1;
  if (qam_phase0 == flag_phase[QAM_index]) {
      motor->ref = QAM_angle[QAM_index] ;
      // motor->PID_iVelDegree.P = QAM_P[QAM_index];
      motor->PID_iVelDegree.P = 50;
      QAM_index++;
      if (QAM_index >=Q_SIZE) QAM_index = 0;
      t_index++;
      if (t_index >=Q_SIZE*4) {
        t_index = 0;
      }
  }
}

  // float QAM_angle[QAM16_SIZE] = {17.9f, 282.4f, 46.2f, 255.7f, 69.0f, 231.6f, 95.3f, 203.6f};
#define QAM16_NUM 16
#define RAWS 4
#define COLS 8
void QAM_run16QAM0(Motor_t *motor) {
  // printf("in function\n");
  float bias = 3.6f;
  float QAM_AMP[4] = {20.6f+bias, 43.1f+bias, 64.7f+bias, 104.8f+bias};
  float QAM_angle0[Q_SIZE] = {17.9f, QAM_AMP[0], 17.9f,QAM_AMP[0], 17.9f,QAM_AMP[0], 17.9f,QAM_AMP[0]};
  float QAM_angle1[Q_SIZE] = {17.9f, QAM_AMP[1], 17.9f,QAM_AMP[1], 17.9f,QAM_AMP[1], 17.9f,QAM_AMP[1]};
  float QAM_angle2[Q_SIZE] = {17.9f, QAM_AMP[2], 17.9f,QAM_AMP[2], 17.9f,QAM_AMP[2], 17.9f,QAM_AMP[2]};
  float QAM_angle3[Q_SIZE] = {17.9f, QAM_AMP[3], 17.9f,QAM_AMP[3], 17.9f,QAM_AMP[3], 17.9f,QAM_AMP[3]};
  //float QAM_angle0[Q_SIZE] = {282.4f, 17.9f,282.4f, 17.9f,282.4f, 17.9f,282.4f, 17.9f};
  //float QAM_angle1[Q_SIZE] = {255.7f, 17.9f,255.7f, 17.9f,255.7f, 17.9f,255.7f, 17.9f};
  //float QAM_angle2[Q_SIZE] = {231.6f, 17.9f,231.6f, 17.9f,231.6f, 17.9f,231.6f, 17.9f};
  //float QAM_angle3[Q_SIZE] = {203.6f, 17.9f,203.6f, 17.9f,203.6f, 17.9f,203.6f, 17.9f};
  float QAM_ANGLE[RAWS][COLS];
  for (int i = 0; i < COLS; i++) {
    QAM_ANGLE[0][i] = QAM_angle0[i];
    QAM_ANGLE[1][i] = QAM_angle1[i];
    QAM_ANGLE[2][i] = QAM_angle2[i];
    QAM_ANGLE[3][i] = QAM_angle3[i];
    // printf("angle[0][%d] = %f\n",i, QAM_angle0[i]);
    // printf("angle[1][%d] = %f\n",i, QAM_angle1[i]);
    // printf("angle[2][%d] = %f\n",i, QAM_angle2[i]);
    // printf("angle[3][%d] = %f\n",i, QAM_angle3[i]);
  }
  // for (int i = 0; i < RAWS; i++) {
  //   for (int j = 0; j < COLS; j++) {
  //     printf("%.2f,", QAM_ANGLE[i][j]);
  //   }
  //   printf("\n");
  // }
  float QAM_P[Q_SIZE];
  for (int i = 0;i <Q_SIZE; i++) {
    QAM_P[i] = 50;
  }
  // int qam_time[QAM16_SIZE*4];

  static int QAM_index = 0;
  static int QAM_AMP_index = 0;
  static int t_index = 0;


  if (!motor->isEnabled) return;
  uint32_t t_start = HAL_GetTick();
  uint32_t t_period = 1000;

  static int isFirst = 1;
  if (qam_phase0 == flag_phase[QAM_index]) {
    motor->ref = QAM_ANGLE[QAM_AMP_index][QAM_index] ;
    // motor->PID_iVelDegree.P = QAM_P[QAM_index];
    motor->PID_iVelDegree.P = 50;
  // printf("angle[%d][%d] = %f\n",QAM_AMP_index, QAM_index, motor->ref);
    QAM_index++;
    if (QAM_index >=Q_SIZE) {
      QAM_index = 0;
      QAM_AMP_index++;
      if (QAM_AMP_index > 3) QAM_AMP_index = 0;
    }
  }
}
#define QAM64_NUM 64
#define RAWS64 8
#define COLS64 16
extern int flag_phase_64qam[16];
void QAM_run64QAM0(Motor_t *motor) {
  // printf("in function\n");
  float bias = 3.6f;
  //float QAM_AMP[4] = {20.6f+bias, 43.1f+bias, 64.7f+bias, 104.8f+bias};
  float QAM_AMP[8] = {24.5f, 35.6f, 46.7f, 57.9f, 68.9f, 80.f, 91.2f, 108.4f};
  float QAM_angle0[QAM64_NUM/4] = {17.9f, QAM_AMP[0], 17.9f,QAM_AMP[0], 17.9f,QAM_AMP[0], 17.9f,QAM_AMP[0], 17.9f, QAM_AMP[0], 17.9f,QAM_AMP[0], 17.9f,QAM_AMP[0], 17.9f,QAM_AMP[0]};
  float QAM_angle1[QAM64_NUM/4] = {17.9f, QAM_AMP[1], 17.9f,QAM_AMP[1], 17.9f,QAM_AMP[1], 17.9f,QAM_AMP[1], 17.9f, QAM_AMP[1], 17.9f,QAM_AMP[1], 17.9f,QAM_AMP[1], 17.9f,QAM_AMP[1]};
  float QAM_angle2[QAM64_NUM/4] = {17.9f, QAM_AMP[2], 17.9f,QAM_AMP[2], 17.9f,QAM_AMP[2], 17.9f,QAM_AMP[2], 17.9f, QAM_AMP[2], 17.9f,QAM_AMP[2], 17.9f,QAM_AMP[2], 17.9f,QAM_AMP[2]};
  float QAM_angle3[QAM64_NUM/4] = {17.9f, QAM_AMP[3], 17.9f,QAM_AMP[3], 17.9f,QAM_AMP[3], 17.9f,QAM_AMP[3], 17.9f, QAM_AMP[3], 17.9f,QAM_AMP[3], 17.9f,QAM_AMP[3], 17.9f,QAM_AMP[3]};
  float QAM_angle4[QAM64_NUM/4] = {17.9f, QAM_AMP[4], 17.9f,QAM_AMP[4], 17.9f,QAM_AMP[4], 17.9f,QAM_AMP[4], 17.9f, QAM_AMP[4], 17.9f,QAM_AMP[4], 17.9f,QAM_AMP[4], 17.9f,QAM_AMP[4]};
  float QAM_angle5[QAM64_NUM/4] = {17.9f, QAM_AMP[5], 17.9f,QAM_AMP[5], 17.9f,QAM_AMP[5], 17.9f,QAM_AMP[5], 17.9f, QAM_AMP[5], 17.9f,QAM_AMP[5], 17.9f,QAM_AMP[5], 17.9f,QAM_AMP[5]};
  float QAM_angle6[QAM64_NUM/4] = {17.9f, QAM_AMP[6], 17.9f,QAM_AMP[6], 17.9f,QAM_AMP[6], 17.9f,QAM_AMP[6], 17.9f, QAM_AMP[6], 17.9f,QAM_AMP[6], 17.9f,QAM_AMP[6], 17.9f,QAM_AMP[6]};
  float QAM_angle7[QAM64_NUM/4] = {17.9f, QAM_AMP[7], 17.9f,QAM_AMP[7], 17.9f,QAM_AMP[7], 17.9f,QAM_AMP[7], 17.9f, QAM_AMP[7], 17.9f,QAM_AMP[7], 17.9f,QAM_AMP[7], 17.9f,QAM_AMP[7]};
  float QAM_ANGLE[RAWS64][COLS64];
  for (int i = 0; i < COLS64; i++) {
    QAM_ANGLE[0][i] = QAM_angle0[i];
    QAM_ANGLE[1][i] = QAM_angle1[i];
    QAM_ANGLE[2][i] = QAM_angle2[i];
    QAM_ANGLE[3][i] = QAM_angle3[i];
    QAM_ANGLE[4][i] = QAM_angle4[i];
    QAM_ANGLE[5][i] = QAM_angle5[i];
    QAM_ANGLE[6][i] = QAM_angle6[i];
    QAM_ANGLE[7][i] = QAM_angle7[i];
  }

  static int QAM_index = 0;
  static int QAM_AMP_index = 0;

  if (!motor->isEnabled) return;
  uint32_t t_start = HAL_GetTick();
  uint32_t t_period = 1000;

  static int isFirst = 1;
  if (qam_phase0 == flag_phase_64qam[QAM_index]) {
    motor->ref = QAM_ANGLE[QAM_AMP_index][QAM_index] ;
    // motor->PID_iVelDegree.P = QAM_P[QAM_index];
    motor->PID_iVelDegree.P = 50;
  // printf("angle[%d][%d] = %f\n",QAM_AMP_index, QAM_index, motor->ref);
    QAM_index++;
    if (QAM_index >= COLS64) {
      QAM_index = 0;
      QAM_AMP_index++;
      if (QAM_AMP_index > RAWS64 - 1) QAM_AMP_index = 0;
    }
  }
}
//标准16QAM的四分之一星座图
#if 0
#define QAM16_NUM 16
#define RAWS16 4
#define COLS16 8
extern int flag_phase_16qam[8];
void QAM_run16QAM_standard(Motor_t *motor) {
  // printf("in function\n");
  float bias = 3.6f;
  //float QAM_AMP[4] = {20.6f+bias, 43.1f+bias, 64.7f+bias, 104.8f+bias};
  float QAM_AMP[4] = {39.27f, 68.59f, 68.59f, 110.f};
  //float QAM_angle0[COLS16] = {17.9f, QAM_AMP[0], 17.9f,QAM_AMP[0], 17.9f,QAM_AMP[0], 17.9f,QAM_AMP[0]};
  //float QAM_angle1[COLS16] = {17.9f, QAM_AMP[1], 17.9f,QAM_AMP[1], 17.9f,QAM_AMP[1], 17.9f,QAM_AMP[1]};
  //float QAM_angle2[COLS16] = {17.9f, QAM_AMP[2], 17.9f,QAM_AMP[2], 17.9f,QAM_AMP[2], 17.9f,QAM_AMP[2]};
  //float QAM_angle3[COLS16] = {17.9f, QAM_AMP[3], 17.9f,QAM_AMP[3], 17.9f,QAM_AMP[3], 17.9f,QAM_AMP[3]};
  float QAM_angle0[COLS16] = {17.9f, QAM_AMP[0], 17.9f,QAM_AMP[1], 17.9f,QAM_AMP[2], 17.9f,QAM_AMP[3]};
  float QAM_angle1[COLS16] = {17.9f, QAM_AMP[0], 17.9f,QAM_AMP[1], 17.9f,QAM_AMP[2], 17.9f,QAM_AMP[3]};
  float QAM_angle2[COLS16] = {17.9f, QAM_AMP[0], 17.9f,QAM_AMP[1], 17.9f,QAM_AMP[2], 17.9f,QAM_AMP[3]};
  float QAM_angle3[COLS16] = {17.9f, QAM_AMP[0], 17.9f,QAM_AMP[1], 17.9f,QAM_AMP[2], 17.9f,QAM_AMP[3]};
  float QAM_ANGLE[RAWS16][COLS16];
  for (int i = 0; i < COLS16; i++) {
    QAM_ANGLE[0][i] = QAM_angle0[i];
    QAM_ANGLE[1][i] = QAM_angle1[i];
    QAM_ANGLE[2][i] = QAM_angle2[i];
    QAM_ANGLE[3][i] = QAM_angle3[i];
  }

  static int QAM_index = 0;
  static int QAM_AMP_index = 0;

  if (!motor->isEnabled) return;
  uint32_t t_start = HAL_GetTick();
  uint32_t t_period = 1000;

  static int isFirst = 1;
  if (qam_phase0 == flag_phase_16qam[QAM_index]) {
    motor->ref = QAM_ANGLE[QAM_AMP_index][QAM_index] ;
    motor->PID_iVelDegree.P = 50;
    QAM_index++;
    if (QAM_index >= COLS16) {
      QAM_index = 0;
      QAM_AMP_index++;
      if (QAM_AMP_index > RAWS16 - 1) QAM_AMP_index = 0;
    }
  }
}
#endif


#define QAM16_NUM 16
#define RAWS16 4
#define COLS16 8
#define QAM_PHASE_NUM 32
extern int flag_phase_16qam[32];
void QAM_run16QAM_standard(Motor_t *motor) {
  // printf("in function\n");
  float bias = 3.6f;
  //float QAM_AMP[4] = {20.6f+bias, 43.1f+bias, 64.7f+bias, 104.8f+bias};
  float QAM_AMP[16] = {39.27f, 68.59f, 68.59f, 110.f, 68.59f, 39.27f,68.59f, 110.f, 68.59f, 39.27f,68.59f, 110.f, 68.59f, 39.27f,68.59f, 110.f};

  float QAM_angle0[COLS16] = {17.9f, QAM_AMP[0], 17.9f,QAM_AMP[1], 17.9f,QAM_AMP[2], 17.9f,QAM_AMP[3]};
  float QAM_angle1[COLS16] = {17.9f, QAM_AMP[4], 17.9f,QAM_AMP[5], 17.9f,QAM_AMP[6], 17.9f,QAM_AMP[7]};
  float QAM_angle2[COLS16] = {17.9f, QAM_AMP[8], 17.9f,QAM_AMP[9], 17.9f,QAM_AMP[10], 17.9f,QAM_AMP[11]};
  float QAM_angle3[COLS16] = {17.9f, QAM_AMP[12], 17.9f,QAM_AMP[13], 17.9f,QAM_AMP[14], 17.9f,QAM_AMP[15]};
  float QAM_ANGLE[RAWS16][COLS16];
  for (int i = 0; i < COLS16; i++) {
    QAM_ANGLE[0][i] = QAM_angle0[i];
    QAM_ANGLE[1][i] = QAM_angle1[i];
    QAM_ANGLE[2][i] = QAM_angle2[i];
    QAM_ANGLE[3][i] = QAM_angle3[i];
  }

  static int QAM_index = 0;
  static int QAM_AMP_index = 0;
  static int QAM_PHA_index = 0;

  if (!motor->isEnabled) return;
  uint32_t t_start = HAL_GetTick();
  uint32_t t_period = 1000;

  static int isFirst = 1;
  if (qam_phase0 == flag_phase_16qam[QAM_PHA_index]) {
    motor->ref = QAM_ANGLE[QAM_AMP_index][QAM_index] ;
    motor->PID_iVelDegree.P = 50;
    QAM_index++;
    QAM_PHA_index++;
    if (QAM_PHA_index >= QAM_PHASE_NUM) {
      QAM_PHA_index =0;
    }
    if (QAM_index >= COLS16) {
      QAM_index = 0;
      QAM_AMP_index++;
      if (QAM_AMP_index > RAWS16 - 1) QAM_AMP_index = 0;
    }
  }
}

#define QAM_AMP_POINTS 100
void QAM_MeanAmp0(Motor_t *motor){
  float QAM_angle[QAM_AMP_POINTS];
  float Amp_slice = 360.f / QAM_AMP_POINTS;
  float QAM_P = 30;
  // float QAM_P[QAM_SIZE]   = {10, 20, 30, 40, 50};
  static int QAM_index = 0;


  if (!motor->isEnabled) return;

  if (GetInterval_ms(2000)) {
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
  // if (strcmp(s_line_buf, "a") == 0) start_run = 1;
  // int start_run = 1;
  static int isFirst = 1;
  static int isStop = 0;
  if (isFirst) {
    isFirst = 0;
    delay_ms(10000);
  }

  if (1) {
    if (GetInterval_ms(1000)) {
      // if (!isStop) {
        // motor->ref += Amp_slice ;
      // }
      motor->ref += Amp_slice ;
      if (motor->ref >= 360.f * 0.99)
        // isStop = 1;
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
void QAM_Test1(void) {
  printf("testing\n");
  while (1) {
    QAM_run16QAM0(&motor);
  }

}
void QAM16_Test(void) {
  printf("===================================\r\n");
  printf("             QAM Debug             \r\n");
  printf("===================================\r\n");
  Motor_InitConfig(&motor);
  motor.ref = 0;
  motor.PID_iVelDegree.P = 0;

  while (1) {

    // QAM_run4QAM(&motor);
    // QAM_run4QAM_ok(&motor);
    // QAM_run8QAM(&motor);

    // QAM_run4QAM_ok2(&motor);
    QAM_run16QAM0(&motor);

    // QAM_run16QAM(&motor);
    Motor_move(&motor, motor.ref);

  }

}
void QAM64_Test(void) {
  printf("===================================\r\n");
  printf("             QAM Debug             \r\n");
  printf("===================================\r\n");
  Motor_InitConfig(&motor);
  motor.ref = 0;
  motor.PID_iVelDegree.P = 0;

  while (1) {

    QAM_run64QAM0(&motor);
    Motor_move(&motor, motor.ref);

  }

}
void QAM16_standard_Test(void) {
  printf("===================================\r\n");
  printf("             QAM Debug             \r\n");
  printf("===================================\r\n");
  Motor_InitConfig(&motor);
  motor.ref = 0;
  motor.PID_iVelDegree.P = 0;

  while (1) {

    QAM_run16QAM_standard(&motor);
    Motor_move(&motor, motor.ref);

  }

}

extern TIM_HandleTypeDef htim3;
void QAM_main(void) {
  UART_RxIT_Start(&CONFIG_UART_PRINTF);
  printf("tim test\n");
  HAL_TIM_Base_Start_IT(&htim3);

  QAM_Amp_Test();
  // test_uart();
  // QAM_Test1();

//1. 16QAM
//   QAM16_Test();

//2. 64QAM
//  QAM64_Test();
  //3.标准16QAM
  QAM16_standard_Test();
}