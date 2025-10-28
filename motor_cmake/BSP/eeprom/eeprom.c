//
// Created by xwj on 10/15/25.
//

#include "eeprom.h"

#include "at24c02.h"

uint8_t eeprom_ReadFloat(uint16_t ReadAddr, float *pData) {
    return AT24C02_ReadFloat(ReadAddr, pData);
}
uint8_t eeprom_WriteFloat(uint16_t ReadAddr, float Data) {
    return AT24C02_WriteFloat(ReadAddr, Data);
}
