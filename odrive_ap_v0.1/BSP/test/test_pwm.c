//
// Created by xwj on 11/18/25.
//

#include "DRV830X.h"
#include "pwm.h"

#include <stdio.h>

extern void test_adc_01();

void test_pwm(void) {
  DRV8301_CONFIG_t drv8301_config;
  drv8301_config.gain = 20;
  printf("test_pwm\n");
  DRV8301_Setup(&drv8301_config);
  // PWM6_Init(10e3, 4096);
  test_6PWM();
  while (1) {
    // test_adc_01();

  }

  // BLDCDriver_3PWM_Init(1000, 4096);
  // while (1) {
  //   delay_ms(1);
  // }

}
