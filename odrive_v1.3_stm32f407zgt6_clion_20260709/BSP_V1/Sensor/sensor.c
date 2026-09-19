//
// Created by xwj on 1/14/26.
//

#include "sensor.h"
#include <stdio.h>

extern SPI_HandleTypeDef hspi1;
Sensor_t sensor;
int Sensor_Example(void) {
  sensor.sensorType = Sensor_MT6835;
  sensor.mt6835.hspi = &hspi1;
  sensor.mt6835.cs_port = GPIOA;
  sensor.mt6835.cs_pin  = GPIO_PIN_4;
  sensor.mt6835.delta_t = 1e-3f;
  sensor.mt6835.direction = 0;
  Sensor_Init(&sensor);

  while (1) {
    float vel = Sensor_getVelocity(&sensor);
    printf("%f,%f\n", sensor.angle, vel);
    delay_ms(10);

  }

}
int Sensor_Init(Sensor_t* sensor) {
  if (!sensor) {
    printf("[ERROR] Sensor failed to INIT\n");
    return -1;
  }
  switch (sensor->sensorType) {
    case Sensor_MT6835:
    // sensor->mt6835.hspi
    // sensor->mt6835.cs_port
    // sensor->mt6835.cs_pin
    // sensor->mt6835.delta_t
    // sensor->mt6835.direction
    MT6835_Init(&sensor->mt6835);
    break;
  case Sensor_AS5600:
    break;
  default:
    printf("[ERROR] Sensor need to choose SensorType\n");
    break;
  }


}
float Sensor_getVelocity(Sensor_t* sensor)
{
  if (!sensor) {
    printf("[ERROR] Sensor is NOT SET\n");
    return -1;
  }
  switch (sensor->sensorType)
  {
    float velocity;
  // case Sensor_AS5600:
  //   velocity = AS5600_getVelocity(&encoder->AS5600);
  //   encoder->angle = encoder->AS5600.angle;
  //   return velocity;
  case Sensor_MT6835:
    velocity = MT6835_GetVelocityHz(&sensor->mt6835);
    sensor->angle = sensor->mt6835.angle;
    return velocity;
  default:
    printf("please choose a valid encoder type!\n");
    break;
  }
  return 0;
}
float Sensor_getAngle(Sensor_t* sensor)
{
  if (!sensor) {
    printf("[ERROR] Sensor is NOT SET\n");
    return -1;
  }
  switch (sensor->sensorType)
  {
    // case Sensor_AS5600:
    //   velocity = AS5600_getVelocity(&encoder->AS5600);
    //   encoder->angle = encoder->AS5600.angle;
    //   return velocity;
  case Sensor_MT6835:
    sensor->angle = MT6835_GetAngleRadians(&sensor->mt6835);
    // sensor->angle = sensor->mt6835.angle;
    return sensor->angle;
  default:
    printf("please choose a valid encoder type!\n");
    break;
  }
  return 0;
}
