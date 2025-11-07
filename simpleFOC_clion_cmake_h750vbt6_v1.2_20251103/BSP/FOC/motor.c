//
// Created by xwj on 11/3/25.
//

#include "motor.h"

#include "pwm.h"
#include "stm32_hal.h"
#include "ads1256.h"
#include <stdio.h>
#include "timer.h"
#include "currentSense.h"

extern BLDCMotor_t motor;
extern BLDCDriver_t driver;
extern FOCMotor_t FOCMotor;
extern Encoder_t encoder;
extern AS5600_t as5600;
extern CurrentSense_t currentSense;
extern PIDController pid;

void MotorFOC_Init(BLDCMotor_t *motor, PIDController pid_param, ControlType_t ctrl_type)
{
    CurrentSense_Init(&currentSense);
    motor->currentSense = &currentSense;

    Encoder_init(&encoder);
    BLDCMotor_init(motor, 7);
    BLDCDriver_init(&driver, 10e3, 4096, 12, 12);
    BLDCMotor_linkEncoder(motor, &encoder);
    BLDCMotor_linkDriver(motor, &driver);

    BLDCMotor_alignSensor(motor);

    // 阶跃测试：用于PID参数调试
    // 步骤1: 仅P控制，找到临界P值
    // PIDController_init(&pid, 0.8f, 0.0, 0, 5, 2); // 从小P值开始
    PIDController_init(&pid, pid_param.P, pid_param.I, pid_param.D, 5, 4); // 从小P值开始
    motor->pid = &pid;
    // motor->foc_motor.controller = ControlType_velocityClosedLoop;
    // motor.foc_motor.foc_modulation = FOCModulationType_SinePWM;
    motor->foc_motor.controller = ctrl_type;
    motor->target = 0.0f; // 低速测试

    // motor.foc_motor.zero_electric_angle = 0.0f;
    BLDCMotor_enable(motor);
    delay_ms(100);

    printf("=== PID Tuning Mode ===\n");
    printf("Current PID: P=%.2f, I=%.2f, D=%.2f\n", pid.P, pid.I, pid.D);
    printf("Target: %.2f RPS\n", motor->target);
    printf("Initial Angle: %.2f rad\n", motor->angle);
    printf("Observe: Rise time, Overshoot, Steady-state error\n");

    // currentSense.adc = &currentSense_adc2;

    printf("\n=== Motor Status Check ===\n");
    // printf("isEnable: %d (should be 1)\n", motor.isEnable);
    printf("zero_electric_angle: %.4f rad\n", motor->foc_motor.zero_electric_angle);
    printf("voltage_limit: %.2f V\n", motor->foc_motor.voltage_limit);
    printf("pole_pairs: %d\n", motor->foc_motor.pole_pairs);
    printf("controller: %d (2=velocityClosedLoop)\n", motor->foc_motor.controller);
    printf("===========================\n\n");

    // 【诊断】添加调试输出
    printf("\n=== Starting Control Loop ===\n");
    delay_ms(1000);
    // 【关键】校准后重新初始化所有状态变量
    static int debug_count = 0;

}
