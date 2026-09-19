//
// Created by xwj on 1/14/26.
//

#include "Motor.h"
#include "FOCDriver.h"
#include "adc.h"
#include "config.h"
#include "configODrive.h"
#include "foc.h"

#include <arm_math.h>

#include <stdio.h>

#include "monitor.h"
#include "pid.h"
#include "debug.h"
#include "stm32_hal.h"

extern DebugParam_t DebugParam;
extern Monitor_t monitor;

Motor_t motor;
extern FOCDriver_t FOCDriver;
extern CurrentSense_t CurrentSense;
extern TIM_HandleTypeDef htim1;
extern KEY_t key_enableMotor;

void Motor_InitConfig(Motor_t* motor) {
  ConfigMotor(motor);
  // ConfigMotor_ODrive(motor);
  Motor_DRV830XInit(motor);

  PWM_Init(&motor->FOCDriver.pwm);
  Sensor_Init(&motor->Sensor);
  Motor_alignSensor(motor);
  delay_ms(1000);

  CurrentSense_Init(motor->pCurrentSense);
  Motor_FilterInit(motor);
  Motor_PIDInit(motor);
  delay_ms(1000);
}

void Motor_DRV830XInit(Motor_t* motor) {

  if (!motor->isODrive) {
    printf("[INFO] ODRIVE IS NOT SET\n");
    return;
  }
  printf("[INFO] ODRIVE IS SET\n");
  // motor->drv830xConfig->gain = 40; // 10 20 40 80
  DRV8301_Setup(&motor->drv830xConfig);

}

void Motor_FilterInit(Motor_t* motor) {
  LowPassFilter_init(&motor->lpf_ia, motor->lpf_ia.Tf, &motor->dt);
  LowPassFilter_init(&motor->lpf_ib, motor->lpf_ib.Tf, &motor->dt);
  LowPassFilter_init(&motor->lpf_ic, motor->lpf_ic.Tf, &motor->dt);
  LowPassFilter_init(&motor->lpf_id, motor->lpf_id.Tf, &motor->dt);
  LowPassFilter_init(&motor->lpf_iq, motor->lpf_iq.Tf, &motor->dt);
  LowPassFilter_init(&motor->lpf_velocity, motor->lpf_velocity.Tf, &motor->dt);
  printf("[INFO] Motor Filter Init OK\n");

}
void Motor_PIDInit(Motor_t* motor) {
  PID_init(&motor->PID_Velocity);
  PID_init(&motor->PID_VelocityAngle);
  PID_init(&motor->PID_Iq);
  PID_init(&motor->PID_Id);
  PID_init(&motor->PID_Current);
  PID_init(&motor->PID_iVelocity);
  PID_init(&motor->PID_iVelDegree);
  printf("[INFO] Motor PID Init OK\n");
}
void Motor_Driver_Init(void) {
  // FOCDriver_PWM_Init();
  // motor.FOCDriver = FOCDriver;
  motor.FOCDriver.FOCModulationType = FOCModulationType_SinePWM;
  motor.FOCDriver.voltage_power_supply = 15;
  motor.FOCDriver.voltage_limit = 10;

  motor.FOCDriver.pwm.htim = &htim1;
  motor.FOCDriver.pwm.resolution = 4096;
  motor.FOCDriver.pwm.frequency = 10e3;
  motor.FOCDriver.pwm.pwm_type = _6PWM;

  motor.pole_pairs = 7;

  PWM_Init(&motor.FOCDriver.pwm);

}
void Motor_DriverInit(Motor_t* motor) {
  // FOCDriver_PWM_Init();
  // motor.FOCDriver = FOCDriver;
  motor->FOCDriver.FOCModulationType = FOCModulationType_SinePWM;
  motor->FOCDriver.voltage_power_supply = 15;
  motor->FOCDriver.voltage_limit = 10;

  motor->FOCDriver.pwm.htim = &htim1;
  motor->FOCDriver.pwm.resolution = 4096;
  motor->FOCDriver.pwm.frequency = 10e3;
  motor->FOCDriver.pwm.pwm_type = _6PWM;

  motor->pole_pairs = 7;

  PWM_Init(&motor->FOCDriver.pwm);

}
void Motor_linkCurrentSense(Motor_t *motor, CurrentSense_t *pCurrentSense) {
  motor->pCurrentSense = pCurrentSense;
}
int Motor_CurrentSenseInit(Motor_t *motor) {
  // motor->pCurrentSense = &CurrentSense; // must be needed
  if (!motor->pCurrentSense) {
    printf("Motor pCurrentSense is NULL\n");
    return -1;
  }
  motor->pCurrentSense->adc.hadc = &hadc1;
  motor->pCurrentSense->adc.resolution = 4096;
  motor->pCurrentSense->gain = 50;
  motor->pCurrentSense->r_sample = 0.001f;
  motor->pCurrentSense->htim = &htim1;

  CurrentSense_Init(motor->pCurrentSense);

  return 0;

}

int Motor_SensorInit(Motor_t *motor) {
  if (!motor) {
    printf("Motor_SensorInit is NULL\n");
    return -1;
  }
  switch (motor->Sensor.sensorType) {
    case Sensor_MT6835:
      motor->Sensor.mt6835.hspi = &hspi1;
      motor->Sensor.mt6835.cs_port = GPIOA;
      motor->Sensor.mt6835.cs_pin  = GPIO_PIN_4;
      motor->Sensor.mt6835.delta_t = 1e-4f;
      motor->Sensor.mt6835.direction = 0;
      break;
    case Sensor_AS5600:
      break;
    default:
      printf("Motor_SensorInit is BAD SensorType\n");
      return -1;
  }
  Sensor_Init(&motor->Sensor);
  return 0;

}

void Motor_setPhaseVoltage(Motor_t* motor, float Uq, float Ud, float electrical_angle) {
  FOCDriver_setPhaseVoltage(&motor->FOCDriver, Uq, Ud, electrical_angle);
}

void Motor_enable(Motor_t* motor) {
  // FOCDriver_setPwm(&motor->FOCDriver, 0, 0, 0);
  motor->isEnabled = 1;
}
void Motor_disable(Motor_t* motor) {
  motor->isEnabled = 0;
  FOCDriver_setPwm(&motor->FOCDriver, 0, 0, 0);
}

float Motor_getVelocity(Motor_t* motor) {
  return  Sensor_getVelocity(&motor->Sensor);
}
float Motor_getAngle(Motor_t* motor) {
  return  Sensor_getAngle(&motor->Sensor);
}
int g_t0;

int Motor_alignSensor(Motor_t *motor) {
  if (!motor)
    return 0;
  const int delay_t = 30;
  const int resolution = 40;

  Motor_enable(motor);
  // printf("[INFO] Starting sensor alignment...\n");

  // ========== 步骤1：将电机转到固定电角度位置 ==========
  // 使用270度（3π/2）作为校准位置
  const float target_electrical_angle = _3PI_2;  // 270度
  // const int resolution = 20;

  printf("[INFO] Moving motor to alignment position (270 deg electrical)\n");

  // 施加电压，让电机转到270度电角度位置
  for (int i = 0; i < 5; i++)
  {
    Motor_setPhaseVoltage(motor,  motor->voltage_alignSensor,0, target_electrical_angle);
    delay_ms(delay_t);  // 等待电机稳定
  }
  delay_ms(1000);  // 额外延迟确保完全稳定

  // ========== 步骤2：读取此时的机械角度 ==========
  float shaft_angle = Motor_getAngle(motor);
  printf("[INFO] shaft angle at zero: %.4f rad (%.2f deg)\n",
         shaft_angle, shaft_angle * 180.0f / 3.14159265f);

  // ========== 步骤3：计算电角度零点 ==========
  // zero_electric_angle = 目标电角度 - (机械角度 × 极对数)
  float electrical_angle = _electricalAngle(shaft_angle, motor->pole_pairs);
  // printf("[INFO]  zero_electric_angle 0 = %.4f rad (%.2f deg)\n",
  //        electrical_angle,
  //        electrical_angle * 180.0f / 3.14159265f);
  // motor->foc_motor.zero_electric_angle = _normalizeAngle(target_electrical_angle-electrical_angle);

  motor->zero_electrical_angle = electrical_angle;

  // ========== 步骤4：测试方向（可选） ==========
  float start_angle = shaft_angle;

  float start_angle_0 = Motor_getAngle(motor);
  start_angle = start_angle_0;
  electrical_angle = _electricalAngle(start_angle, motor->pole_pairs);
  printf("[INFO]  zero_electric_angle 1 = %.4f rad (%.2f deg)\n",
         electrical_angle,
         electrical_angle * 180.0f / 3.14159265f);
  // if (electrical_angle > _PI) {
  //     electrical_angle = _2PI - electrical_angle;
  // }
  motor->zero_electrical_angle = electrical_angle;
  // 正向转动60度
  for (int i = 0; i <= resolution; i++)
  {
    float angle = _3PI_2 + _2PI * i / (resolution+1);
    angle = _normalizeAngle(angle);
    Motor_setPhaseVoltage(motor,  motor->voltage_alignSensor,0,angle);
    delay_ms(delay_t);
  }
  float end_angle = Motor_getAngle(motor);
  float deltaAngle = end_angle - start_angle;

  for (int i = 0; i <= resolution; i++)
  {
    float angle = _3PI_2 + _2PI *(resolution - i) / (resolution+1);
    angle = _normalizeAngle(angle);
    Motor_setPhaseVoltage(motor,  motor->voltage_alignSensor,0,angle);
    delay_ms(delay_t);
  }
  // float start_angle_1 = Motor_getAngle(motor);
  float shaft2pi_to_electrical2pi;

  if (fabsf(deltaAngle) > 0.5*_2PI)
  {
    if ((deltaAngle) > 0)
    {
      motor->direction = Direction_CW;
      shaft2pi_to_electrical2pi =  (float)(_2PI - end_angle + start_angle);
    }
    else
    {
      motor->direction = Direction_CCW;
      shaft2pi_to_electrical2pi = (float)(_2PI - start_angle + end_angle);
    }
  }
  else
  {
    if (end_angle > start_angle)
    {
      motor->direction = Direction_CCW;
    }
    else if (end_angle < start_angle)
    {
      motor->direction = Direction_CW;
    }
    else
    {
      printf("[ERROR] Sensor failed to notice movement!\n");
    }
    shaft2pi_to_electrical2pi = (fabsf(deltaAngle));

  }
  if (motor->direction == Direction_CCW) {
    printf("[INFO] Motor direction: CCW (Counter-Clockwise-0->2PI)\n");
  }
  else if (motor->direction == Direction_CW) {
    printf("[INFO] Motor direction: CW (Clockwise-2PI->0)\n");

  }
  printf("[INFO] start angle = %f, end angle = %f, delta = %f, pp = %f\n", start_angle, end_angle, shaft2pi_to_electrical2pi, _2PI/shaft2pi_to_electrical2pi);
  Motor_disable(motor);
  return 0;
}

void Motor_GetCurrentIdIq(Motor_t *motor) {
  monitor.Ia_prev = motor->pCurrentSense->i_a;
  monitor.Ib_prev = motor->pCurrentSense->i_b;
  motor->pCurrentSense->i_a = LowPassFilter(&motor->lpf_ia, motor->pCurrentSense->i_a);
  motor->pCurrentSense->i_b = LowPassFilter(&motor->lpf_ib, motor->pCurrentSense->i_b);
  // motor->pCurrentSense->i_c = LowPassFilter(&motor->lpf_ic, motor->pCurrentSense->i_c);
  float* ia = NULL, *ib = NULL, *ic = NULL;
    switch (motor->pCurrentSense->phase) {
      case CurrentSensePhase_AB:
        ia = &motor->pCurrentSense->i_a;
        ib = &motor->pCurrentSense->i_b;
        ic = NULL;
        break;
      case CurrentSensePhase_BC:
        ib = &motor->pCurrentSense->i_a;
        ic = &motor->pCurrentSense->i_b;
      break;
      case CurrentSensePhase_AC:
        ia = &motor->pCurrentSense->i_a;
        ic = &motor->pCurrentSense->i_b;
        ib = NULL;
        break;
      case CurrentSensePhase_ABC:
        ia = &motor->pCurrentSense->i_a;
        ib = &motor->pCurrentSense->i_b;
        ic = &motor->pCurrentSense->i_c;
        break;
      default:
        printf("[ERROR] Motor_GetCurrentIdIq: Unknown current sense phase!\n");
        break;
    }

  Clarke_Park(ia, ib, ic, &motor->pCurrentSense->i_d, &motor->pCurrentSense->i_q, motor->e_angle);



}

void Motor_velocityOpenLoop(Motor_t *motor, float target_velocity)
{
  if (!motor)
    return;
  // get current timestamp

    float velocity = Motor_getVelocity(motor);
  motor->velocity = LowPassFilter(&motor->lpf_velocity, velocity);
  static uint32_t open_loop_timestamp = 0;
  static float angle = 0;
#ifdef IS_FIXED_TIME
  float Ts = motor->Sensor.mt6835.delta_t;
#else
  uint32_t now_us = getUs();
  // calculate the sample time from last call
  g_t0 = now_us - open_loop_timestamp;
  float Ts = (float)(now_us - open_loop_timestamp) * 1e-6f;
  if (Ts > 0.5) {
    Ts = 1e-3f;
  }
  open_loop_timestamp = now_us;
#endif

  float velocity_radian = target_velocity * _2PI;
  angle = _normalizeAngle(angle + velocity_radian* Ts);
  float e_angle = _normalizeAngle(_electricalAngle(angle, motor->pole_pairs));
  // float e_angle = _electricalAngle_calibrated(motor->direction, motor->angle, motor->foc_motor.pole_pairs, motor->foc_motor.zero_electric_angle);
  motor->e_angle = e_angle;

  Motor_GetCurrentIdIq(motor);

  Motor_setPhaseVoltage(motor, motor->voltage_alignSensor, 0, e_angle);

  monitor.Ia = motor->pCurrentSense->i_a;
  monitor.Ib = motor->pCurrentSense->i_b;
  monitor.Ic = motor->pCurrentSense->i_c;
  monitor.Id = motor->pCurrentSense->i_d;
  monitor.Iq = motor->pCurrentSense->i_q;
  monitor.velocity = motor->velocity;

}

float Motor_GetElectricalAngleCalibrated(Motor_t *motor) {
  return  _electricalAngle_calibrated(motor->direction, motor->angle,motor->pole_pairs, motor->zero_electrical_angle);
}

void Motor_velocityClosedLoop(Motor_t *motor, float ref_velocity) {
  motor->ref = ref_velocity;
  float velocity = Motor_getVelocity(motor);
  monitor.velocity_prev = velocity;
  velocity = _constrain(velocity, -20, 20);
  motor->velocity = LowPassFilter(&motor->lpf_velocity, velocity);

  motor->angle = motor->Sensor.angle;
  float e_angle = Motor_GetElectricalAngleCalibrated(motor);
  motor->e_angle = e_angle;

  motor->Uq = PIDController_update(&motor->PID_Velocity,  ref_velocity - motor->velocity);
  Motor_setPhaseVoltage(motor, motor->Uq,0, motor->e_angle);

  monitor.ref = motor->ref;
  monitor.velocity = motor->velocity;

}
void Motor_velocityAngleClosedLoop(Motor_t *motor, float ref) {
  static int angle_period = 2;
  static int angle_cnt = 0;
  motor->ref = ref;
  float velocity = Motor_getVelocity(motor);
  static float ref_velocity = 0;
  monitor.velocity_prev = velocity;
  velocity = _constrain(velocity, -20, 20);
  motor->velocity = LowPassFilter(&motor->lpf_velocity, velocity);

  motor->angle = motor->Sensor.angle;
  float e_angle = Motor_GetElectricalAngleCalibrated(motor);
  motor->e_angle = e_angle;

  float degree = motor->angle * 180 / _PI;
  motor->degree = degree;

  angle_cnt++;
  if (angle_cnt >= angle_period) {
    angle_cnt = 0;
    ref_velocity = PIDController_update(&motor->PID_VelocityAngle,  ref - degree);
  }

  motor->Uq = PIDController_update(&motor->PID_Velocity,  ref_velocity - motor->velocity);
  Motor_setPhaseVoltage(motor, motor->Uq,0, motor->e_angle);

  monitor.ref = motor->ref;
  monitor.degree = degree;
  monitor.velocity = motor->velocity;
  monitor.ref_velocity = ref_velocity;

}

static void Motor_currentClosedLoop(Motor_t *motor, float ref) {
  // motor->ref = ref;
  float velocity = Motor_getVelocity(motor);
  monitor.velocity_prev = velocity;
  velocity = _constrain(velocity, -20, 20);

  motor->angle = motor->Sensor.angle;
  float e_angle = Motor_GetElectricalAngleCalibrated(motor);
  motor->e_angle = e_angle;

  Motor_GetCurrentIdIq(motor);

  motor->Ud = PIDController_update(&motor->PID_Id,  0 - motor->pCurrentSense->i_d);
  motor->Uq = PIDController_update(&motor->PID_Iq,  ref - motor->pCurrentSense->i_q);
  Motor_setPhaseVoltage(motor, motor->Uq,motor->Ud, motor->e_angle);

  monitor.ref = motor->ref;
  monitor.velocity = motor->velocity;
  monitor.Id = motor->pCurrentSense->i_d;
  monitor.Iq = motor->pCurrentSense->i_q;

}
static void Motor_currentVelocityClosedLoop(Motor_t *motor, float ref) {
  motor->ref = ref;
  // motor->ref = ref;
  static int vel_cnt = 0;
  static float ref_iq = 0;
  int vel_period = motor->vel_period;

  float velocity = Motor_getVelocity(motor) * 360;
  monitor.velocity_prev = velocity;
  motor->velocity = LowPassFilter(&motor->lpf_velocity, velocity);
  // motor->velocity = velocity;

  motor->angle = motor->Sensor.angle;
  float e_angle = Motor_GetElectricalAngleCalibrated(motor);
  motor->e_angle = e_angle;
  Motor_GetCurrentIdIq(motor);

  if (vel_cnt++ >= vel_period) {
    vel_cnt = 0;
    ref_iq = PIDController_update(&motor->PID_iVelocity, ref - motor->velocity);
    motor->ref_iq = ref_iq;
  }

  motor->Ud = PIDController_update(&motor->PID_Id,  0 - motor->pCurrentSense->i_d);
  motor->Uq = PIDController_update(&motor->PID_Iq,  ref_iq - motor->pCurrentSense->i_q);
  Motor_setPhaseVoltage(motor, motor->Uq,motor->Ud, motor->e_angle);

  monitor.ref = motor->ref;
  monitor.ref_iq = motor->ref_iq;
  monitor.velocity = motor->velocity;
  monitor.Id = motor->pCurrentSense->i_d;
  monitor.Iq = motor->pCurrentSense->i_q;
  monitor.vel_period = vel_period;

}
static void Motor_currentVelocityAngleClosedLoop(Motor_t *motor, float ref) {
  // motor->ref = ref;
  // motor->ref = ref;
  static int vel_cnt = 0, pos_cnt = 0;
  static float ref_iq = 0, ref_velocity = 0;
  int vel_period = motor->vel_period;
  int pos_period = motor->pos_period;

  float velocity = Motor_getVelocity(motor) * 360;
  monitor.velocity_prev = velocity;
  motor->velocity = LowPassFilter(&motor->lpf_velocity, velocity);

  motor->angle = motor->Sensor.angle;
  float e_angle = Motor_GetElectricalAngleCalibrated(motor);
  motor->e_angle = e_angle;
  Motor_GetCurrentIdIq(motor);

  motor->degree = motor->angle * 180 / _PI;
  if (pos_cnt++ >= pos_period) {
    pos_cnt = 0;
    ref_velocity = PIDController_update(&motor->PID_iVelDegree, ref - motor->degree);
    motor->ref_velocity = ref_velocity;

  }

  if (vel_cnt++ >= vel_period) {
    vel_cnt = 0;
    ref_iq = PIDController_update(&motor->PID_iVelocity, ref_velocity - motor->velocity);
    motor->ref_iq = ref_iq;
  }

  motor->Ud = PIDController_update(&motor->PID_Id,  0 - motor->pCurrentSense->i_d);
  motor->Uq = PIDController_update(&motor->PID_Iq,  ref_iq - motor->pCurrentSense->i_q);
  Motor_setPhaseVoltage(motor, motor->Uq,motor->Ud, motor->e_angle);

  monitor.ref = ref;
  monitor.degree = motor->degree;
  monitor.ref_iq = motor->ref_iq;
  monitor.ref_velocity  = motor->ref_velocity;
  monitor.velocity = motor->velocity;
  monitor.Id = motor->pCurrentSense->i_d;
  monitor.Iq = motor->pCurrentSense->i_q;
  monitor.vel_period = vel_period;

}

int GetInterval_ms(int ms) {
  static uint32_t last_t = 0, now_t = 0;
  now_t = HAL_GetTick();
  if (now_t - last_t >= ms) {
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
  if (motor->controllerType == ControllerType_velocityClosedLoop) {
    DebugParam.delta_value = 1.f; //A
  }
  else if (motor->controllerType == ControllerType_currentVelocityClosedLoop) {
    DebugParam.delta_value = 360.f;
  }
  else{
    DebugParam.delta_value = 0.f;
  }
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
void Motor_runMode(Motor_t* motor) {

  switch (motor->controllerType) {
    case ControllerType_velocityClosedLoop:
    case ControllerType_currentVelocityClosedLoop:
      Motor_runMode_stepVelocity(motor);
      break;
    case  ControllerType_currentClosedLoop:
      Motor_runMode_stepCurrent(motor);
      break;
    case ControllerType_velocityAngleClosedLoop:
    case ControllerType_currentVelocityAngleClosedLoop:
      Motor_runMode_stepDegrees(motor);
      break;
    case ControllerType_velocityOpenLoop:
      motor->ref = 1;
      break;
    default:
      printf("[ERROR] please choose the controllerType for MOTOR\n");
      break;
  }

}
int g_t_cnt = 0;
void Motor_move(Motor_t *motor, float ref) {
  if (!motor)
    return;
  if (!motor->isEnabled)
    return;
  if (motor->pCurrentSense->isReady) {
  uint32_t t0 = getUs();
    motor->pCurrentSense->isReady = 0;
    switch (motor->controllerType) {
    case ControllerType_velocityOpenLoop:
      Motor_velocityOpenLoop(motor, ref);
      break;
    case ControllerType_velocityClosedLoop:
      Motor_velocityClosedLoop(motor, ref);
      break;
    case ControllerType_velocityAngleClosedLoop:
      Motor_velocityAngleClosedLoop(motor, ref);
      break;
    case ControllerType_currentClosedLoop:
      Motor_currentClosedLoop(motor, ref);
      break;
    case ControllerType_currentVelocityClosedLoop:
      Motor_currentVelocityClosedLoop(motor, ref);
      break;
    case ControllerType_currentVelocityAngleClosedLoop:
      Motor_currentVelocityAngleClosedLoop(motor, ref);
      break;
    default:
      break;
    }
  uint32_t t1 = getUs();
  g_t_cnt = t1 - t0;
  }

}