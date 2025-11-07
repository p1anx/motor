#ifndef __AS5600_H
#define __AS5600_H
#include "i2c.h"
#include "mymain.h"
#include "encoder_as5600.h"

#define AS5600_ADDRESS 0x36
#define AS5600_ADDRESS_WR AS5600_ADDRESS << 1
#define AS5600_ADDRESS_RD (AS5600_ADDRESS << 1) | 0x1

#define AS5600_CW 0
#define AS5600_CCW_INCREASE -1


extern I2C_HandleTypeDef hi2c2;
#define as5600_i2c hi2c2

void AS5600_test(void);
float getAngle(void);
int getCircle(void);
float getVelocity(void);
void test_velocity(void);
float AS5600_getAngle(AS5600_t *as5600);
float AS5600_getVelocity(AS5600_t *as5600);

#endif
