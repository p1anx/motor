//
// Created by xwj on 1/13/26.
//

#ifndef ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_UART_H
#define ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_UART_H

#include "stm32_hal.h"
// #include "config.h"

typedef struct UART_t UART_t;

struct UART_t {
  UART_HandleTypeDef huart;

};

#endif // ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_UART_H
