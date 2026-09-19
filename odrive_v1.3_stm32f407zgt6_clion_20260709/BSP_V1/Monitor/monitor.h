//
// Created by xwj on 1/17/26.
//

#ifndef ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_MONITOR_H
#define ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_MONITOR_H

typedef struct Monitor_t Monitor_t;
struct Monitor_t {
  float ref;
  float Ua, Ub, Uc, Ud, Uq;
  float Ia, Ib, Ic, Id, Iq, ref_id, ref_iq;
  float Ia_prev, Ib_prev, Ic_prev, Id_prev, Iq_prev;
  float velocity_prev, angle_prev;
  float velocity, ref_velocity, angle, ref_angle, degree, ref_degree, position, ref_position;
  int vel_period, pos_period;

};

#endif // ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_MONITOR_H
