// ==================== as5600.h ====================
#ifndef AS5600_H
#define AS5600_H

#include "stm32f4xx_hal.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

// ==================== 版本信息 ====================
#define AS5600_LIB_VERSION "0.6.4_C"

// ==================== I2C地址 ====================
#define AS5600_DEFAULT_ADDRESS 0x36
#define AS5600L_DEFAULT_ADDRESS 0x40
#define AS5600_SW_DIRECTION_PIN 255

// ==================== 方向定义 ====================
#define AS5600_CLOCK_WISE 0        // 顺时针
#define AS5600_COUNTERCLOCK_WISE 1 // 逆时针

// ==================== 转换常数 ====================
#define AS5600_RAW_TO_DEGREES (360.0f / 4096.0f)             // 0.087890625
#define AS5600_DEGREES_TO_RAW (4096.0f / 360.0f)             // 11.377778
#define AS5600_RAW_TO_RADIANS (2.0f * 3.14159265f / 4096.0f) // 0.00153398
#define AS5600_RAW_TO_RPM (60.0f / 4096.0f)                  // 0.01464844

// ==================== 角速度模式 ====================
#define AS5600_MODE_DEGREES 0
#define AS5600_MODE_RADIANS 1
#define AS5600_MODE_RPM 2

// ==================== 错误码 ====================
#define AS5600_OK 0
#define AS5600_ERROR_I2C_READ_0 -100
#define AS5600_ERROR_I2C_READ_1 -101
#define AS5600_ERROR_I2C_READ_2 -102
#define AS5600_ERROR_I2C_READ_3 -103
#define AS5600_ERROR_I2C_WRITE_0 -200
#define AS5600_ERROR_I2C_WRITE_1 -201

// ==================== 配置常量 ====================
// 输出模式
#define AS5600_OUTMODE_ANALOG_100 0
#define AS5600_OUTMODE_ANALOG_90 1
#define AS5600_OUTMODE_PWM 2

// 电源模式
#define AS5600_POWERMODE_NOMINAL 0
#define AS5600_POWERMODE_LOW1 1
#define AS5600_POWERMODE_LOW2 2
#define AS5600_POWERMODE_LOW3 3

// PWM频率
#define AS5600_PWM_115 0
#define AS5600_PWM_230 1
#define AS5600_PWM_460 2
#define AS5600_PWM_920 3

// 磁滞
#define AS5600_HYST_OFF 0
#define AS5600_HYST_LSB1 1
#define AS5600_HYST_LSB2 2
#define AS5600_HYST_LSB3 3

// 慢速滤波器
#define AS5600_SLOW_FILT_16X 0
#define AS5600_SLOW_FILT_8X 1
#define AS5600_SLOW_FILT_4X 2
#define AS5600_SLOW_FILT_2X 3

// 快速滤波器
#define AS5600_FAST_FILT_NONE 0
#define AS5600_FAST_FILT_LSB6 1
#define AS5600_FAST_FILT_LSB7 2
#define AS5600_FAST_FILT_LSB9 3
#define AS5600_FAST_FILT_LSB18 4
#define AS5600_FAST_FILT_LSB21 5
#define AS5600_FAST_FILT_LSB24 6
#define AS5600_FAST_FILT_LSB10 7

// 看门狗
#define AS5600_WATCHDOG_OFF 0
#define AS5600_WATCHDOG_ON 1

// ==================== 寄存器地址 ====================
// 配置寄存器
#define AS5600_REG_ZMCO 0x00
#define AS5600_REG_ZPOS 0x01 // +0x02
#define AS5600_REG_MPOS 0x03 // +0x04
#define AS5600_REG_MANG 0x05 // +0x06
#define AS5600_REG_CONF 0x07 // +0x08

// 输出寄存器
#define AS5600_REG_RAW_ANGLE 0x0C // +0x0D
#define AS5600_REG_ANGLE 0x0E     // +0x0F

// I2C地址寄存器 (AS5600L)
#define AS5600_REG_I2CADDR 0x20
#define AS5600_REG_I2CUPDT 0x21

// 状态寄存器
#define AS5600_REG_STATUS 0x0B
#define AS5600_REG_AGC 0x1A
#define AS5600_REG_MAGNITUDE 0x1B // +0x1C
#define AS5600_REG_BURN 0xFF

// 状态位
#define AS5600_MAGNET_HIGH 0x08
#define AS5600_MAGNET_LOW 0x10
#define AS5600_MAGNET_DETECT 0x20

// 配置位掩码
#define AS5600_CONF_POWER_MODE 0x03
#define AS5600_CONF_HYSTERESIS 0x0C
#define AS5600_CONF_OUTPUT_MODE 0x30
#define AS5600_CONF_PWM_FREQUENCY 0xC0
#define AS5600_CONF_SLOW_FILTER 0x03
#define AS5600_CONF_FAST_FILTER 0x1C
#define AS5600_CONF_WATCH_DOG 0x20

// ==================== AS5600 结构体 ====================
typedef struct
{
    // I2C相关
    I2C_HandleTypeDef *hi2c; // STM32 HAL I2C句柄
    uint8_t address;         // I2C地址

    // 方向控制
    uint8_t directionPin;        // 方向控制引脚
    GPIO_TypeDef *directionPort; // 方向控制端口
    uint8_t direction;           // 当前方向

    // 错误状态
    int error; // 最后的错误码

    // 角速度测量
    uint32_t lastMeasurement; // 上次测量时间 (us)
    int16_t lastAngle;        // 上次角度
    int16_t lastReadAngle;    // 上次读取的角度

    // 偏移
    uint16_t offset; // 角度偏移

    // 累积位置
    int32_t position;     // 累积位置
    int16_t lastPosition; // 上次位置
    float angle;
    float velocity;
} AS5600_t;

// ==================== 函数声明 ====================

// 初始化和连接
bool AS5600_Init(AS5600_t *sensor, I2C_HandleTypeDef *hi2c, uint8_t address);
bool AS5600_InitWithPin(AS5600_t *sensor, I2C_HandleTypeDef *hi2c, uint8_t address, GPIO_TypeDef *port, uint8_t pin);
bool AS5600_IsConnected(AS5600_t *sensor);
uint8_t AS5600_GetAddress(AS5600_t *sensor);

// 方向控制
void AS5600_SetDirection(AS5600_t *sensor, uint8_t direction);
uint8_t AS5600_GetDirection(AS5600_t *sensor);

// 配置寄存器
uint8_t AS5600_GetZMCO(AS5600_t *sensor);
bool AS5600_SetZPosition(AS5600_t *sensor, uint16_t value);
uint16_t AS5600_GetZPosition(AS5600_t *sensor);
bool AS5600_SetMPosition(AS5600_t *sensor, uint16_t value);
uint16_t AS5600_GetMPosition(AS5600_t *sensor);
bool AS5600_SetMaxAngle(AS5600_t *sensor, uint16_t value);
uint16_t AS5600_GetMaxAngle(AS5600_t *sensor);

// 配置寄存器（整体）
bool AS5600_SetConfigure(AS5600_t *sensor, uint16_t value);
uint16_t AS5600_GetConfigure(AS5600_t *sensor);

// 配置寄存器（详细）
bool AS5600_SetPowerMode(AS5600_t *sensor, uint8_t powerMode);
uint8_t AS5600_GetPowerMode(AS5600_t *sensor);
bool AS5600_SetHysteresis(AS5600_t *sensor, uint8_t hysteresis);
uint8_t AS5600_GetHysteresis(AS5600_t *sensor);
bool AS5600_SetOutputMode(AS5600_t *sensor, uint8_t outputMode);
uint8_t AS5600_GetOutputMode(AS5600_t *sensor);
bool AS5600_SetPWMFrequency(AS5600_t *sensor, uint8_t pwmFreq);
uint8_t AS5600_GetPWMFrequency(AS5600_t *sensor);
bool AS5600_SetSlowFilter(AS5600_t *sensor, uint8_t mask);
uint8_t AS5600_GetSlowFilter(AS5600_t *sensor);
bool AS5600_SetFastFilter(AS5600_t *sensor, uint8_t mask);
uint8_t AS5600_GetFastFilter(AS5600_t *sensor);
bool AS5600_SetWatchDog(AS5600_t *sensor, uint8_t mask);
uint8_t AS5600_GetWatchDog(AS5600_t *sensor);

// 读取角度
uint16_t AS5600_RawAngle(AS5600_t *sensor);
uint16_t AS5600_ReadAngle(AS5600_t *sensor);
float AS5600_ReadAngleDegrees(AS5600_t *sensor);
float AS5600_ReadAngleRadians(AS5600_t *sensor);

// 偏移设置
bool AS5600_SetOffset(AS5600_t *sensor, float degrees);
float AS5600_GetOffset(AS5600_t *sensor);
bool AS5600_IncreaseOffset(AS5600_t *sensor, float degrees);

// 状态寄存器
uint8_t AS5600_ReadStatus(AS5600_t *sensor);
uint8_t AS5600_ReadAGC(AS5600_t *sensor);
uint16_t AS5600_ReadMagnitude(AS5600_t *sensor);
bool AS5600_DetectMagnet(AS5600_t *sensor);
bool AS5600_MagnetTooStrong(AS5600_t *sensor);
bool AS5600_MagnetTooWeak(AS5600_t *sensor);

// 角速度
float AS5600_GetAngularSpeed(AS5600_t *sensor, uint8_t mode, bool update);

// 累积位置
int32_t AS5600_GetCumulativePosition(AS5600_t *sensor, bool update);
int32_t AS5600_GetRevolutions(AS5600_t *sensor);
int32_t AS5600_ResetPosition(AS5600_t *sensor, int32_t position);
int32_t AS5600_ResetCumulativePosition(AS5600_t *sensor, int32_t position);

// 错误处理
int AS5600_LastError(AS5600_t *sensor);

// 底层I2C操作（内部使用）
uint8_t AS5600_ReadReg(AS5600_t *sensor, uint8_t reg);
uint16_t AS5600_ReadReg2(AS5600_t *sensor, uint8_t reg);
uint8_t AS5600_WriteReg(AS5600_t *sensor, uint8_t reg, uint8_t value);
uint8_t AS5600_WriteReg2(AS5600_t *sensor, uint8_t reg, uint16_t value);

void AS5600_Example_Init(void);

void AS5600_Example_CumulativePosition(void);

void AS5600_Example_ReadSpeed(void);
void AS5600_Example_ReadAngle(void);

void Encoder_AS5600_Init(AS5600_t *as5600, I2C_HandleTypeDef *hi2c);
// bool AS5600_Init(AS5600_t *sensor, I2C_HandleTypeDef *hi2c);
void Encoder_AS5600_ReadAngle(AS5600_t *as5600);
#endif // AS5600_H
//
