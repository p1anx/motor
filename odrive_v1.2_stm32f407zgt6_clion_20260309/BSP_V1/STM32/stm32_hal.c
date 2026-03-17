#include "stm32_hal.h"
#include <string.h>
#include <stdio.h>

extern char txDMA_buffer[256];
extern int TxCompleteFlag;
#define TIM_TIME_BASE TIM2

void delay_ms(int ms)
{
    HAL_Delay(ms);
}

uint32_t micros(void) {
    register uint32_t ms, cycle_cnt;
    do {
        ms = HAL_GetTick();
        cycle_cnt = TIM_TIME_BASE->CNT;
    } while (ms != HAL_GetTick());

    return (ms * 1000) + cycle_cnt;
}

// @brief: Busy wait delay for given amount of microseconds (us)
void delay_us(uint32_t us)
{
    uint32_t start = micros();
    while (micros() - start < (uint32_t) us) {
        __ASM("nop");
    }
}
uint32_t getMicros(void)
{
    // STM32实现示例
    return HAL_GetTick() * 1000 + (SysTick->LOAD - SysTick->VAL) / (SystemCoreClock / 1000000);
}

#include <stdint.h>

static uint32_t us_counter_base = 0;
static uint32_t last_cyccnt = 0;
static uint32_t cycles_per_us = 0;  // 系统时钟(MHz)

uint32_t getUs(void) {
    // 初始化 DWT
    if (!(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
        last_cyccnt = 0;
    }

    if (cycles_per_us == 0) {
        // 获取系统时钟频率，转换为每微秒的周期数
        cycles_per_us = HAL_RCC_GetSysClockFreq() / 1000000UL;
    }

    uint32_t current_cyccnt = DWT->CYCCNT;
    uint32_t diff = current_cyccnt - last_cyccnt;

    // 计算微秒差值
    uint32_t us_diff = diff / cycles_per_us;

    // 更新基准
    us_counter_base += us_diff;
    last_cyccnt = current_cyccnt;

    return us_counter_base;
}

// 重置计数器（可选）
void resetUsCounter(void) {
    us_counter_base = 0;
    last_cyccnt = DWT->CYCCNT;
}
