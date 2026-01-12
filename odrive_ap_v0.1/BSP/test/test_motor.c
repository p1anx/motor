//
// Created by xwj on 12/24/25.
//

#include "LoopController.h"
#include "as5600.h"
#include "currentSense.h"
#include "key.h"
#include "lowpass_filter.h"
#include "motor.h"
#include "pwm.h"
#include "usart.h"
#include "vofa.h"
#include "DRV830X.h"

#include <stdio.h>
#include "CONFIG_PID.h"
#include <string.h>
#ifdef JS_RTT
#include "JS_RTT.h"
#endif


extern BLDCMotor_t motor;
extern CurrentSense_t currentSense;
extern AS5600_t as5600;
extern Encoder_t encoder;
extern char txDMA_buffer[256];
extern int TxCompleteFlag;
typedef struct g_motorVar g_motorVar;
static void motor_currentOpenLoopBandwith(void)
{
  BLDCMotor_init3508(&motor, EncoderType_MT6835, ControlType_currentVelocityOpenLoop);

  delay_ms(1000);
  printf("waiting to start motor....\n");
  // motor.monitorType = Monitor_UaUbUc;
  motor.monitorType = Monitor_IaIbIdIqUabc;
  // motor.monitorType = Monitor_Velocity;

  motor.target = 2.f;

  int i = 0;
  int last_t = getUs();
  int tx_counter = 0;
  motor.isEnable = 1;

  while (1)
  {
    // BLDCMotor_ControlRunByKey(&motor);
    BLDCMotor_KeyControl(&motor);
    if (motor.isEnable) {
      BLDCMotor_move(&motor, motor.target);
      // if (getUs() % 100 == 0) {
        motor.velocity = BLDCMotor_getVelocity(&motor);

        // BLDCMotor_Monitor(&motor,motor.monitorType);

      // }


      // HAL_Delay(1);
    }
  }
}

static void motor_velocityLoop(void)
{
    // HAL_TIM_Base_Start(&us_htim);
    PIDController pid_id;
    PIDController pid_iq;
    PIDController pid_degree;

    PIDController pid_velocity;
    motor.PID_velocity.P = 0.6f;
    motor.PID_velocity.I = 0.1f;
    motor.PID_velocity.D = 0.0f;
    motor.PID_velocity.limit = 2.f;
  motor.PID_velocity.output_ramp = 100;
    BLDCMotor_init3508(&motor, EncoderType_MT6835, ControlType_velocityClosedLoop);
    delay_ms(1000);
    // BLDCMotor_disable(&motor);

    printf("waiting to start motor....\n");
    // HAL_UART_Transmit_DMA(&vofa_huart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));
    // motor.monitorType = Monitor_NULL;
    // motor.monitorType = Monitor_Velocity;
    motor.monitorType = Monitor_IalphaIbetaUabc;

    motor.target = 2.0f;
  int t_last = 0;

    while (1)
    {
        // BLDCMotor_ControlRunByKey(&motor);
        BLDCMotor_KeyControl(&motor);
        if (motor.isEnable) {
            BLDCMotor_move(&motor, motor.target);
          if (HAL_GetTick() - t_last > 1000) {
            motor.target = motor.target + 1;
            t_last = HAL_GetTick();
          }
          if (motor.target > 10) {
            motor.target = 0;
          }
          // if (getUs() % 100 == 0) {
          //   // motor.velocity = BLDCMotor_getVelocity(&motor);
          //
          //   BLDCMotor_Monitor(&motor,motor.monitorType);
          // }
        }
      else {
        BLDCDriver_setPwm(&motor.Driver, 0, 0, 0);
         // BLDCMotor_disable(&motor);

      }
    }
}

void motor_currentLoop(void)
{

  //current pid
  //1. id
  // motor.PID_id.P = ;//10
  // motor.PID_id.I = 0.0f; //90
  // motor.PID_id.limit = 1;
  //
  // //2.
  // // motor.PID_id.P = 0.116f;
  // // motor.PID_id.I = 471.225f;
  // // motor.PID_id.limit = 3;
  //
  // motor.PID_iq.P = 1.5f;
  // motor.PID_iq.I = 1.f;//200
  // motor.PID_iq.limit = 2;
#ifdef JS_RTT
  JS_RTT_InitFloat(3);
#endif

  motor.PID_id.P = CONFIG_PID_CUR_ID_KP;//10
  motor.PID_id.I = CONFIG_PID_CUR_ID_KI; //90
  motor.PID_id.limit = CONFIG_PID_CUR_ID_LIMIT;
  motor.PID_id.output_ramp = CONFIG_PID_CUR_ID_OUTRAMP;

  motor.PID_iq.P = CONFIG_PID_CUR_IQ_KP;
  motor.PID_iq.I = CONFIG_PID_CUR_IQ_KI;//33
  motor.PID_iq.limit = CONFIG_PID_CUR_IQ_LIMIT;
  motor.PID_iq.output_ramp = 100;
  BLDCMotor_init3508(&motor, EncoderType_MT6835, ControlType_currentClosedLoopBandwidth);
  delay_ms(1000);
  BLDCMotor_disable(&motor);

  printf("waiting to start motor....\n");
  // HAL_UART_Transmit_DMA(&vofa_huart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));

  // motor.monitorType = Monitor_TargetIdIq;
  motor.monitorType = Monitor_NULL;
  motor.target = 0.0f;

  int i = 0;
  int last_t = getUs();
  while (1)
  {
    // BLDCMotor_ControlRunByKey(&motor);
    BLDCMotor_KeyControl(&motor);
    if (motor.isEnable) {
      static int last_t = 0;
      int now_t = getUs();
      float delta_target = 1.f;

      if (now_t - last_t > 1000000) {
        motor.target += delta_target;
        last_t =  now_t;
      }
      if (motor.target > delta_target*4) {
        motor.target = 0;
      }
      BLDCMotor_move(&motor, motor.target);
      // UART_DMASendVOFA_justFloat2(motor.target, motor.velocity);

      // HAL_Delay(1);
    }
  }
}

void motor_currentVelocityLoop(void)
{
  motor.PID_id.P = CONFIG_PID_CUR_ID_KP;//10
  motor.PID_id.I = CONFIG_PID_CUR_ID_KI; //90
  motor.PID_id.limit = CONFIG_PID_CUR_ID_LIMIT;

  motor.PID_iq.P = CONFIG_PID_CUR_IQ_KP;
  motor.PID_iq.I = CONFIG_PID_CUR_IQ_KI;//33
  motor.PID_iq.limit = CONFIG_PID_CUR_IQ_LIMIT;

  motor.PID_velocity.P = CONFIG_PID_CUR_VEL_KP;
  motor.PID_velocity.I = CONFIG_PID_CUR_VEL_KI;
  motor.PID_velocity.limit = CONFIG_PID_CUR_VEL_LIMIT; //A

  BLDCMotor_init3508(&motor, EncoderType_MT6835, ControlType_currentVelocityClosedLoopBandwidth);
  delay_ms(1000);
  BLDCMotor_disable(&motor);

  printf("waiting to start motor....\n");
  // HAL_UART_Transmit_DMA(&print_uart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));

  // motor.target = 360.f;
  motor.target = 0.0f;

  int i = 0;
  int last_t = getUs();
  while (1)
  {
    // BLDCMotor_ControlRunByKey(&motor);
    BLDCMotor_KeyControl(&motor);
    if (motor.isEnable) {
      const int num = 6;
      static int last_t = 0;
      int now_t = getUs();
      float delta_target = 360.f * 3;

      if (now_t - last_t > 1000000) {
        motor.target += delta_target;
        last_t =  now_t;
      }
      if (delta_target > 0) {
        if (motor.target > delta_target* num) {
          motor.target = 0;
        }
      }
      else {
        if (motor.target < delta_target* num) {
          motor.target = 0;
        }

      }
      // motor.target = 360.0f;
      BLDCMotor_move(&motor, motor.target);
      // UART_DMASendVOFA_justFloat2(motor.target, motor.velocity);

      // HAL_Delay(1);
    }
  }
}

float g_delta_target = 10.f;
void motor_currentVelocityAngleLoop(void)
{
  motor.PID_id.P = CONFIG_PID_CUR_ID_KP;//10
  motor.PID_id.I = CONFIG_PID_CUR_ID_KI; //90
  motor.PID_id.limit = CONFIG_PID_CUR_ID_LIMIT;

  motor.PID_iq.P = CONFIG_PID_CUR_IQ_KP;
  motor.PID_iq.I = CONFIG_PID_CUR_IQ_KI;//33
  motor.PID_iq.limit = CONFIG_PID_CUR_IQ_LIMIT;

  motor.PID_velocity.P = CONFIG_PID_CUR_VEL_KP;
  motor.PID_velocity.I = CONFIG_PID_CUR_VEL_KI;
  motor.PID_velocity.limit = CONFIG_PID_CUR_VEL_LIMIT; //A

  motor.PID_degree.P = CONFIG_PID_CUR_VEL_POS_KP;
  motor.PID_degree.I = CONFIG_PID_CUR_VEL_POS_KI;
  motor.PID_degree.limit = CONFIG_PID_CUR_VEL_POS_LIMIT; //deg/s
  BLDCMotor_init3508(&motor, EncoderType_MT6835, ControlType_currentVelocityAngleClosedLoopBandwidth);
    delay_ms(1000);
    BLDCMotor_disable(&motor);

    printf("waiting to start motor....\n");
    // motor.target = 360.f;
    motor.target = 0.0f;

    int i = 0;
    int last_t = getUs();
    while (1)
    {
        // BLDCMotor_ControlRunByKey(&motor);
        BLDCMotor_KeyControl(&motor);
        if (motor.isEnable) {
            static int last_t = 0;
            int now_t = getUs();
            float delta_target = g_delta_target;
            static int target_direction = 0;

            if (now_t - last_t > 1000000) {
                if (motor.target >= delta_target*16) {
                    // motor.target = 0;
                    target_direction = 1;
                }
                else if (motor.target <= 10) {
                    target_direction = 0;
                }
                if (target_direction == 0) {
                    motor.target += delta_target;
                }
                else {
                    motor.target -= delta_target;
                }
                last_t =  now_t;
            }
            BLDCMotor_move(&motor, motor.target);
        }
    }
}
void test_motor(void) {
  //1. velocity open loop
  motor_currentOpenLoopBandwith();

  // 2. velocity closed loop
  // motor_velocityLoop();

  //3. current
  // motor_currentLoop();

  //4. current velocity
  // motor_currentVelocityLoop();

  //5. current velocity vangle
  // motor_currentVelocityAngleLoop();

}