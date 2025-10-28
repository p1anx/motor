//
// Created by xwj on 10/15/25.
//

#include "test_main.h"
#include "test_at24c02.h"

extern void test_mt6835();
extern void test_motor(void);
extern void test_lis3mdl(void);

extern void test_esp8266();

void test_main(void)
{
    // 1.
    //  test_at24c02();
    // 2.
    //  test_mt6835();
    // 3. motor
    // test_motor();
    // 4.
    test_lis3mdl();
    // 5.
    //  test_esp8266();
}