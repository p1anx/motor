#ifndef __QAM_H
#define __QAM_H

#include "motor.h"
typedef struct QAM_t QAM_t;
struct QAM_t
{
    float angle16[16];
    float angle4[4];
};

void qam_init(QAM_t *qam);
void qam16_sequence(QAM_t *qam);
void qam4_sequence(Motor_t *motor, QAM_t *qam);
void qam_loop(Motor_t *motor, QAM_t *qam, int update_ms);
#endif // !__QAM_H
