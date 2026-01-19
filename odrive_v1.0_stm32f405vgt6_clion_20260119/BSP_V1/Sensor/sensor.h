//
// Created by xwj on 1/14/26.
//

#ifndef ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_SENSOR_H
#define ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_SENSOR_H
#include "mt6835.h"

typedef enum {
  Sensor_MT6835,
  Sensor_AS5600
}SensorType;

typedef struct Sensor_t Sensor_t;
struct Sensor_t {
  SensorType sensorType;
  MT6835_t mt6835;
  float ref_velocity, ref_position, ref_degree;
  float velocity, position, degree, angle;

};

int Sensor_Init(Sensor_t* sensor);
float Sensor_getVelocity(Sensor_t* sensor);
float Sensor_getAngle(Sensor_t* sensor);
int Sensor_Example(void);
#endif // ODRIVE_F407ZGT6_CMAKE_V0_2_20260104_SENSOR_H
