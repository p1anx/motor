//
// Created by xwj on 1/12/26.
//

#ifndef ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_MONITOR_H
#define ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_MONITOR_H

typedef struct gMonitorVars_t gMonitorVars_t;

struct gMonitorVars_t{
  float Ia, Ib, Ic, Id, Iq, Ialpha, Ibeta;
  float Ua, Ub, Uc, Ualpha, Ubeta;
  float velocity, degree, target_velocity, target_iq, target;
  int runtime_us, runtime_ms;

};

#endif // ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_MONITOR_H
