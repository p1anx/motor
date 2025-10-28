//
// Created by xwj on 10/15/25.
//

#ifndef MOTOR_CMAKE_EEPROM_H
#define MOTOR_CMAKE_EEPROM_H

#include <stdint.h>

#define EEPROM_LAST_ANGLE_ADDR 0

uint8_t eeprom_ReadFloat(uint16_t ReadAddr, float *pData);
uint8_t eeprom_WriteFloat(uint16_t ReadAddr, float Data);
#endif //MOTOR_CMAKE_EEPROM_H