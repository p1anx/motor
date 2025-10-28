/**
 * @file    mt6835.h
 * @brief   MT6835磁性编码器驱动头文件
 * @author
 * @date    2025
 */

#ifndef __MT6835_H
#define __MT6835_H

#include "stm32f4xx_hal.h"  // 根据你的STM32系列修改
#include <stdint.h>
#include <stdbool.h>

extern  SPI_HandleTypeDef hspi1;
#define  mt6835_spi hspi1

#define  MT6835_SPI_CS_PORT GPIOA
#define  MT6835_SPI_CS_PIN GPIO_PIN_4

/* MT6835 SPI命令定义 */
#define MT6835_CMD_READ         0x03    // 读寄存器命令
#define MT6835_CMD_WRITE        0x06    // 写寄存器命令
#define MT6835_CMD_PROG_EEPROM  0x0C    // 编程EEPROM命令
#define MT6835_CMD_BURST_READ   0x0A    // 连续读角度命令

/* MT6835 寄存器地址定义 */
#define MT6835_REG_ANGLE_H      0x003   // 角度寄存器高字节
#define MT6835_REG_ANGLE_M      0x004   // 角度寄存器中字节
#define MT6835_REG_ANGLE_L      0x005   // 角度寄存器低字节
#define MT6835_REG_ZERO_POS_H   0x000   // 零位高字节
#define MT6835_REG_ZERO_POS_L   0x001   // 零位低字节
#define MT6835_REG_ABZ_RES_H    0x002   // ABZ分辨率高字节
#define MT6835_REG_ABZ_RES_L    0x009   // ABZ分辨率低字节

/**
 * @brief  电机角度计算句柄
 */
typedef struct  Motor_Angle_t Motor_Angle_t;

struct  Motor_Angle_t{
    // 配置参数
    float gear_ratio;              // 减速比 (例如: 10表示10:1)

    // 电机轴(编码器)数据
    float motor_angle_deg;         // 电机轴当前角度 (0-360°)
    uint32_t motor_angle_raw;      // 电机轴原始编码器值
    int32_t motor_turns;           // 电机轴转过的圈数
    float motor_total_angle;       // 电机轴累积总角度

    // 输出轴数据 (经过减速器)
    float output_angle_deg;        // 输出轴当前角度 (0-360°)
    int32_t output_turns;          // 输出轴转过的圈数
    float output_total_angle;      // 输出轴累积总角度

    // 内部变量
    float prev_motor_angle;        // 上次电机轴角度
    float last_angle;        // 上次电机轴角度
    bool is_first_read;            // 是否首次读取
};

typedef struct {
    // 配置参数
    float gear_ratio;              // 减速比 (例如: 10表示10:1)

    // 电机轴(编码器)数据
    float motor_angle_deg;         // 电机轴当前角度 (0-360°)
    uint32_t motor_angle_raw;      // 电机轴原始编码器值
    int32_t motor_turns;           // 电机轴转过的圈数
    float motor_total_angle;       // 电机轴累积总角度

    // 输出轴数据 (经过减速器)
    float output_angle_deg;        // 输出轴当前角度 (0-360°)
    int32_t output_turns;          // 输出轴转过的圈数
    float output_total_angle;      // 输出轴累积总角度

    // 内部变量
    float prev_motor_angle;        // 上次电机轴角度
    bool is_first_read;            // 是否首次读取
} Motor_AngleCalc_Handle;

/* MT6835 数据结构 */
typedef struct {
    SPI_HandleTypeDef *hspi;    // SPI句柄
    GPIO_TypeDef *cs_port;      // CS引脚端口
    uint16_t cs_pin;            // CS引脚编号
    uint32_t angle_raw;         // 原始角度值(21位)
    float angle_deg;            // 角度值(度)
    float angle_rad;            // 角度值(弧度)
    Motor_AngleCalc_Handle motor_angle_calc;
    Motor_Angle_t motor_angle;
} MT6835_Handle;

/* 函数声明 */
void MT6835_Init(MT6835_Handle *mt6835, SPI_HandleTypeDef *hspi,
                 GPIO_TypeDef *cs_port, uint16_t cs_pin);
uint8_t MT6835_ReadRegister(MT6835_Handle *mt6835, uint16_t reg_addr);
void MT6835_WriteRegister(MT6835_Handle *mt6835, uint16_t reg_addr, uint8_t data);
bool MT6835_ReadAngle(MT6835_Handle *mt6835);
bool MT6835_ReadAngleFast(MT6835_Handle *mt6835);
float MT6835_GetAngleDegrees(MT6835_Handle *mt6835);
float MT6835_GetAngleRadians(MT6835_Handle *mt6835);
uint32_t MT6835_GetAngleRaw(MT6835_Handle *mt6835);
void MT6835_SetZeroPosition(MT6835_Handle *mt6835, uint16_t zero_pos);


/* 函数声明 */
void Motor_AngleCalc_Init(Motor_AngleCalc_Handle *motor, float gear_ratio);
void Motor_AngleCalc_Update(Motor_AngleCalc_Handle *motor, float encoder_angle);

// 获取电机轴信息
float Motor_GetMotorAngle(Motor_AngleCalc_Handle *motor);
float Motor_GetMotorTotalAngle(Motor_AngleCalc_Handle *motor);
int32_t Motor_GetMotorTurns(Motor_AngleCalc_Handle *motor);

// 获取输出轴信息
float Motor_GetOutputAngle(Motor_AngleCalc_Handle *motor);
float Motor_GetOutputTotalAngle(Motor_AngleCalc_Handle *motor);
int32_t Motor_GetOutputTurns(Motor_AngleCalc_Handle *motor);
float Motor_GetOutputAngleRad(Motor_AngleCalc_Handle *motor);

// 重置和校准
void Motor_AngleCalc_Reset(Motor_AngleCalc_Handle *motor);
void Motor_SetZeroPosition(Motor_AngleCalc_Handle *motor);
void Motor_ResetToZero(Motor_AngleCalc_Handle *motor);

// void MT6835_MotorAngle_Init(Motor_Angle_t *motor, float gear_ratio);

void MT6835_MotorAngle_Init(MT6835_Handle *mt6835, float gear_ratio);
void MT6835_Angle_Update(MT6835_Handle *mt6835);
float MT6835_GetMotorAngleDegrees(MT6835_Handle *mt6835);

void MT6835_Angle_Update_with_last(MT6835_Handle *mt6835);
void Example1_BasicUsage(void);
#endif /* __MT6835_H */