//
// Created by xwj on 1/21/26.
//

#ifndef ODRIVE_V1_0_STM32F405VGT6_DEBUG_H
#define ODRIVE_V1_0_STM32F405VGT6_DEBUG_H

typedef struct {
  float ref, ref_velocity, ref_angle, ref_current;
  float velocity, angle;
  float delta_current, delta_velocity, delta_value;

}DebugParam_t;

#endif // ODRIVE_V1_0_STM32F405VGT6_DEBUG_H
