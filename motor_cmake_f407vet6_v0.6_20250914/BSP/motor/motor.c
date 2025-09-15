
#include "motor.h"
#include "tb6612_init.h"
#include "types.h"

int motor_init(Motor_t *motor)
{
    tb6612_init(motor->tb6612);
    return 0;
}

int motor_linkTB6612(Motor_t *motor, TB6612_t *tb6612)
{
    motor->tb6612 = tb6612;
    return 0;
}
