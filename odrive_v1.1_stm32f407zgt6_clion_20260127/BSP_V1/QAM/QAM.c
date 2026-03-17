//
// Created by xwj on 1/21/26.
//

#include "QAM.h"
#include <stdio.h>

extern Motor_t motor;
#define QAM_SIZE 5
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
void QAM_main(void) {

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
