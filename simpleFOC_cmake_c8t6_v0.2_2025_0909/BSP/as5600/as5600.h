#ifndef __AS5600_H
#define __AS5600_H
#include "i2c.h"
#include "mymain.h"

#define AS5600_ADDRESS 0x36
#define AS5600_ADDRESS_WR AS5600_ADDRESS << 1
#define AS5600_ADDRESS_RD (AS5600_ADDRESS << 1) | 0x1

void AS5600_test(void);
float getAngle(void);
int getCircle();
float getVelocity(void);
void test_velocity(void);
#endif
