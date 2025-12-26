//
// Created by xwj on 11/12/25.
//

#include "LoopController.h"

#include "BLDCMotor.h"

#include <stdio.h>

volatile MultiRate_Controller_t MultiRate_Controller;
// 初始化多速率控制
void init_multirate_controller(MultiRate_Controller_t* mr,
                              float current_freq, float speed_freq, float pos_freq) {
    // 假设最高速率为10kHz
    // float base_freq = 20000.0f;
    float base_freq = CONFIG_PWM_HZ;

    // 计算各环路相对于基频的周期
    mr->current_period = (int)(base_freq / current_freq);
    mr->speed_period = (int)(base_freq / speed_freq);
    mr->pos_period = (int)(base_freq / pos_freq);

    // 初始化计数器
    mr->current_counter = 0;
    mr->speed_counter = 0;
    mr->pos_counter = 0;

    printf("Multi-rate Control:\n");
    printf("Current loop period: %d cycles\n", mr->current_period);
    printf("Speed loop period: %d cycles\n", mr->speed_period);
    printf("Position loop period: %d cycles\n", mr->pos_period);
}

// 多速率控制执行函数
bool execute_multirate_control(MultiRate_Controller_t* mr) {
    // 每次调用时增加计数器
    mr->current_counter++;
    mr->speed_counter++;
    mr->pos_counter++;

    // 判断各环路是否执行
    mr->current_enable = (mr->current_counter >= mr->current_period);
    mr->speed_enable = (mr->speed_counter >= mr->speed_period);
    mr->pos_enable = (mr->pos_counter >= mr->pos_period);

    // 重置计数器
    if (mr->current_enable) mr->current_counter = 0;
    if (mr->speed_enable) mr->speed_counter = 0;
    if (mr->pos_enable) mr->pos_counter = 0;

    return mr->current_enable || mr->speed_enable || mr->pos_enable;
}
