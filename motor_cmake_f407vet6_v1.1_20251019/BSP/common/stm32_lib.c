//
// Created by xwj on 10/20/25.
//

#include "stm32_lib.h"
#include "common.h"
// float UART_getFloatFromCmd(char cmd[], UART_HandleTypeDef *huart)
// {
//     uint8_t rxBuffer;
//     getFloatFromUART(cmd, rxBuffer);
//
// }

void system_reset(void)
{
    __set_FAULTMASK(1);
    NVIC_SystemReset();
}