#include "encoder.h"
#include "encoder_as5600.h"
#include "i2c.h"
// #include "stm32f4xx_hal.h"
// extern I2C_HandleTypeDef hi2c1;

Encoder_t encoder;

void Encoder_init(Encoder_t *encoder)
{

    // Encoder_AS5600_Init(encoder->as5600, encoder->as5600->hi2c);
    encoder->as5600_Instance.hi2c = &as5600_i2c;

    Encoder_AS5600_Init(&encoder->as5600_Instance, encoder->as5600_Instance.hi2c);
    HAL_Delay(10);
        // 【修复2】检查磁铁状态
    if (!AS5600_DetectMagnet(&encoder->as5600_Instance)) {
        printf("[ERROR] No magnet detected! Check magnet position!\n");
        encoder->initial_angle_offset = 0;
        return;
    }

    // 【修复3】多次读取取平均值，提高稳定性
    float angle_sum = 0;
    int valid_reads = 0;

    for (int i = 0; i < 5; i++) {
        encoder->as5600_Instance.error = AS5600_OK;
        float angle = AS5600_ReadAngleRadians(&encoder->as5600_Instance);

        if (encoder->as5600_Instance.error == AS5600_OK) {
            angle_sum += angle;
            valid_reads++;
            printf("[Encoder Init] Read %d: %.4f rad\n", i+1, angle);
        } else {
            printf("[WARN] Read %d failed, error: %d\n", i+1, encoder->as5600_Instance.error);
        }

        HAL_Delay(2);  // 每次读取间隔2ms
    }

    if (valid_reads == 0) {
        printf("[ERROR] All AS5600 reads failed! Using 0 as offset.\n");
        encoder->initial_angle_offset = 0;
    } else {
        encoder->initial_angle_offset = angle_sum / valid_reads;
        printf("[Encoder Init] ✅ Initial offset: %.4f rad (avg of %d reads, %.1f deg)\n",
               encoder->initial_angle_offset, valid_reads,
               encoder->initial_angle_offset * 180.0f / 3.14159265f);
    }

    // 【关键】初始化AS5600的lastReadAngle和lastAngle
    encoder->as5600_Instance.lastReadAngle = AS5600_ReadAngle(&encoder->as5600_Instance);
    encoder->as5600_Instance.lastAngle = encoder->as5600_Instance.lastReadAngle;
    encoder->as5600_Instance.lastMeasurement = 0;

    // 【修复4】打印诊断信息
    uint8_t agc = AS5600_ReadAGC(&encoder->as5600_Instance);
    uint16_t magnitude = AS5600_ReadMagnitude(&encoder->as5600_Instance);

    printf("[Encoder Init] Raw: %d, AGC: %d, Magnitude: %d\n",
           encoder->as5600_Instance.lastReadAngle, agc, magnitude);

    if (AS5600_MagnetTooStrong(&encoder->as5600_Instance)) {
        printf("[WARN] ⚠️  Magnet too strong! Move further away.\n");
    }
    if (AS5600_MagnetTooWeak(&encoder->as5600_Instance)) {
        printf("[WARN] ⚠️  Magnet too weak! Move closer.\n");
    }

    printf("[Encoder Init] ✅ Initialization complete!\n");

}
float Encoder_getAngle(Encoder_t *enc)
{
    // 1.
    // enc->angle = AS5600_ReadAngleRadians(enc->as5600);
    // // return _2PI * (enc->pulse_counter) / ((float)enc->cpr);
    // return enc->angle;
    // 2.
    enc->angle = AS5600_ReadAngleRadians(&enc->as5600_Instance);
    return enc->angle;
}

/*
  Shaft velocity calculation
  function using mixed time and frequency measurement technique
*/
float Encoder_getVelocity(Encoder_t *enc)
{

    // 1.
    // float speedRPM = AS5600_GetAngularSpeed(enc->as5600, AS5600_MODE_RPM, 1);
    // 2.
    float speedRPM = AS5600_GetAngularSpeed(&enc->as5600_Instance, AS5600_MODE_RPM, 1);
    return speedRPM;
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
