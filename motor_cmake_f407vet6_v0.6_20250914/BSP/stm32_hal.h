#ifndef __MY_STM32_HAL_H
#define __MY_STM32_HAL_H

/* 根据预定义的宏来包含相应的头文件 */
#undef DEBUG

#ifdef STM32F103xx
#include "stm32f1xx_hal.h"
#define MCU_SERIES "STM32F1"
#define APB1_MAX_FREQ 36000000UL
#define APB2_MAX_FREQ 72000000UL

#elif defined(STM32F407xx) || defined(STM32F405xx)
#include "stm32f4xx_hal.h"
#define MCU_SERIES "STM32F4"
#define APB1_MAX_FREQ 42000000UL
#define APB2_MAX_FREQ 84000000UL

#elif defined(STM32F767xx) || defined(STM32F746xx)
#include "stm32f7xx_hal.h"
#define MCU_SERIES "STM32F7"
#define APB1_MAX_FREQ 54000000UL
#define APB2_MAX_FREQ 108000000UL

#elif defined(STM32H743xx) || defined(STM32H750xx)
#include "stm32h7xx_hal.h"
#define MCU_SERIES "STM32H7"
#define APB1_MAX_FREQ 120000000UL
#define APB2_MAX_FREQ 120000000UL

#elif defined(STM32L476xx) || defined(STM32L432xx)
#include "stm32l4xx_hal.h"
#define MCU_SERIES "STM32L4"
#define APB1_MAX_FREQ 80000000UL
#define APB2_MAX_FREQ 80000000UL

#elif defined(STM32G474xx) || defined(STM32G431xx)
#include "stm32g4xx_hal.h"
#define MCU_SERIES "STM32G4"
#define APB1_MAX_FREQ 170000000UL
#define APB2_MAX_FREQ 170000000UL

#else
#error "未支持的STM32系列，请添加相应的头文件包含"
#endif

/* 通用的宏定义 */
#define MCU_IDENTIFIER MCU_SERIES

/* 版本信息 */
#define HAL_WRAPPER_VERSION_MAJOR 1
#define HAL_WRAPPER_VERSION_MINOR 0
#define HAL_WRAPPER_VERSION_PATCH 0

/* 通用的调试宏 */
#ifdef DEBUG
// #define DEBUG_PRINT(fmt, ...) printf("[%s] " fmt "\n", MCU_SERIES, ##__VA_ARGS__)
#define DEBUG_PRINT(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...)
#endif

void delay_ms(int ms);
#endif /* __STM32_HAL_H */
