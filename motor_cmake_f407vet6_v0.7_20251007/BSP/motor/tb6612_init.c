#include "tb6612_init.h"
#include "encoder.h"
#include "pwm.h"

int tb6612_init(TB6612_t *tb6612)
{
    pwm_init(tb6612->pwm);
    encoder_init(tb6612->encoder);
    return 0;
}
