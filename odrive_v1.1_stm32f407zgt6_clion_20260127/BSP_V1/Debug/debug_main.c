
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
#include "config.h"
#include "configODrive.h"
#include "DRV830X.h"
#include "debug.h"
#include "QAM.h"

extern Motor_t motor;
float g_t;
extern Sensor_t sensor;
extern CurrentSense_t CurrentSense;

DebugParam_t DebugParam;

void Motor_UpdatePID(Motor_t* motor) {
  DebugParam.delta_value = 20;
  float delta_value = DebugParam.delta_value;
  if (GetInterval_ms(2100)) {
    motor->PID_iVelDegree.P +=  delta_value;
  }
  if (motor->PID_iVelDegree.P >= delta_value*3) {
    motor->PID_iVelDegree.P = 00.f;
  }

}

void Motor_runMode_stepDegreesTime(Motor_t *motor) {
  DebugParam.delta_value = 20.f; //A
  // static int last_t = 0, now_t = 0;
  // now_t = HAL_GetTick();
  // if (now_t - last_t >= 1000) {
  //   motor->ref +=  DebugParam.delta_current;
  //   last_t = now_t;
  // }
  float delta_vel = 10.f;
  if (GetInterval_ms(1000)) {
    motor->ref +=  DebugParam.delta_value;
    motor->PID_iVelDegree.P += delta_vel;
  }
  if (motor->ref >= DebugParam.delta_value*8) {
    motor->ref = 1.f;
  }

  if (motor->PID_iVelDegree.P >= delta_vel * 4) {
    motor->PID_iVelDegree.P = 10;
  }

}
void debug_motorSetTime(void) {
  printf("\r\n");
  Motor_InitConfig(&motor);

  // PWM_Init(&motor.FOCDriver.pwm);
  // CurrentSense_Init(motor.pCurrentSense);
  // Sensor_Init(&motor.Sensor);
  DebugParam.ref = 1;
  motor.ref = DebugParam.ref;

  delay_ms(1000);

  while (1) {

    // Motor_runMode_stepVelocity(&motor);
    // Motor_runMode_stepCurrent(&motor);
    Motor_runMode_stepDegreesTime(&motor);
    // Motor_UpdatePID(&motor);
    // Motor_runMode(&motor);
    Motor_move(&motor, motor.ref);

  }
}
void debug_motorConfig(void) {
  printf("\r\n");
  Motor_InitConfig(&motor);

  // PWM_Init(&motor.FOCDriver.pwm);
  // CurrentSense_Init(motor.pCurrentSense);
  // Sensor_Init(&motor.Sensor);
  DebugParam.ref = 1;
  motor.ref = DebugParam.ref;

  delay_ms(1000);

  while (1) {

    Motor_runMode(&motor);
    Motor_move(&motor, motor.ref);

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
void debug_MotorMT6835(void) {
  // Motor_InitConfig(&motor);
  ConfigMotor_ODrive(&motor);



  Sensor_Init(&motor.Sensor);
  while (1) {

    motor.velocity = Motor_getVelocity(&motor);
    printf("angle:%f\n", motor.Sensor.angle);
    delay_ms(10);
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

void debug_drv830x(void) {
  // drv8301_example0();
  drv8301_example1();

}

void debug_main(void) {
  // 0. test
  // CurrentSense_Example();
  // debug_drv830x();
  // debug_mt6835();
  // debug_MotorMT6835();
  // debug_adc();
  // debug_pwm();
  // debug_motor();
  // debug_motor_with_current();
  // debug_motorSetTime();
  // debug_motorCurrent();
  // debug_sensor();
  // 1.ok
  debug_motorConfig();
  //2. QAM
  // QAM_main();

}
