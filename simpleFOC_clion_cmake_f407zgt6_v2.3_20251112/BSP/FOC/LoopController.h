//
// Created by xwj on 11/12/25.
//

#ifndef STM32H750VBT6_CMAKE_LOOPCONTROLLER_H
#define STM32H750VBT6_CMAKE_LOOPCONTROLLER_H
// 多速率控制系统
#include <stdbool.h>

typedef struct  MultiRate_Controller_t MultiRate_Controller_t;

struct  MultiRate_Controller_t{
    int current_counter;    // 电流环计数器
    int speed_counter;      // 速度环计数器
    int pos_counter;        // 位置环计数器

    int current_period;     // 电流环周期（相对于最高速率）
    int speed_period;       // 速度环周期
    int pos_period;         // 位置环周期

    // 各环路使能标志
    bool current_enable;
    bool speed_enable;
    bool pos_enable;
};

bool execute_multirate_control(MultiRate_Controller_t* mr);

void init_multirate_controller(MultiRate_Controller_t* mr,
                              float current_freq, float speed_freq, float pos_freq);
    #endif // STM32H750VBT6_CMAKE_LOOPCONTROLLER_H
