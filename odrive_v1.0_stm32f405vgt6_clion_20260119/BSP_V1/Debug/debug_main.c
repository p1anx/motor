//
// Created by xwj on 1/13/26.
//

#include <stdio.h>
#include "mt6835.h"
#include "adc_sampling.h"
#include "currentSense.h"
#include "pwm.h"
#include "FOCDriver.h"
#include "foc.h"
#include "Motor.h"
#include "sensor.h"

extern Motor_t motor;
float g_t;
extern Sensor_t sensor;
extern CurrentSense_t CurrentSense;

typedef struct {
  float ref, ref_velocity, ref_angle, ref_current;
  float velocity, angle;
  float delta_current, delta_velocity, delta_value;

}DebugParam_t;

DebugParam_t DebugParam;

int GetInterval_ms(int ms) {
  static int last_t = 0, now_t = 0;
  now_t = HAL_GetTick();
  if (now_t - last_t >= 1000) {
    last_t = now_t;
    return 1;
  }
  return 0;
}
void Motor_runMode_stepCurrent(Motor_t *motor) {
  DebugParam.delta_current = 0.5f; //A
  if (GetInterval_ms(1000)) {
    motor->ref +=  DebugParam.delta_current;
  }
  if (motor->ref >= DebugParam.delta_current*5) {
    motor->ref = 0;
  }

}
void Motor_runMode_stepVelocity(Motor_t *motor) {
  DebugParam.delta_value = 360.f; //A
  // static int last_t = 0, now_t = 0;
  // now_t = HAL_GetTick();
  // if (now_t - last_t >= 1000) {
  //   motor->ref +=  DebugParam.delta_current;
  //   last_t = now_t;
  // }
  if (GetInterval_ms(1000)) {
    motor->ref +=  DebugParam.delta_value;
  }
  if (motor->ref >= DebugParam.delta_value*5) {
    motor->ref = 0;
  }

}
void Motor_runMode_stepDegrees(Motor_t *motor) {
  DebugParam.delta_value = 20.f; //A
  // static int last_t = 0, now_t = 0;
  // now_t = HAL_GetTick();
  // if (now_t - last_t >= 1000) {
  //   motor->ref +=  DebugParam.delta_current;
  //   last_t = now_t;
  // }
  if (GetInterval_ms(1000)) {
    motor->ref +=  DebugParam.delta_value;
  }
  if (motor->ref >= DebugParam.delta_value*5) {
    motor->ref = 1.f;
  }

}

void debug_motorCurrent(void) {

  printf("\r\n");
  Motor_InitConfig(&motor);
  delay_ms(1000);

  while (1) {

    Motor_move(&motor, DebugParam.ref);

  }
}

void debug_motorConfig(void) {
  printf("\r\n");
  Motor_InitConfig(&motor);
  // PWM_Init(&motor.FOCDriver.pwm);
  // CurrentSense_Init(motor.pCurrentSense);
  // Sensor_Init(&motor.Sensor);
  DebugParam.ref = 180;

  delay_ms(1000);

  while (1) {

    // Motor_runMode_stepVelocity(&motor);
    // Motor_runMode_stepCurrent(&motor);
    Motor_runMode_stepDegrees(&motor);
    Motor_move(&motor, motor.ref);

  }
}

void debug_sensor(void){
  Sensor_Example();

}
void debug_motor_with_current(void) {
  // CurrentSense_Example();
  Motor_linkCurrentSense(&motor, &CurrentSense);
  Motor_CurrentSenseInit(&motor);
  Motor_Driver_Init();
  motor.controllerType = ControllerType_velocityOpenLoop;

  motor.Sensor.sensorType = Sensor_MT6835;
  motor.isEnabled = 1;
  Motor_SensorInit(&motor);

  delay_ms(1000);
  while (1) {
    // Motor_velocityOpenLoop(&motor, 1);
    // motor.pCurrentSense->isReady = 0;
    Motor_move(&motor, 1);
    // printf("%f\n", motor.velocity);

    // delay_ms(1);
  }
}

void debug_motor(void) {
 Motor_Driver_Init();
  while (1) {
    Motor_velocityOpenLoop(&motor, 1);
    delay_ms(1);
  }

}
void debug_mt6835(void) {
  MT6835_Example_BasicUsage();

}
void debug_driver(void) {
  FOCDriver_PWM_Init();
  while (1) {

  }

}
void debug_pwm(void) {
  // PWM_Example();
  FOCDriver_PWM_Init();

  while (1) {

  }


}
void debug_adc(void) {
  // ADC_Example();
  CurrentSense_Example();


}

void debug_main(void) {
  // debug_mt6835();
  // debug_adc();
  // debug_pwm();
  // debug_motor();
  // debug_motor_with_current();
  debug_motorConfig();
  // debug_motorCurrent();
  // debug_sensor();

}
