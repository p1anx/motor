#include "mymain.h"
// #include  "stm32_hal.h"
// #include "motor_2804.h"
// #include "motor_config.h"

#include "stm32f4xx_hal.h"

#include <stdio.h>

// extern void ads1256_main(void);
// extern void test_as5600();
// extern void test_motor(void);
extern void debug_main(void);

int mymain(void)
{
    // motor_2804_currentLoopBandwith();
#ifdef Debug
    debug_main();


#elifdef Release
    printf("hello release\n");

#endif
    // motor_2804_currentVelocityAngleLoopBandwith();
    // motor_2804_currentVelocityLoopBandwith();


}

