#include "encoder.h"

#include "as5600.h"
#include "encoder_as5600.h"
#include "i2c.h"

#include <stdio.h>
// #include "stm32f4xx_hal.h"
// extern I2C_HandleTypeDef hi2c1;

Encoder_t encoder;

EncoderStatus_t Encoder_init0(Encoder_t *encoder)
{
    switch (encoder->EncoderType)
    {
    case EncoderType_AS5600:
        printf("[Encoder Init] AS5600 init!\n");
        break;
    case EncoderType_MT6835:
        printf("[Encoder Init] MT6835 init!\n");
        MT6835_Init(&encoder->mt6835, &mt6835_spi, MT6835_SPI_CS_PORT, MT6835_SPI_CS_PIN);
        break;
    default:
        printf("please choose a encoderType to init!\n");
        return Encoder_ERROR;
    }
    return Encoder_OK;
}
int Encoder_init(Encoder_t *encoder)
{

    // Encoder_AS5600_Init(encoder->as5600, encoder->as5600->hi2c);
    encoder->as5600_Instance.hi2c = &as5600_i2c;

    if (Encoder_AS5600_Init(&encoder->as5600_Instance, encoder->as5600_Instance.hi2c) != AS5600_OK)
    {
        printf("[ERROR] Encoder Init failed!\n");
        return -1;
    }
    HAL_Delay(10);
    // 【修复2】检查磁铁状态
    if (!AS5600_DetectMagnet(&encoder->as5600_Instance))
    {
        printf("[ERROR] No magnet detected! Check magnet position!\n");
        encoder->initial_angle_offset = 0;
        return -1;
    }

    // 【修复3】多次读取取平均值，提高稳定性
    float angle_sum = 0;
    int valid_reads = 0;

    for (int i = 0; i < 5; i++)
    {
        encoder->as5600_Instance.error = AS5600_OK;
        float angle = AS5600_ReadAngleRadians(&encoder->as5600_Instance);

        if (encoder->as5600_Instance.error == AS5600_OK)
        {
            angle_sum += angle;
            valid_reads++;
            printf("[Encoder Init] Read %d: %.4f rad\n", i + 1, angle);
        }
        else
        {
            printf("[WARN] Read %d failed, error: %d\n", i + 1, encoder->as5600_Instance.error);
        }

        HAL_Delay(2); // 每次读取间隔2ms
    }

    if (valid_reads == 0)
    {
        printf("[ERROR] All AS5600 reads failed! Using 0 as offset.\n");
        encoder->initial_angle_offset = 0;
    }
    else
    {
        encoder->initial_angle_offset = angle_sum / valid_reads;
        printf("[Encoder Init] ✅ Initial offset: %.4f rad (avg of %d reads, %.1f deg)\n", encoder->initial_angle_offset, valid_reads, encoder->initial_angle_offset * 180.0f / 3.14159265f);
    }

    // 【关键】初始化AS5600的lastReadAngle和lastAngle
    encoder->as5600_Instance.lastReadAngle = AS5600_ReadAngle(&encoder->as5600_Instance);
    encoder->as5600_Instance.lastAngle = encoder->as5600_Instance.lastReadAngle;
    encoder->as5600_Instance.lastMeasurement = 0;

    // 【修复4】打印诊断信息
    uint8_t agc = AS5600_ReadAGC(&encoder->as5600_Instance);
    uint16_t magnitude = AS5600_ReadMagnitude(&encoder->as5600_Instance);

    printf("[Encoder Init] Raw: %d, AGC: %d, Magnitude: %d\n", encoder->as5600_Instance.lastReadAngle, agc, magnitude);

    if (AS5600_MagnetTooStrong(&encoder->as5600_Instance))
    {
        printf("[WARN] ⚠️  Magnet too strong! Move further away.\n");
    }
    if (AS5600_MagnetTooWeak(&encoder->as5600_Instance))
    {
        printf("[WARN] ⚠️  Magnet too weak! Move closer.\n");
    }

    printf("[Encoder Init] ✅ Initialization complete!\n");

    return 0;
}

float Encoder_getAngle_v0(Encoder_t *encoder)
{
    encoder->angle = AS5600_getAngle(&encoder->as5600_Instance);
    return encoder->angle;
}
float Encoder_getAngle(Encoder_t *encoder)
{
    switch (encoder->EncoderType)
    {
    case EncoderType_AS5600:
        encoder->angle = AS5600_getAngle(&encoder->AS5600);
        return encoder->angle;
    case EncoderType_MT6835:
        encoder->angle = MT6835_GetAngleRadians(&encoder->mt6835);
        return encoder->angle;
    default:
        printf("please choose a valid encoder type!\n");
        break;
    }
}
float Encoder_getAngleDegrees(Encoder_t *encoder)
{
    switch (encoder->EncoderType)
    {
    case EncoderType_AS5600:
        encoder->angle = AS5600_getAngleDegrees(&encoder->as5600_Instance);
        return encoder->angle;
    case EncoderType_MT6835:
        encoder->angle = MT6835_GetAngleDegrees(&encoder->mt6835);
        return encoder->angle;
    default:
        printf("please choose a valid encoder type!\n");
        break;
    }
}

/*
  Shaft velocity calculation
  function using mixed time and frequency measurement technique
*/
float Encoder_getVelocity_v0(Encoder_t *enc)
{

    // 1.
    // float speedRPM = AS5600_GetAngularSpeed(enc->as5600, AS5600_MODE_RPM, 1);
    // 2.
    float speedRPM = AS5600_GetAngularSpeed(&enc->as5600_Instance, AS5600_MODE_RPM, 1);
    return speedRPM;
}
float Encoder_getVelocity(Encoder_t *encoder)
{
    switch (encoder->EncoderType)
    {
        float velocity;
    case EncoderType_AS5600:
        velocity = AS5600_getVelocity(&encoder->AS5600);
        return velocity;
    case EncoderType_MT6835:
        velocity = MT6835_GetVelocityHz(&encoder->mt6835);
        return velocity;
    default:
        printf("please choose a valid encoder type!\n");
        break;
    }
    return 0;
}

float Encoder_getVelocityRPM(Encoder_t *enc)
{

    // 1.
    // float speedRPM = AS5600_GetAngularSpeed(enc->as5600, AS5600_MODE_RPM, 1);
    // 2.
    float speedRPM = AS5600_GetAngularSpeed(&enc->as5600_Instance, AS5600_MODE_RPM, 1);
    return speedRPM;
}
// void Encoder_init(Encoder_t *encoder)
// {
//     Encoder_AS5600_Init(encoder->as5600, encoder->as5600->hi2c);
// }

void Encoder_linkAS5600(Encoder_t *encoder, AS5600_t *as5600)
{
    encoder->as5600 = as5600;
}
