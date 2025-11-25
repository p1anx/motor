//
// Created by xwj on 11/22/25.
//

#include "stm32h7_dma.h"
#include "stm32_hal.h"

DMA_BUFFER char txDMA_buffer[256];
// DMA_BUFFER uint8_t txDMA_VOFA_Buffer[256];

DMA_BUFFER uint8_t tempData[8] = {0, 0, 0, 0, 0, 0, 0x80, 0x7F};
