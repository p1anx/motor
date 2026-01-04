#include "mymain.h"
#include  "stm32_hal.h"
#include "motor_2804.h"
#include "motor_config.h"

extern void ads1256_main(void);
extern void test_as5600();
extern void test_motor(void);
extern void test_adc_0(void);

#if CONFIG_FOR_MOTOR2804
int mymain(void)
{
    motor_2804_currentOpenLoopBandwith();
    // motor_2804_currentLoopBandwith();
    // motor_2804_currentVelocityAngleLoopBandwith();
    // motor_2804_currentVelocityLoopBandwith();


}
#endif
