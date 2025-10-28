//
// Created by xwj on 10/13/25.
//

/**
 * @file    mt6835.c
 * @brief   MT6835磁性编码器驱动实现
 * @author
 * @date    2025
 */

#include "mt6835.h"
#include <math.h>
#include "eeprom.h"

#define MT6835_ANGLE_MAX    0x1FFFFF  // 21位最大值 (2097151)
#define PI                  3.14159265358979323846f

/* 私有函数 */
static inline void MT6835_CS_Low(MT6835_Handle *mt6835)
{
    HAL_GPIO_WritePin(mt6835->cs_port, mt6835->cs_pin, GPIO_PIN_RESET);
}

static inline void MT6835_CS_High(MT6835_Handle *mt6835)
{
    HAL_GPIO_WritePin(mt6835->cs_port, mt6835->cs_pin, GPIO_PIN_SET);
}

/**
 * @brief  初始化MT6835
 * @param  mt6835: MT6835句柄
 * @param  hspi: SPI句柄
 * @param  cs_port: CS引脚端口
 * @param  cs_pin: CS引脚编号
 */
void MT6835_Init(MT6835_Handle *mt6835, SPI_HandleTypeDef *hspi,
                 GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
    float last_angle;
    mt6835->hspi = hspi;
    mt6835->cs_port = cs_port;
    mt6835->cs_pin = cs_pin;
    mt6835->angle_raw = 0;
    mt6835->angle_deg = 0.0f;
    mt6835->angle_rad = 0.0f;
    // Motor_AngleCalc_Init(&mt6835->motor_angle_calc,10);
    MT6835_MotorAngle_Init(mt6835, 10);


    // CS引脚设置为高电平(未选中)
    MT6835_CS_High(mt6835);

    // 短暂延时等待芯片就绪
    HAL_Delay(10);
}

/**
 * @brief  读取MT6835寄存器
 * @param  mt6835: MT6835句柄
 * @param  reg_addr: 寄存器地址(12位)
 * @return 寄存器值(8位)
 */
uint8_t MT6835_ReadRegister(MT6835_Handle *mt6835, uint16_t reg_addr)
{
    uint8_t tx_data[3];
    uint8_t rx_data[3];

    // 构建24位SPI帧: [命令4位][地址12位][数据8位]
    tx_data[0] = (MT6835_CMD_READ << 4) | ((reg_addr >> 8) & 0x0F);
    tx_data[1] = reg_addr & 0xFF;
    tx_data[2] = 0x00;  // 占位字节

    MT6835_CS_Low(mt6835);
    HAL_SPI_TransmitReceive(mt6835->hspi, tx_data, rx_data, 3, HAL_MAX_DELAY);
    MT6835_CS_High(mt6835);

    return rx_data[2];  // 返回的数据在第3个字节
}

/**
 * @brief  写入MT6835寄存器
 * @param  mt6835: MT6835句柄
 * @param  reg_addr: 寄存器地址(12位)
 * @param  data: 要写入的数据(8位)
 */
void MT6835_WriteRegister(MT6835_Handle *mt6835, uint16_t reg_addr, uint8_t data)
{
    uint8_t tx_data[3];

    // 构建24位SPI帧: [命令4位][地址12位][数据8位]
    tx_data[0] = (MT6835_CMD_WRITE << 4) | ((reg_addr >> 8) & 0x0F);
    tx_data[1] = reg_addr & 0xFF;
    tx_data[2] = data;

    MT6835_CS_Low(mt6835);
    HAL_SPI_Transmit(mt6835->hspi, tx_data, 3, HAL_MAX_DELAY);
    MT6835_CS_High(mt6835);
}

/**
 * @brief  读取角度(标准方式,分3次读取3个寄存器)
 * @param  mt6835: MT6835句柄
 * @return true: 成功, false: 失败
 */
bool MT6835_ReadAngle(MT6835_Handle *mt6835)
{
    uint8_t angle_h, angle_m, angle_l;

    // 读取3个角度寄存器
    angle_h = MT6835_ReadRegister(mt6835, MT6835_REG_ANGLE_H);
    angle_m = MT6835_ReadRegister(mt6835, MT6835_REG_ANGLE_M);
    angle_l = MT6835_ReadRegister(mt6835, MT6835_REG_ANGLE_L);

    // 组合成21位角度值
    mt6835->angle_raw = ((uint32_t)angle_h << 13) |
                        ((uint32_t)angle_m << 5) |
                        ((uint32_t)angle_l)>>3;
    mt6835->angle_raw &= MT6835_ANGLE_MAX;  // 确保是21位

    // 转换为角度(度)
    mt6835->angle_deg = (float)mt6835->angle_raw * 360.0f / (MT6835_ANGLE_MAX + 1);

    // 转换为弧度
    mt6835->angle_rad = mt6835->angle_deg * PI / 180.0f;

    return true;
}

/**
 * @brief  快速读取角度(使用连续读取模式)
 * @param  mt6835: MT6835句柄
 * @return true: 成功, false: 失败
 */
bool MT6835_ReadAngleFast(MT6835_Handle *mt6835)
{
    uint8_t tx_data[6];
    uint8_t rx_data[6];

    // 使用连续读取命令
    // 第一帧: 读0x003
    tx_data[0] = (MT6835_CMD_READ << 4) | 0x00;
    tx_data[1] = 0x03;
    tx_data[2] = 0x00;

    // 第二帧: 读0x004
    tx_data[3] = (MT6835_CMD_READ << 4) | 0x00;
    tx_data[4] = 0x04;
    tx_data[5] = 0x00;

    MT6835_CS_Low(mt6835);
    HAL_SPI_TransmitReceive(mt6835->hspi, tx_data, rx_data, 6, HAL_MAX_DELAY);
    MT6835_CS_High(mt6835);

    // 读取第三个寄存器
    uint8_t angle_l = MT6835_ReadRegister(mt6835, MT6835_REG_ANGLE_L);

    // 组合角度值
    mt6835->angle_raw = ((uint32_t)rx_data[2] << 13) |
                        ((uint32_t)rx_data[5] << 5) |
                        ((uint32_t)angle_l >> 3);
    mt6835->angle_raw &= MT6835_ANGLE_MAX;

    // 转换为角度
    mt6835->angle_deg = (float)mt6835->angle_raw * 360.0f / (MT6835_ANGLE_MAX + 1);
    mt6835->angle_rad = mt6835->angle_deg * PI / 180.0f;

    return true;
}

/**
 * @brief  获取角度值(度)
 * @param  mt6835: MT6835句柄
 * @return 角度值(0-360度)
 */
float MT6835_GetAngleDegrees(MT6835_Handle *mt6835)
{
    return mt6835->angle_deg;
}

/**
 * @brief  获取角度值(弧度)
 * @param  mt6835: MT6835句柄
 * @return 角度值(0-2π弧度)
 */
float MT6835_GetAngleRadians(MT6835_Handle *mt6835)
{
    return mt6835->angle_rad;
}

/**
 * @brief  获取原始角度值
 * @param  mt6835: MT6835句柄
 * @return 21位原始角度值(0-2097151)
 */
uint32_t MT6835_GetAngleRaw(MT6835_Handle *mt6835)
{
    return mt6835->angle_raw;
}

/**
 * @brief  设置零位
 * @param  mt6835: MT6835句柄
 * @param  zero_pos: 零位值(12位)
 */
void MT6835_SetZeroPosition(MT6835_Handle *mt6835, uint16_t zero_pos)
{
    uint8_t zero_h = (zero_pos >> 8) & 0x0F;
    uint8_t zero_l = zero_pos & 0xFF;

    MT6835_WriteRegister(mt6835, MT6835_REG_ZERO_POS_H, zero_h);
    MT6835_WriteRegister(mt6835, MT6835_REG_ZERO_POS_L, zero_l);
}

#define ANGLE_THRESHOLD  270.0f  // 角度跳变检测阈值

/**
 * @brief  初始化电机角度计算器
 * @param  motor: 电机句柄
 * @param  gear_ratio: 减速比 (例如: 10表示10:1减速)
 */

void MT6835_MotorAngle_Init(MT6835_Handle *mt6835, float gear_ratio)
{


    mt6835->motor_angle.gear_ratio = gear_ratio;
    mt6835->motor_angle.motor_angle_deg = 0.0f;
    mt6835->motor_angle.motor_angle_raw = 0;
    mt6835->motor_angle.motor_turns = 0;
    mt6835->motor_angle.motor_total_angle = 0.0f;
    mt6835->motor_angle.output_angle_deg = 0.0f;
    mt6835->motor_angle.output_turns = 0;
    mt6835->motor_angle.output_total_angle = 0.0f;
    mt6835->motor_angle.prev_motor_angle =0.0f;
    mt6835->motor_angle.is_first_read = true;
    eeprom_ReadFloat(EEPROM_LAST_ANGLE_ADDR, &mt6835->motor_angle.last_angle);

}

void MT6835_Angle_Update_with_last(MT6835_Handle *mt6835)
{
    // Motor_Angle_t motor = mt6835->motor_angle;
    MT6835_ReadAngleFast(mt6835);
    float encoder_angle = MT6835_GetAngleDegrees(mt6835)+mt6835->motor_angle.last_angle;
    mt6835->motor_angle.motor_angle_deg = encoder_angle;

    // 第一次读取，初始化
    if (mt6835->motor_angle.is_first_read)
    {
        mt6835->motor_angle.prev_motor_angle = encoder_angle;
        // mt6835->motor_angle.output_angle_deg = encoder_angle;
        mt6835->motor_angle.is_first_read = false;
        return;
    }

    // 计算角度变化
    float delta_angle = encoder_angle -mt6835->motor_angle.prev_motor_angle;

    // 检测角度跳变 (跨越0度/360度边界)
    if (delta_angle > ANGLE_THRESHOLD)
    {
        // 从359°跳到1° (逆时针跨越边界)
        delta_angle -= 360.0f;
        mt6835->motor_angle.motor_turns--;  // 反转时圈数减少
    }
    else if (delta_angle < -ANGLE_THRESHOLD)
    {
        // 从1°跳到359° (顺时针跨越边界)
        delta_angle += 360.0f;
        mt6835->motor_angle.motor_turns++;  // 正转时圈数增加
    }

    // 累积电机轴总角度
    mt6835->motor_angle.motor_total_angle += delta_angle;

    // 计算输出轴角度
    float output_total = mt6835->motor_angle.motor_total_angle / mt6835->motor_angle.gear_ratio;

    // 计算输出轴圈数和当前角度
    mt6835->motor_angle.output_turns = (int32_t)(output_total / 360.0f);
    mt6835->motor_angle.output_total_angle = output_total;
    mt6835->motor_angle.output_angle_deg = fmodf(output_total, 360.0f);

    // 处理负角度
    if (mt6835->motor_angle.output_angle_deg < 0)
    {
        mt6835->motor_angle.output_angle_deg += 360.0f;
    }

    // 保存当前角度用于下次计算
    mt6835->motor_angle.prev_motor_angle = encoder_angle;
}
void MT6835_Angle_Update(MT6835_Handle *mt6835)
{
    // Motor_Angle_t motor = mt6835->motor_angle;
    MT6835_ReadAngleFast(mt6835);
    float encoder_angle = MT6835_GetAngleDegrees(mt6835);
    mt6835->motor_angle.motor_angle_deg = encoder_angle;

    // 第一次读取，初始化
    // if (mt6835->motor_angle.is_first_read)
    // {
    //     mt6835->motor_angle.prev_motor_angle = encoder_angle;
    //     // mt6835->motor_angle.output_angle_deg = encoder_angle;
    //     mt6835->motor_angle.is_first_read = false;
    //     return;
    // }

    // 计算角度变化
    float delta_angle = encoder_angle -mt6835->motor_angle.prev_motor_angle;

    // 检测角度跳变 (跨越0度/360度边界)
    if (delta_angle > ANGLE_THRESHOLD)
    {
        // 从359°跳到1° (逆时针跨越边界)
        delta_angle -= 360.0f;
        mt6835->motor_angle.motor_turns--;  // 反转时圈数减少
    }
    else if (delta_angle < -ANGLE_THRESHOLD)
    {
        // 从1°跳到359° (顺时针跨越边界)
        delta_angle += 360.0f;
        mt6835->motor_angle.motor_turns++;  // 正转时圈数增加
    }

    // 累积电机轴总角度
    mt6835->motor_angle.motor_total_angle += delta_angle;

    // 计算输出轴角度
    float output_total = mt6835->motor_angle.motor_total_angle / mt6835->motor_angle.gear_ratio;

    // 计算输出轴圈数和当前角度
    mt6835->motor_angle.output_turns = (int32_t)(output_total / 360.0f);
    mt6835->motor_angle.output_total_angle = output_total;
    mt6835->motor_angle.output_angle_deg = fmodf(output_total, 360.0f);

    // 处理负角度
    if (mt6835->motor_angle.output_angle_deg < 0)
    {
        mt6835->motor_angle.output_angle_deg += 360.0f;
    }

    // 保存当前角度用于下次计算
    mt6835->motor_angle.prev_motor_angle = encoder_angle;
}

/**
 * @brief  获取输出轴当前角度 (单圈,0-360度)
 * @param  motor: 电机句柄
 * @return 输出轴角度
 */
float MT6835_GetMotorAngleDegrees(MT6835_Handle *mt6835)
{
    return mt6835->motor_angle.output_angle_deg;
}













void Motor_AngleCalc_Init(Motor_AngleCalc_Handle *motor, float gear_ratio)
{
    motor->gear_ratio = gear_ratio;
    motor->motor_angle_deg = 0.0f;
    motor->motor_angle_raw = 0;
    motor->motor_turns = 0;
    motor->motor_total_angle = 0.0f;
    motor->output_angle_deg = 0.0f;
    motor->output_turns = 0;
    motor->output_total_angle = 0.0f;
    motor->prev_motor_angle = 0.0f;
    motor->is_first_read = true;
}

/**
 * @brief  更新电机角度 (每次读取编码器后调用)
 * @param  motor: 电机句柄
 * @param  encoder_angle: 编码器读取的角度 (0-360度)
 */
void Motor_AngleCalc_Update(Motor_AngleCalc_Handle *motor, float encoder_angle)
{
    motor->motor_angle_deg = encoder_angle;

    // 第一次读取，初始化
    if (motor->is_first_read)
    {
        motor->prev_motor_angle = encoder_angle;
        motor->is_first_read = false;
        return;
    }

    // 计算角度变化
    float delta_angle = encoder_angle - motor->prev_motor_angle;

    // 检测角度跳变 (跨越0度/360度边界)
    if (delta_angle > ANGLE_THRESHOLD)
    {
        // 从359°跳到1° (逆时针跨越边界)
        delta_angle -= 360.0f;
        motor->motor_turns--;  // 反转时圈数减少
    }
    else if (delta_angle < -ANGLE_THRESHOLD)
    {
        // 从1°跳到359° (顺时针跨越边界)
        delta_angle += 360.0f;
        motor->motor_turns++;  // 正转时圈数增加
    }

    // 累积电机轴总角度
    motor->motor_total_angle += delta_angle;

    // 计算输出轴角度
    float output_total = motor->motor_total_angle / motor->gear_ratio;

    // 计算输出轴圈数和当前角度
    motor->output_turns = (int32_t)(output_total / 360.0f);
    motor->output_total_angle = output_total;
    motor->output_angle_deg = fmodf(output_total, 360.0f);

    // 处理负角度
    if (motor->output_angle_deg < 0)
    {
        motor->output_angle_deg += 360.0f;
    }

    // 保存当前角度用于下次计算
    motor->prev_motor_angle = encoder_angle;
}


/**
 * @brief  获取电机轴当前角度 (单圈,0-360度)
 * @param  motor: 电机句柄
 * @return 电机轴角度
 */
float Motor_GetMotorAngle(Motor_AngleCalc_Handle *motor)
{
    return motor->motor_angle_deg;
}

/**
 * @brief  获取电机轴总角度 (多圈累积)
 * @param  motor: 电机句柄
 * @return 电机轴累积角度
 */
float Motor_GetMotorTotalAngle(Motor_AngleCalc_Handle *motor)
{
    return motor->motor_total_angle;
}

/**
 * @brief  获取电机轴转过的圈数
 * @param  motor: 电机句柄
 * @return 圈数 (正数为顺时针,负数为逆时针)
 */
int32_t Motor_GetMotorTurns(Motor_AngleCalc_Handle *motor)
{
    return motor->motor_turns;
}

/**
 * @brief  获取输出轴当前角度 (单圈,0-360度)
 * @param  motor: 电机句柄
 * @return 输出轴角度
 */
float Motor_GetOutputAngle(Motor_AngleCalc_Handle *motor)
{
    return motor->output_angle_deg;
}

/**
 * @brief  获取输出轴总角度 (多圈累积)
 * @param  motor: 电机句柄
 * @return 输出轴累积角度
 */
float Motor_GetOutputTotalAngle(Motor_AngleCalc_Handle *motor)
{
    return motor->output_total_angle;
}

/**
 * @brief  获取输出轴转过的圈数
 * @param  motor: 电机句柄
 * @return 圈数 (正数为顺时针,负数为逆时针)
 */
int32_t Motor_GetOutputTurns(Motor_AngleCalc_Handle *motor)
{
    return motor->output_turns;
}

/**
 * @brief  获取输出轴角度(弧度)
 * @param  motor: 电机句柄
 * @return 输出轴角度(弧度,0-2π)
 */
float Motor_GetOutputAngleRad(Motor_AngleCalc_Handle *motor)
{
    return motor->output_angle_deg * M_PI / 180.0f;
}

/**
 * @brief  重置角度计数器
 * @param  motor: 电机句柄
 */
void Motor_AngleCalc_Reset(Motor_AngleCalc_Handle *motor)
{
    motor->motor_turns = 0;
    motor->motor_total_angle = 0.0f;
    motor->output_turns = 0;
    motor->output_total_angle = 0.0f;
    motor->is_first_read = true;
}

/**
 * @brief  设置当前位置为零点
 * @param  motor: 电机句柄
 * @note   保留已转过的圈数,只重置单圈角度偏移
 */
void Motor_SetZeroPosition(Motor_AngleCalc_Handle *motor)
{
    // 计算当前总角度
    float current_total = motor->motor_total_angle;

    // 减去当前单圈角度,保留整圈部分
    float full_rotations = (float)motor->motor_turns * 360.0f;
    motor->motor_total_angle = full_rotations;

    // 重新计算输出轴
    motor->output_total_angle = motor->motor_total_angle / motor->gear_ratio;
    motor->output_turns = (int32_t)(motor->output_total_angle / 360.0f);
    motor->output_angle_deg = fmodf(motor->output_total_angle, 360.0f);

    if (motor->output_angle_deg < 0)
    {
        motor->output_angle_deg += 360.0f;
    }
}

/**
 * @brief  完全重置到零点 (包括圈数)
 * @param  motor: 电机句柄
 */
void Motor_ResetToZero(Motor_AngleCalc_Handle *motor)
{
    Motor_AngleCalc_Reset(motor);
}

void Example1_BasicUsage(void)
{
    // extern SPI_HandleTypeDef hspi1;

    /* 全局变量 */
    MT6835_Handle mt6835;              // 编码器句柄

    // 初始化编码器
    MT6835_Init(&mt6835, &mt6835_spi, MT6835_SPI_CS_PORT, MT6835_SPI_CS_PIN);

    // 初始化角度计算器 (减速比10:1)
    // Motor_AngleCalc_Init(&motor, 10.0f);

    printf("[ok] init\r\n");

    while(1)
    {
        // // 1. 读取编码器角度
        // MT6835_ReadAngle(&mt6835);
        // float mt6835_angle = MT6835_GetAngleDegrees(&mt6835);
        //
        // // 2. 更新角度计算
        // Motor_AngleCalc_Update(&mt6835.motor_angle_calc, mt6835_angle);

        MT6835_Angle_Update(&mt6835);
        // 3. 获取输出轴角度
        float output_angle = MT6835_GetMotorAngleDegrees(&mt6835);


        // 4. 显示结果
        printf("motor angle: %.2f | actual angle: %.2f\r\n",
               mt6835.angle_deg, output_angle);

        HAL_Delay(10);
    }
}
