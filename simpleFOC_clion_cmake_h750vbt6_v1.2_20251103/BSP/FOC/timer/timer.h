#ifndef __TIMER_H
#define __TIMER_H

void timer_init(TIM_HandleTypeDef *tim, int update_ms);
void pid_update_timer(void);
#endif // !__TIMER_H
