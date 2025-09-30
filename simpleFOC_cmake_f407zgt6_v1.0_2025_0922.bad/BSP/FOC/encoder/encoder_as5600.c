#include "stm32f4xx_hal_i2c.h"
#include <stdbool.h>
#include "encoder_as5600.h"

// ==================== 初始化和连接 ====================

bool AS5600_Init(AS5600_t *sensor, I2C_HandleTypeDef *hi2c, uint8_t address)
{
    if (!sensor || !hi2c)
    {
        return false;
    }

    sensor->hi2c = hi2c;
    sensor->address = address;
    sensor->directionPin = AS5600_SW_DIRECTION_PIN;
    sensor->directionPort = NULL;
    sensor->direction = AS5600_CLOCK_WISE;
    sensor->error = AS5600_OK;
    sensor->lastMeasurement = 0;
    sensor->lastAngle = 0;
    sensor->lastReadAngle = 0;
    sensor->offset = 0;
    sensor->position = 0;
    sensor->lastPosition = 0;

    // 设置默认方向
    AS5600_SetDirection(sensor, AS5600_CLOCK_WISE);

    // 检查连接
    if (!AS5600_IsConnected(sensor))
    {
        printf("[error] as5600 is not connected!\n");
        return false;
    }
    printf("[ok] as5600 is connected!\n");

    return true;
}

bool AS5600_InitWithPin(AS5600_t *sensor, I2C_HandleTypeDef *hi2c, uint8_t address, GPIO_TypeDef *port, uint8_t pin)
{
    if (!AS5600_Init(sensor, hi2c, address))
        return false;

    sensor->directionPin = pin;
    sensor->directionPort = port;

    // 配置GPIO为输出
    if (port != NULL)
    {
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = (1 << pin);
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(port, &GPIO_InitStruct);
    }

    return true;
}

bool AS5600_IsConnected(AS5600_t *sensor)
{
    if (!sensor || !sensor->hi2c)
    {
        return false;
    }

    HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(sensor->hi2c, sensor->address << 1, 2, 100);
    // printf("status = %d\n", status);
    return (status == HAL_OK);
}

uint8_t AS5600_GetAddress(AS5600_t *sensor)
{
    if (!sensor)
        return 0;
    return sensor->address;
}

// ==================== 方向控制 ====================

void AS5600_SetDirection(AS5600_t *sensor, uint8_t direction)
{
    if (!sensor)
        return;

    sensor->direction = direction;
    if (sensor->directionPin != AS5600_SW_DIRECTION_PIN && sensor->directionPort != NULL)
    {
        HAL_GPIO_WritePin(sensor->directionPort, (1 << sensor->directionPin), direction ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}

uint8_t AS5600_GetDirection(AS5600_t *sensor)
{
    if (!sensor)
        return 0;

    if (sensor->directionPin != AS5600_SW_DIRECTION_PIN && sensor->directionPort != NULL)
    {
        sensor->direction = HAL_GPIO_ReadPin(sensor->directionPort, (1 << sensor->directionPin));
    }
    return sensor->direction;
}

// ==================== 配置寄存器 ====================

uint8_t AS5600_GetZMCO(AS5600_t *sensor)
{
    if (!sensor)
        return 0;
    return AS5600_ReadReg(sensor, AS5600_REG_ZMCO);
}

bool AS5600_SetZPosition(AS5600_t *sensor, uint16_t value)
{
    if (!sensor)
        return false;
    if (value > 0x0FFF)
        return false;
    AS5600_WriteReg2(sensor, AS5600_REG_ZPOS, value);
    return true;
}

uint16_t AS5600_GetZPosition(AS5600_t *sensor)
{
    if (!sensor)
        return 0;
    return AS5600_ReadReg2(sensor, AS5600_REG_ZPOS) & 0x0FFF;
}

bool AS5600_SetMPosition(AS5600_t *sensor, uint16_t value)
{
    if (!sensor)
        return false;
    if (value > 0x0FFF)
        return false;
    AS5600_WriteReg2(sensor, AS5600_REG_MPOS, value);
    return true;
}

uint16_t AS5600_GetMPosition(AS5600_t *sensor)
{
    if (!sensor)
        return 0;
    return AS5600_ReadReg2(sensor, AS5600_REG_MPOS) & 0x0FFF;
}

bool AS5600_SetMaxAngle(AS5600_t *sensor, uint16_t value)
{
    if (!sensor)
        return false;
    if (value > 0x0FFF)
        return false;
    AS5600_WriteReg2(sensor, AS5600_REG_MANG, value);
    return true;
}

uint16_t AS5600_GetMaxAngle(AS5600_t *sensor)
{
    if (!sensor)
        return 0;
    return AS5600_ReadReg2(sensor, AS5600_REG_MANG) & 0x0FFF;
}

// ==================== 配置寄存器（整体） ====================

bool AS5600_SetConfigure(AS5600_t *sensor, uint16_t value)
{
    if (!sensor)
        return false;
    if (value > 0x3FFF)
        return false;
    AS5600_WriteReg2(sensor, AS5600_REG_CONF, value);
    return true;
}

uint16_t AS5600_GetConfigure(AS5600_t *sensor)
{
    if (!sensor)
        return 0;
    return AS5600_ReadReg2(sensor, AS5600_REG_CONF) & 0x3FFF;
}

// ==================== 配置寄存器（详细） ====================

bool AS5600_SetPowerMode(AS5600_t *sensor, uint8_t powerMode)
{
    if (!sensor)
        return false;
    if (powerMode > 3)
        return false;

    uint8_t value = AS5600_ReadReg(sensor, AS5600_REG_CONF + 1);
    value &= ~AS5600_CONF_POWER_MODE;
    value |= powerMode;
    AS5600_WriteReg(sensor, AS5600_REG_CONF + 1, value);
    return true;
}

uint8_t AS5600_GetPowerMode(AS5600_t *sensor)
{
    if (!sensor)
        return 0;
    return AS5600_ReadReg(sensor, AS5600_REG_CONF + 1) & 0x03;
}

bool AS5600_SetHysteresis(AS5600_t *sensor, uint8_t hysteresis)
{
    if (!sensor)
        return false;
    if (hysteresis > 3)
        return false;

    uint8_t value = AS5600_ReadReg(sensor, AS5600_REG_CONF + 1);
    value &= ~AS5600_CONF_HYSTERESIS;
    value |= (hysteresis << 2);
    AS5600_WriteReg(sensor, AS5600_REG_CONF + 1, value);
    return true;
}

uint8_t AS5600_GetHysteresis(AS5600_t *sensor)
{
    if (!sensor)
        return 0;
    return (AS5600_ReadReg(sensor, AS5600_REG_CONF + 1) >> 2) & 0x03;
}

bool AS5600_SetOutputMode(AS5600_t *sensor, uint8_t outputMode)
{
    if (!sensor)
        return false;
    if (outputMode > 2)
        return false;

    uint8_t value = AS5600_ReadReg(sensor, AS5600_REG_CONF + 1);
    value &= ~AS5600_CONF_OUTPUT_MODE;
    value |= (outputMode << 4);
    AS5600_WriteReg(sensor, AS5600_REG_CONF + 1, value);
    return true;
}

uint8_t AS5600_GetOutputMode(AS5600_t *sensor)
{
    if (!sensor)
        return 0;
    return (AS5600_ReadReg(sensor, AS5600_REG_CONF + 1) >> 4) & 0x03;
}

bool AS5600_SetPWMFrequency(AS5600_t *sensor, uint8_t pwmFreq)
{
    if (!sensor)
        return false;
    if (pwmFreq > 3)
        return false;

    uint8_t value = AS5600_ReadReg(sensor, AS5600_REG_CONF + 1);
    value &= ~AS5600_CONF_PWM_FREQUENCY;
    value |= (pwmFreq << 6);
    AS5600_WriteReg(sensor, AS5600_REG_CONF + 1, value);
    return true;
}

uint8_t AS5600_GetPWMFrequency(AS5600_t *sensor)
{
    if (!sensor)
        return 0;
    return (AS5600_ReadReg(sensor, AS5600_REG_CONF + 1) >> 6) & 0x03;
}

bool AS5600_SetSlowFilter(AS5600_t *sensor, uint8_t mask)
{
    if (!sensor)
        return false;
    if (mask > 3)
        return false;

    uint8_t value = AS5600_ReadReg(sensor, AS5600_REG_CONF);
    value &= ~AS5600_CONF_SLOW_FILTER;
    value |= mask;
    AS5600_WriteReg(sensor, AS5600_REG_CONF, value);
    return true;
}

uint8_t AS5600_GetSlowFilter(AS5600_t *sensor)
{
    if (!sensor)
        return 0;
    return AS5600_ReadReg(sensor, AS5600_REG_CONF) & 0x03;
}

bool AS5600_SetFastFilter(AS5600_t *sensor, uint8_t mask)
{
    if (!sensor)
        return false;
    if (mask > 7)
        return false;

    uint8_t value = AS5600_ReadReg(sensor, AS5600_REG_CONF);
    value &= ~AS5600_CONF_FAST_FILTER;
    value |= (mask << 2);
    AS5600_WriteReg(sensor, AS5600_REG_CONF, value);
    return true;
}

uint8_t AS5600_GetFastFilter(AS5600_t *sensor)
{
    if (!sensor)
        return 0;
    return (AS5600_ReadReg(sensor, AS5600_REG_CONF) >> 2) & 0x07;
}

bool AS5600_SetWatchDog(AS5600_t *sensor, uint8_t mask)
{
    if (!sensor)
        return false;
    if (mask > 1)
        return false;

    uint8_t value = AS5600_ReadReg(sensor, AS5600_REG_CONF);
    value &= ~AS5600_CONF_WATCH_DOG;
    value |= (mask << 5);
    AS5600_WriteReg(sensor, AS5600_REG_CONF, value);
    return true;
}

uint8_t AS5600_GetWatchDog(AS5600_t *sensor)
{
    if (!sensor)
        return 0;
    return (AS5600_ReadReg(sensor, AS5600_REG_CONF) >> 5) & 0x01;
}

// ==================== 读取角度 ====================

uint16_t AS5600_RawAngle(AS5600_t *sensor)
{
    if (!sensor)
        return 0;

    int16_t value = AS5600_ReadReg2(sensor, AS5600_REG_RAW_ANGLE);
    if (sensor->offset > 0)
        value += sensor->offset;
    value &= 0x0FFF;

    if ((sensor->directionPin == AS5600_SW_DIRECTION_PIN) && (sensor->direction == AS5600_COUNTERCLOCK_WISE))
    {
        value = (4096 - value) & 0x0FFF;
    }

    return value;
}

uint16_t AS5600_ReadAngle(AS5600_t *sensor)
{
    if (!sensor)
        return 0;

    uint16_t value = AS5600_ReadReg2(sensor, AS5600_REG_ANGLE);
    if (sensor->error != AS5600_OK)
    {
        return sensor->lastReadAngle;
    }

    if (sensor->offset > 0)
        value += sensor->offset;
    value &= 0x0FFF;

    if ((sensor->directionPin == AS5600_SW_DIRECTION_PIN) && (sensor->direction == AS5600_COUNTERCLOCK_WISE))
    {
        value = (4096 - value) & 0x0FFF;
    }

    sensor->lastReadAngle = value;
    return value;
}

float AS5600_ReadAngleDegrees(AS5600_t *sensor)
{
    uint16_t raw = AS5600_ReadAngle(sensor);
    return raw * AS5600_RAW_TO_DEGREES;
}

float AS5600_ReadAngleRadians(AS5600_t *sensor)
{
    uint16_t raw = AS5600_ReadAngle(sensor);
    return raw * AS5600_RAW_TO_RADIANS;
}

// ==================== 偏移设置 ====================

bool AS5600_SetOffset(AS5600_t *sensor, float degrees)
{
    if (!sensor)
        return false;
    if (fabs(degrees) > 36000)
        return false;

    bool neg = (degrees < 0);
    if (neg)
        degrees = -degrees;

    uint16_t offset = (uint16_t)(degrees * AS5600_DEGREES_TO_RAW + 0.5f);
    offset &= 0x0FFF;
    if (neg)
        offset = (4096 - offset) & 0x0FFF;

    sensor->offset = offset;
    return true;
}

float AS5600_GetOffset(AS5600_t *sensor)
{
    if (!sensor)
        return 0;
    return sensor->offset * AS5600_RAW_TO_DEGREES;
}

bool AS5600_IncreaseOffset(AS5600_t *sensor, float degrees)
{
    if (!sensor)
        return false;
    return AS5600_SetOffset(sensor, (sensor->offset * AS5600_RAW_TO_DEGREES) + degrees);
}

// ==================== 状态寄存器 ====================

uint8_t AS5600_ReadStatus(AS5600_t *sensor)
{
    if (!sensor)
        return 0;
    return AS5600_ReadReg(sensor, AS5600_REG_STATUS);
}

uint8_t AS5600_ReadAGC(AS5600_t *sensor)
{
    if (!sensor)
        return 0;
    return AS5600_ReadReg(sensor, AS5600_REG_AGC);
}

uint16_t AS5600_ReadMagnitude(AS5600_t *sensor)
{
    if (!sensor)
        return 0;
    return AS5600_ReadReg2(sensor, AS5600_REG_MAGNITUDE) & 0x0FFF;
}

bool AS5600_DetectMagnet(AS5600_t *sensor)
{
    if (!sensor)
        return false;
    return (AS5600_ReadStatus(sensor) & AS5600_MAGNET_DETECT) > 0;
}

bool AS5600_MagnetTooStrong(AS5600_t *sensor)
{
    if (!sensor)
        return false;
    return (AS5600_ReadStatus(sensor) & AS5600_MAGNET_HIGH) > 0;
}

bool AS5600_MagnetTooWeak(AS5600_t *sensor)
{
    if (!sensor)
        return false;
    return (AS5600_ReadStatus(sensor) & AS5600_MAGNET_LOW) > 0;
}

// ==================== 角速度 ====================

// 获取微秒级时间戳（需要根据具体MCU实现）
static uint32_t getMicros(void)
{
    // STM32实现示例
    return HAL_GetTick() * 1000 + (SysTick->LOAD - SysTick->VAL) / (SystemCoreClock / 1000000);
}

float AS5600_GetAngularSpeed(AS5600_t *sensor, uint8_t mode, bool update)
{
    if (!sensor)
        return 0;

    if (update)
    {
        sensor->lastReadAngle = AS5600_ReadAngle(sensor);
        if (sensor->error != AS5600_OK)
        {
            return 0; // 或者返回 NAN
        }
    }

    uint32_t now = getMicros();
    int angle = sensor->lastReadAngle;
    uint32_t deltaT = now - sensor->lastMeasurement;
    printf("get angle time = %d\n", deltaT);
    int deltaA = angle - sensor->lastAngle;

    // 假设两次测量之间旋转不超过180度
    if (deltaA > 2048)
        deltaA -= 4096;
    else if (deltaA < -2048)
        deltaA += 4096;

    float speed = (deltaA * 1000000.0f) / deltaT;

    // 记住最后的时间和角度
    sensor->lastMeasurement = now;
    sensor->lastAngle = angle;

    // 返回弧度、RPM或度数
    if (mode == AS5600_MODE_RADIANS)
    {
        return speed * AS5600_RAW_TO_RADIANS;
    }
    if (mode == AS5600_MODE_RPM)
    {
        return speed * AS5600_RAW_TO_RPM;
    }
    // 默认返回度数
    return speed * AS5600_RAW_TO_DEGREES;
}

// ==================== 累积位置 ====================

int32_t AS5600_GetCumulativePosition(AS5600_t *sensor, bool update)
{
    if (!sensor)
        return 0;

    if (update)
    {
        sensor->lastReadAngle = AS5600_ReadAngle(sensor);
        if (sensor->error != AS5600_OK)
        {
            return sensor->position; // 返回最后已知位置
        }
    }

    int16_t value = sensor->lastReadAngle;

    // 整圈顺时针旋转？
    // 少于半圈
    if ((sensor->lastPosition > 2048) && (value < (sensor->lastPosition - 2048)))
    {
        sensor->position = sensor->position + 4096 - sensor->lastPosition + value;
    }
    // 整圈逆时针旋转？
    // 少于半圈
    else if ((value > 2048) && (sensor->lastPosition < (value - 2048)))
    {
        sensor->position = sensor->position - 4096 - sensor->lastPosition + value;
    }
    else
    {
        sensor->position = sensor->position - sensor->lastPosition + value;
    }
    sensor->lastPosition = value;

    return sensor->position;
}

int32_t AS5600_GetRevolutions(AS5600_t *sensor)
{
    if (!sensor)
        return 0;

    int32_t p = sensor->position >> 12; // 除以4096
    if (p < 0)
        p++; // 修正负值
    return p;
}

int32_t AS5600_ResetPosition(AS5600_t *sensor, int32_t position)
{
    if (!sensor)
        return 0;

    int32_t old = sensor->position;
    sensor->position = position;
    return old;
}

int32_t AS5600_ResetCumulativePosition(AS5600_t *sensor, int32_t position)
{
    if (!sensor)
        return 0;

    sensor->lastPosition = AS5600_ReadAngle(sensor);
    int32_t old = sensor->position;
    sensor->position = position;
    return old;
}

// ==================== 错误处理 ====================

int AS5600_LastError(AS5600_t *sensor)
{
    if (!sensor)
        return AS5600_ERROR_I2C_READ_0;

    int value = sensor->error;
    sensor->error = AS5600_OK;
    return value;
}

// ==================== 底层I2C操作 ====================

uint8_t AS5600_ReadReg(AS5600_t *sensor, uint8_t reg)
{
    if (!sensor || !sensor->hi2c)
    {
        if (sensor)
            sensor->error = AS5600_ERROR_I2C_READ_0;
        return 0;
    }

    sensor->error = AS5600_OK;
    uint8_t data = 0;

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(sensor->hi2c, sensor->address << 1, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);

    if (status != HAL_OK)
    {
        sensor->error = AS5600_ERROR_I2C_READ_1;
        return 0;
    }

    return data;
}

uint16_t AS5600_ReadReg2(AS5600_t *sensor, uint8_t reg)
{
    if (!sensor || !sensor->hi2c)
    {
        if (sensor)
            sensor->error = AS5600_ERROR_I2C_READ_2;
        return 0;
    }

    sensor->error = AS5600_OK;
    uint8_t data[2] = {0, 0};

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(sensor->hi2c, sensor->address << 1, reg, I2C_MEMADD_SIZE_8BIT, data, 2, 100);

    if (status != HAL_OK)
    {
        sensor->error = AS5600_ERROR_I2C_READ_3;
        return 0;
    }

    uint16_t value = (data[0] << 8) | data[1];
    return value;
}

uint8_t AS5600_WriteReg(AS5600_t *sensor, uint8_t reg, uint8_t value)
{
    if (!sensor || !sensor->hi2c)
    {
        if (sensor)
            sensor->error = AS5600_ERROR_I2C_WRITE_0;
        return AS5600_ERROR_I2C_WRITE_0;
    }

    sensor->error = AS5600_OK;

    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(sensor->hi2c, sensor->address << 1, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 100);

    if (status != HAL_OK)
    {
        sensor->error = AS5600_ERROR_I2C_WRITE_0;
    }

    return sensor->error;
}

uint8_t AS5600_WriteReg2(AS5600_t *sensor, uint8_t reg, uint16_t value)
{
    if (!sensor || !sensor->hi2c)
    {
        if (sensor)
            sensor->error = AS5600_ERROR_I2C_WRITE_0;
        return AS5600_ERROR_I2C_WRITE_0;
    }

    sensor->error = AS5600_OK;
    uint8_t data[2];
    data[0] = (value >> 8) & 0xFF;
    data[1] = value & 0xFF;

    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(sensor->hi2c, sensor->address << 1, reg, I2C_MEMADD_SIZE_8BIT, data, 2, 100);

    if (status != HAL_OK)
    {
        sensor->error = AS5600_ERROR_I2C_WRITE_1;
    }

    return sensor->error;
}

// ==================== AS5600L 扩展功能 ====================

typedef struct
{
    AS5600_t base; // 继承基础AS5600功能
} AS5600L_t;

bool AS5600L_Init(AS5600L_t *sensor, I2C_HandleTypeDef *hi2c, uint8_t address)
{
    if (!sensor)
        return false;

    // 使用AS5600L的默认地址或自定义地址
    if (address == 0)
    {
        address = AS5600L_DEFAULT_ADDRESS;
    }

    return AS5600_Init(&sensor->base, hi2c, address);
}

bool AS5600L_SetAddress(AS5600L_t *sensor, uint8_t address)
{
    if (!sensor)
        return false;

    // 跳过保留的I2C地址
    if ((address < 8) || (address > 119))
        return false;

    // 注意地址需要左移1位
    AS5600_WriteReg(&sensor->base, AS5600_REG_I2CADDR, address << 1);
    AS5600_WriteReg(&sensor->base, AS5600_REG_I2CUPDT, address << 1);

    // 记住新地址
    sensor->base.address = address;
    return true;
}

bool AS5600L_SetI2CUPDT(AS5600L_t *sensor, uint8_t address)
{
    if (!sensor)
        return false;

    // 跳过保留的I2C地址
    if ((address < 8) || (address > 119))
        return false;

    AS5600_WriteReg(&sensor->base, AS5600_REG_I2CUPDT, address << 1);
    return true;
}

uint8_t AS5600L_GetI2CUPDT(AS5600L_t *sensor)
{
    if (!sensor)
        return 0;

    return (AS5600_ReadReg(&sensor->base, AS5600_REG_I2CUPDT) >> 1);
}

void Encoder_AS5600_Init(AS5600_t *as5600, I2C_HandleTypeDef *hi2c)
{
    // 初始化I2C（假设已经在CubeMX中配置）
    // extern I2C_HandleTypeDef hi2c2;

    // ========== AS5600 初始化 ==========
    printf("this is as5600 init test\n");
    printf("Initializing AS5600...\n");

    // 方法1：不使用方向引脚（软件控制方向）
    if (AS5600_Init(as5600, hi2c, AS5600_DEFAULT_ADDRESS))
    {
        printf("AS5600 initialized successfully\n");
    }
    else
    {
        printf("AS5600 initialization failed\n");
        return;
    }

    // 方法2：使用方向引脚（硬件控制方向）
    // if (AS5600_InitWithPin(&as5600, &hi2c1, AS5600_DEFAULT_ADDRESS,
    //                        GPIOA, 5)) {  // PA5作为方向引脚
    //     printf("AS5600 initialized with direction pin\n");
    // }

    // 检查磁铁
    if (AS5600_DetectMagnet(as5600))
    {
        printf("Magnet detected\n");

        if (AS5600_MagnetTooStrong(as5600))
        {
            printf("Warning: Magnet too strong\n");
        }
        if (AS5600_MagnetTooWeak(as5600))
        {
            printf("Warning: Magnet too weak\n");
        }
    }
    else
    {
        printf("No magnet detected\n");
    }

    // 配置AS5600
    AS5600_SetPowerMode(as5600, AS5600_POWERMODE_NOMINAL);
    AS5600_SetHysteresis(as5600, AS5600_HYST_LSB1);
    AS5600_SetSlowFilter(as5600, AS5600_SLOW_FILT_4X);
    AS5600_SetFastFilter(as5600, AS5600_FAST_FILT_LSB6);

    // 设置零位
    uint16_t currentAngle = AS5600_RawAngle(as5600);
    AS5600_SetZPosition(as5600, currentAngle);
    printf("Zero position set to: %d\n", currentAngle);

    // ========== AS5600L 初始化（如果使用） ==========
    // if (AS5600L_Init(&magneticEnco
    // derL, &hi2c1, AS5600L_DEFAULT_ADDRESS)) {
    //     printf("AS5600L initialized\n");
    //     // 可以更改I2C地址
    //     // AS5600L_SetAddress(&as5600L, 0x41);
    // }
}

void Encoder_AS5600_ReadAngle(AS5600_t *as5600)
{
    // 读取原始角度（0-4095）
    uint16_t rawAngle = AS5600_RawAngle(as5600);

    // 读取角度（带偏移补偿）
    uint16_t angle = AS5600_ReadAngle(as5600);

    // 读取角度（度数）
    float angleDegrees = AS5600_ReadAngleDegrees(as5600);

    // 读取角度（弧度）
    float angleRadians = AS5600_ReadAngleRadians(as5600);

    printf("Raw: %d, Angle: %d, Degrees: %.2f, Radians: %.4f\n", rawAngle, angle, angleDegrees, angleRadians);

    // 读取状态信息
    uint8_t agc = AS5600_ReadAGC(as5600);
    uint16_t magnitude = AS5600_ReadMagnitude(as5600);
    printf("AGC: %d, Magnitude: %d\n", agc, magnitude);
    printf("this is reading angle\n");
}
//=======================================================================================
// 全局变量
AS5600_t magneticEncoder;
// 初始化示例
void AS5600_Example_Init(void)
{
    // 初始化I2C（假设已经在CubeMX中配置）
    extern I2C_HandleTypeDef hi2c2;

    // ========== AS5600 初始化 ==========
    printf("Initializing AS5600...\n");

    // 方法1：不使用方向引脚（软件控制方向）
    if (AS5600_Init(&magneticEncoder, &hi2c2, AS5600_DEFAULT_ADDRESS))
    {
        printf("AS5600 initialized successfully\n");
    }
    else
    {
        printf("AS5600 initialization failed\n");
        return;
    }

    // 方法2：使用方向引脚（硬件控制方向）
    // if (AS5600_InitWithPin(&magneticEncoder, &hi2c1, AS5600_DEFAULT_ADDRESS,
    //                        GPIOA, 5)) {  // PA5作为方向引脚
    //     printf("AS5600 initialized with direction pin\n");
    // }

    // 检查磁铁
    if (AS5600_DetectMagnet(&magneticEncoder))
    {
        printf("Magnet detected\n");

        if (AS5600_MagnetTooStrong(&magneticEncoder))
        {
            printf("Warning: Magnet too strong\n");
        }
        if (AS5600_MagnetTooWeak(&magneticEncoder))
        {
            printf("Warning: Magnet too weak\n");
        }
    }
    else
    {
        printf("No magnet detected\n");
    }

    // 配置AS5600
    AS5600_SetPowerMode(&magneticEncoder, AS5600_POWERMODE_NOMINAL);
    AS5600_SetHysteresis(&magneticEncoder, AS5600_HYST_LSB1);
    AS5600_SetSlowFilter(&magneticEncoder, AS5600_SLOW_FILT_4X);
    AS5600_SetFastFilter(&magneticEncoder, AS5600_FAST_FILT_LSB6);

    // 设置零位
    uint16_t currentAngle = AS5600_RawAngle(&magneticEncoder);
    AS5600_SetZPosition(&magneticEncoder, currentAngle);
    printf("Zero position set to: %d\n", currentAngle);

    // ========== AS5600L 初始化（如果使用） ==========
    // if (AS5600L_Init(&magneticEnco
    // derL, &hi2c1, AS5600L_DEFAULT_ADDRESS)) {
    //     printf("AS5600L initialized\n");
    //     // 可以更改I2C地址
    //     // AS5600L_SetAddress(&magneticEncoderL, 0x41);
    // }
}

// 读取角度示例
void AS5600_Example_ReadAngle(void)
{
    // 读取原始角度（0-4095）
    uint16_t rawAngle = AS5600_RawAngle(&magneticEncoder);

    // 读取角度（带偏移补偿）
    uint16_t angle = AS5600_ReadAngle(&magneticEncoder);

    // 读取角度（度数）
    float angleDegrees = AS5600_ReadAngleDegrees(&magneticEncoder);

    // 读取角度（弧度）
    float angleRadians = AS5600_ReadAngleRadians(&magneticEncoder);

    printf("Raw: %d, Angle: %d, Degrees: %.2f, Radians: %.4f\n", rawAngle, angle, angleDegrees, angleRadians);

    // 读取状态信息
    uint8_t agc = AS5600_ReadAGC(&magneticEncoder);
    uint16_t magnitude = AS5600_ReadMagnitude(&magneticEncoder);
    printf("AGC: %d, Magnitude: %d\n", agc, magnitude);
}

// 速度测量示例
void AS5600_Example_ReadSpeed(void)
{
    // 获取角速度（度/秒）
    float speedDegrees = AS5600_GetAngularSpeed(&magneticEncoder, AS5600_MODE_DEGREES, false);

    // 获取角速度（弧度/秒）
    float speedRadians = AS5600_GetAngularSpeed(&magneticEncoder, AS5600_MODE_RADIANS, true);

    // 获取转速（RPM）
    float speedRPM = AS5600_GetAngularSpeed(&magneticEncoder, AS5600_MODE_RPM, 1);

    printf("Speed: %.2f deg/s, %.4f rad/s, %.2f RPM\n", speedDegrees, speedRadians, speedRPM);
}

// 累积位置示例
void AS5600_Example_CumulativePosition(void)
{
    // 获取累积位置
    int32_t position = AS5600_GetCumulativePosition(&magneticEncoder, true);

    // 获取转数
    int32_t revolutions = AS5600_GetRevolutions(&magneticEncoder);

    printf("Cumulative position: %ld, Revolutions: %ld\n", position, revolutions);

    // 重置位置
    if (/* 需要重置 */ 0)
    {
        AS5600_ResetPosition(&magneticEncoder, 0);
        printf("Position reset\n");
    }
}
