//
// Created by xwj on 1/13/26.
//

#ifndef ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_CONFIGMOTOR_H
#define ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_CONFIGMOTOR_H

typedef struct ConfigMotor_t ConfigMotor_t;

struct ConfigMotor_t {
  int pole_pairs;
  float rated_voltage;
  float rated_current;
};

#endif // ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_CONFIGMOTOR_H
