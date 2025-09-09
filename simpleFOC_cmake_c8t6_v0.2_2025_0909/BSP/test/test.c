#include "test.h"
#include "as5600.h"
#include "led.h"
#include "open_loop_FOC.h"

void test_led(void) { led_blink(1000); }

void test_as5600_angle(void) {

  float angle = getAngle();
  printf("angle = %f\n", angle);
  HAL_Delay(100);
}

void test_torque(void) { setTorque(3, _PI_2); }
void test_velocityOpenLoop(void) { velocityOpenloop(1); }
