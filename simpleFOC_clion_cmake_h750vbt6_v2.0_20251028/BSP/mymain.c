#include "mymain.h"
#include  "stm32_hal.h"

extern void ads1256_main(void);
extern void test_as5600();
extern void test_motor(void);

int mymain(void)
{
    // test_main();
    //1.
    // ads1256_main();
    //2.
    // test_as5600();
    //3.
    test_motor();

}
