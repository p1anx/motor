/*
 * ads1256.c
 *
 *  Created on: Dec 16, 2022
 *      Author: XWJ
 */

#include "stm32_hal.h"
#include "ads1256.h"
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

extern uint32_t tick;
void Delay_us(int time)
{
    int i;
    while (time--)
    {
        i = 10;
        while (i--)
            ;
    }
}
void ADS1256_SPI_init(void)
{
    ads1256_spi.Init.CLKPolarity = SPI_POLARITY_LOW;
    ads1256_spi.Init.CLKPhase = SPI_PHASE_2EDGE;
    HAL_SPI_Init(&ads1256_spi);
}

void ADS1256_ConfigInit(ADS1256_t *ads1256)
{
    ADS1256_SPI_init();
    uint8_t data;
    // ADS1256_RST_LOW();
    // HAL_Delay(10);
    ADS1256_RST_HIGH();
    HAL_Delay(10);
    // AD_RST(0);
    // AD_RST(1);

    // 1.
    // data = CLKOUT_OFF | DETECT_OFF | ads1256->configGain;
    // ADS1256_write(ADS1256_ADCON, &data);
    ADS1256_setGain(ads1256->configGain);
    data = ads1256->configDrate;
    ADS1256_write(ADS1256_DRATE, &data);
    data = ACALON | BufferON;
    ADS1256_write(ADS1256_STATUS, &data);
    data = ADS1256_IO_OFF;
    ADS1256_write(ADS1256_IO, &data);

    ADS1256_setChannel(ads1256->configChannel);
    HAL_Delay(10);
    ADS1256_SELFCAL();
    ADS1256_OFC(ads1256);
    ADS1256_getFSC(ads1256);
    delay_ms(1000);
    // SelectChannel(1);
    // 2.
    // data = ACALON | BufferON;
    // ADS1256_write(ADS1256_STATUS, &data);
    // ADS1256_setGain(ads1256->configGain);
    // ADS1256_setDrate(ads1256->configDrate);
    // data = ADS1256_IO_OFF;
    // ADS1256_write(ADS1256_IO, &data);
    // ADS1256_setChannel(ads1256->configChannel);

    // 3.
    // data = CLKOUT_OFF | DETECT_OFF | ADS1256_GAIN_1;
    // AD_WRITE(ADS1256_ADCON, data);
    // AD_WRITE(ADS1256_DRATE, ADS1256_DRATE_1000SPS);
    // data = ACALON | BufferON;
    // AD_WRITE(ADS1256_STATUS, data);
    // AD_WRITE(ADS1256_IO, 0x00);
    // SelectChannel(1);
}
/*
typedef enum
{
    _2_5SPS,
    _5SPS,
    _10SPS,
    _15SPS,
    _25SPS,
    _30SPS,
    _50SPS,
    _60SPS,
    _100SPS,
    _500SPS,
    _1000SPS,
    _2000SPS,
    _3750SPS,
    _7500SPS,
    _15000SPS,
    _30000SPS

} Drate_enum;
*/
// void ADS1256_setDrate(Drate_enum drate)
// {
//     uint8_t data;
//     switch (drate)
//     {
//     case _2_5SPS:
//         data = ADS1256_DRATE_2_5SPS;
//         ADS1256_write(ADS1256_DRATE, &data);
//         break;
//     case _5SPS:
//         data = ADS1256_DRATE_5SPS;
//         ADS1256_write(ADS1256_DRATE, &data);
//         break;
//     case _10SPS:
//         data = ADS1256_DRATE_10SPS;
//         ADS1256_write(ADS1256_DRATE, &data);
//         break;
//     case _15SPS:
//         data = ADS1256_DRATE_15SPS;
//         ADS1256_write(ADS1256_DRATE, &data);
//         break;
//     case _25SPS:
//         data = ADS1256_DRATE_25SPS;
//         ADS1256_write(ADS1256_DRATE, &data);
//         break;
//     case _30SPS:
//         data = ADS1256_DRATE_30SPS;
//         ADS1256_write(ADS1256_DRATE, &data);
//         break;
//     case _100SPS:
//         data = ADS1256_DRATE_100SPS;
//         ADS1256_write(ADS1256_DRATE, &data);
//         break;
//     case _500SPS:
//         data = ADS1256_DRATE_500SPS;
//         ADS1256_write(ADS1256_DRATE, &data);
//         break;
//     case _1000SPS:
//         data = ADS1256_DRATE_1000SPS;
//         ADS1256_write(ADS1256_DRATE, &data);
//         break;
//     case _2000SPS:
//         data = ADS1256_DRATE_2000SPS;
//         ADS1256_write(ADS1256_DRATE, &data);
//         break;
//     case _3750SPS:
//         data = ADS1256_DRATE_3750SPS;
//         ADS1256_write(ADS1256_DRATE, &data);
//         break;
//     case _7500SPS:
//         data = ADS1256_DRATE_7500SPS;
//         ADS1256_write(ADS1256_DRATE, &data);
//         break;
//     case _15000SPS:
//         data = ADS1256_DRATE_15000SPS;
//         ADS1256_write(ADS1256_DRATE, &data);
//         break;
//     case _30000SPS:
//         data = ADS1256_DRATE_30000SPS;
//         ADS1256_write(ADS1256_DRATE, &data);
//         break;
//     default:
//         data = ADS1256_DRATE_1000SPS;
//         ADS1256_write(ADS1256_DRATE, &data);
//         break;
//     }
// }
/*
parameter:  gain
options:    1, 2, 4, 8, 16, 32, 64
*/
void ADS1256_setGain(int gain)
{
    uint8_t data;
    switch (gain)
    {
    case 1:
        data = CLKOUT_OFF | DETECT_OFF | ADS1256_GAIN_1;
        ADS1256_write(ADS1256_ADCON, &data);
        break;
    case 2:
        data = CLKOUT_OFF | DETECT_OFF | ADS1256_GAIN_2;
        ADS1256_write(ADS1256_ADCON, &data);
        break;
    case 4:
        data = CLKOUT_OFF | DETECT_OFF | ADS1256_GAIN_4;
        ADS1256_write(ADS1256_ADCON, &data);
        break;
    case 8:
        data = CLKOUT_OFF | DETECT_OFF | ADS1256_GAIN_8;
        ADS1256_write(ADS1256_ADCON, &data);
        break;
    case 16:
        data = CLKOUT_OFF | DETECT_OFF | ADS1256_GAIN_16;
        ADS1256_write(ADS1256_ADCON, &data);
        break;
    case 32:
        data = CLKOUT_OFF | DETECT_OFF | ADS1256_GAIN_32;
        ADS1256_write(ADS1256_ADCON, &data);
        break;
    case 64:
        data = CLKOUT_OFF | DETECT_OFF | ADS1256_GAIN_64;
        ADS1256_write(ADS1256_ADCON, &data);
        break;
    default:
        data = CLKOUT_OFF | DETECT_OFF | ADS1256_GAIN_1;
        ADS1256_write(ADS1256_ADCON, &data);
        break;
    }
}



void ADS1256_SYSGCAL(void)
{
    uint8_t cmd = ADS1256_CMD_SYSGCAL;
    ADS1256_SPI_CS_LOW();
    ADS1256_SPI_transmit(&cmd, 1);
    ADS1256_SPI_CS_HIGH();
    delay_ms(1000);
}
void ADS1256_SELFCAL(void)
{
    uint8_t cmd = ADS1256_CMD_SELFCAL;
    ADS1256_SPI_CS_LOW();
    ADS1256_SPI_transmit(&cmd, 1);
    ADS1256_SPI_CS_HIGH();
    delay_ms(1000);
}
void ADS1256_getInfo(void)
{
    uint8_t status_regAddr = ADS1256_STATUS;
    uint8_t adcon_regAddr = ADS1256_ADCON;
    uint8_t mux_regAddr = ADS1256_MUX;
    uint8_t drate_regAddr = ADS1256_DRATE;

    uint8_t status_regValue, adcon_regValue, mux_regValue, drate_regValue;

    ADS1256_read(status_regAddr, &status_regValue);
    ADS1256_read(adcon_regAddr, &adcon_regValue);
    ADS1256_read(mux_regAddr, &mux_regValue);
    ADS1256_read(drate_regAddr, &drate_regValue);
    printf("\nreg info\n");
    printf("reg status\t= 0x%xh\n", status_regValue);
    printf("reg adcon\t= 0x%xh\n", adcon_regValue);
    printf("reg mux\t= 0x%xh\n", mux_regValue);
    printf("reg drate\t= 0x%xh\n", drate_regValue);
}
uint32_t ADS1256_getRawOFC(ADS1256_t *ads1256)
{
    uint8_t ofc0_regAddr = ADS1256_OFC0;
    uint8_t ofc1_regAddr = ADS1256_OFC1;
    uint8_t ofc2_regAddr = ADS1256_OFC2;
    // uint8_t adcon_regAddr = ADS1256_ADCON;
    // uint8_t mux_regAddr = ADS1256_MUX;
    // uint8_t drate_regAddr = ADS1256_DRATE;

    uint8_t ofc0_regValue, ofc1_regValue, ofc2_regValue;

    ADS1256_read(ofc0_regAddr, &ofc0_regValue);
    ADS1256_read(ofc1_regAddr, &ofc1_regValue);
    ADS1256_read(ofc2_regAddr, &ofc2_regValue);

    ads1256->rawOFC = ofc0_regValue | ofc1_regValue << 8 | ofc2_regValue << 16;
    return ads1256->rawOFC;
}
// 0.483 0.65    ----
// 0.606 0.80
// 0.997 1.257
//

// 0.25 0.23
// 1.06 0.93

uint32_t ADS1256_OFC(ADS1256_t *ads1256)
{
    uint32_t rawOFC = ADS1256_getRawOFC(ads1256);
    int result = rawOFC;
    // printf("raw ofc\t=%d\n", result);

    if (result > 0x7FFFFF)
    {
        result = 0xFFFFFF - (unsigned int)result + 1; // ����ȡֵ
        result &= 0xFFFFFF;
        result = -result;
    }
    float ofc_voltage = result * 5 / (pow(2, 23) - 1);
    printf("ofc v\t= %f\n", ofc_voltage);
}
float ADS1256_getFSC(ADS1256_t *ads1256)
{
    uint8_t fsc0_regAddr = ADS1256_FSC0;
    uint8_t fsc1_regAddr = ADS1256_FSC1;
    uint8_t fsc2_regAddr = ADS1256_FSC2;
    // uint8_t adcon_regAddr = ADS1256_ADCON;
    // uint8_t mux_regAddr = ADS1256_MUX;
    // uint8_t drate_regAddr = ADS1256_DRATE;

    uint8_t fsc0_regValue, fsc1_regValue, fsc2_regValue;

    ADS1256_read(fsc0_regAddr, &fsc0_regValue);
    ADS1256_read(fsc1_regAddr, &fsc1_regValue);
    ADS1256_read(fsc2_regAddr, &fsc2_regValue);

    ads1256->rawFSC = fsc0_regValue | fsc1_regValue << 8 | fsc2_regValue << 16;

    int result = ads1256->rawFSC;
    // printf("fsc raw\t= %d\n", result);

    if (result > 0x7FFFFF)
    {
        result = 0xFFFFFF - (unsigned int)result + 1; // ����ȡֵ
        result &= 0xFFFFFF;
        result = -result;
    }
    float ofc_voltage = result * 5 / (pow(2, 23) - 1);
    ads1256->fsc_voltage = ofc_voltage;
    printf("fsc v\t= %f\n", ads1256->fsc_voltage);
    return ads1256->fsc_voltage;
}
void ADS1256_getOFC(void)
{
    uint8_t ofc0_regAddr = ADS1256_OFC0;
    uint8_t ofc1_regAddr = ADS1256_OFC1;
    uint8_t ofc2_regAddr = ADS1256_OFC2;
    // uint8_t adcon_regAddr = ADS1256_ADCON;
    // uint8_t mux_regAddr = ADS1256_MUX;
    // uint8_t drate_regAddr = ADS1256_DRATE;

    uint8_t ofc0_regValue, ofc1_regValue, ofc2_regValue;

    ADS1256_read(ofc0_regAddr, &ofc0_regValue);
    ADS1256_read(ofc1_regAddr, &ofc1_regValue);
    ADS1256_read(ofc2_regAddr, &ofc2_regValue);
    // ADS1256_read(adcon_regAddr, &adcon_regValue);
    // ADS1256_read(mux_regAddr, &mux_regValue);
    // ADS1256_read(drate_regAddr, &drate_regValue);
    printf("\nofc info\n");
    printf("reg ofc0\t= 0x%xh\n", ofc0_regValue);
    printf("reg ofc1\t= 0x%xh\n", ofc1_regValue);
    printf("reg ofc2\t= 0x%xh\n", ofc2_regValue);
    // printf("reg drate\t= 0x%xh\n", drate_regValue);
}
uint8_t SPI_SendByte(uint8_t data)
{
    uint8_t rd;
    HAL_SPI_TransmitReceive(&ads1256_spi, &data, &rd, 1, 1000);
    return rd;
}
ADS1256_Status ADS1256_SPI_transmit(uint8_t *data, int n)
{

    HAL_SPI_Transmit(&ads1256_spi, data, n, 1000);
    // if (HAL_SPI_Transmit(&ads1256_spi, &data, 1, 1000) != HAL_OK)
    // {
    //     printf("[erro] failed to send using spi");
    //     return ADS1256_ERROR;
    // }
    return ADS1256_OK;
}

ADS1256_Status ADS1256_SPI_receive(uint8_t *data, int n)
{

    HAL_SPI_Receive(&ads1256_spi, data, n, 1000);
    // HAL_SPI_Transmit(&ads1256_spi, &data, 1, 1000);
    // if (HAL_SPI_Transmit(&ads1256_spi, &data, 1, 1000) != HAL_OK)
    // {
    //     printf("[erro] failed to send using spi");
    //     return ADS1256_ERROR;
    // }
    return ADS1256_OK;
}

ADS1256_Status ADS1256_SPI_transmitByte(uint8_t *data)
{
    ADS1256_SPI_transmit(data, 1);
    return ADS1256_OK;
}
ADS1256_Status ADS1256_SPI_receiveByte(uint8_t *data)
{
    ADS1256_SPI_receive(data, 1);
    return ADS1256_OK;
}

void ADS1256_write(uint8_t addr, uint8_t *data)
{
    uint8_t cmd = ADS1256_CMD_WREG | addr;
    uint8_t n = 0x00;
    ADS1256_SPI_CS_LOW();
    ADS1256_SPI_transmit(&cmd, 1);
    ADS1256_SPI_transmit(&n, 1);
    ADS1256_SPI_transmit(data, 1);
    ADS1256_SPI_CS_HIGH();
}
void ADS1256_read(uint8_t addr, uint8_t *data)
{
    uint8_t cmd = ADS1256_CMD_RREG | addr;
    uint8_t n = 0x00;
    ADS1256_SPI_CS_LOW();

    ADS1256_SPI_transmit(&cmd, 1);
    ADS1256_SPI_transmit(&n, 1);
    ADS1256_SPI_receive(data, 1);

    ADS1256_SPI_CS_HIGH();
}

void AD_WRITE(uint8_t addr, uint8_t data)
{
    uint8_t cmd = ADS1256_CMD_WREG | addr;
    uint8_t n = 0x00;

    SPI_CS_LOW;
    HAL_SPI_Transmit(&ads1256_spi, &cmd, 1, 1000);
    HAL_SPI_Transmit(&ads1256_spi, &n, 1, 1000);
    HAL_SPI_Transmit(&ads1256_spi, &data, 1, 1000);

    SPI_SendByte(cmd);
    //	SPI_SendByte(n);
    //	SPI_SendByte(data);

    SPI_CS_HIGH;
}

uint8_t AD_READ(uint8_t addr)
{
    uint8_t data;
    uint8_t cmd = ADS1256_CMD_RREG | addr;
    uint8_t n = 0x00;
    SPI_CS_LOW;
    HAL_SPI_Transmit(&ads1256_spi, &cmd, 1, 1000);
    HAL_SPI_Transmit(&ads1256_spi, &n, 1, 1000);
    HAL_SPI_Receive(&ads1256_spi, &data, 1, 1000);
    SPI_CS_HIGH;

    return data;
}
// a=1 is reset pin high; a = else,reset pin low
void AD_RST(int a)
{
    if (a == 1)
    {
        HAL_GPIO_WritePin(AD_RST_GPIO_Port, AD_RST_Pin, GPIO_PIN_SET); // ads1256 rst低有效
        HAL_Delay(10);
    }
    else
    {
        HAL_GPIO_WritePin(AD_RST_GPIO_Port, AD_RST_Pin, GPIO_PIN_RESET); // ads1256 rst低有效
        HAL_Delay(10);
    }
}
// ADS1256——STATUS must be set afer ACON and DRATE
void ADS1256_Init(uint8_t gain, uint8_t drate, uint8_t c)
{
    ADS1256_SPI_init();
    AD_RST(0);
    AD_RST(1);

    AD_WRITE(ADS1256_ADCON, CLKOUT_OFF | DETECT_OFF | gain);
    AD_WRITE(ADS1256_DRATE, drate);
    AD_WRITE(ADS1256_STATUS, ACALON | BufferON);

    AD_WRITE(ADS1256_IO, 0x00);
    SelectChannel(c);
    printf("[ok] init\n");
}

void ADS1256_setChannel(uint8_t channel)
{
    switch (channel)
    {
    case 1:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN0 | ADS1256_MUXN_AINCOM);
        break;
    case 2:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN1 | ADS1256_MUXN_AINCOM);
        break;
    case 3:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN2 | ADS1256_MUXN_AINCOM);
        break;
    case 4:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN3 | ADS1256_MUXN_AINCOM);
        break;
    case 5:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN4 | ADS1256_MUXN_AINCOM);
        break;
    case 6:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN5 | ADS1256_MUXN_AINCOM);
        break;
    case 7:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN6 | ADS1256_MUXN_AINCOM);
        break;
    case 8:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN7 | ADS1256_MUXN_AINCOM);
        break;
    case 12:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN0 | ADS1256_MUXN_AIN1);
        break;
    case 13:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN0 | ADS1256_MUXN_AIN2);
        break;
    default:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN0 | ADS1256_MUXN_AINCOM);
        break;
    }
}

uint32_t ADS1256_ReadRawData(void)
{
    SelectChannel(1); // 不影响采集
    uint32_t rd;
    uint8_t cmd[3] = {ADS1256_CMD_SYNC, ADS1256_CMD_WAKEUP, ADS1256_CMD_RDATA};
    uint8_t data[3];
    ADS1256_SPI_CS_LOW();
    while (ADS1256_GET_DRDY())
    {
    }

    while (ADS1256_GET_DRDY())
    {
    }
    ADS1256_SPI_transmit(cmd, sizeof(cmd) / sizeof(cmd[0]));

    HAL_Delay(1);
    ADS1256_SPI_receive(data, 3);
    ADS1256_SPI_CS_HIGH();

    // 2.
    //  SPI_CS_LOW;
    //  while (DRDY)
    //      ;
    //  HAL_SPI_Transmit(&hspi2, cmd, sizeof(cmd) / sizeof(cmd[0]), 1000);
    //  HAL_Delay(1);
    //  HAL_SPI_Receive(&hspi2, data, 3, 1000);
    //  SPI_CS_HIGH;

    rd = data[0] << 16 | data[1] << 8 | data[2];
    return rd;
}

uint32_t ADS1256_GetRawData(int channel)
{
    SelectChannel(channel); // 不影响采集
    uint32_t rd;
    uint8_t cmd[3] = {ADS1256_CMD_SYNC, ADS1256_CMD_WAKEUP, ADS1256_CMD_RDATA};
    uint8_t data[3];
    ADS1256_SPI_CS_LOW();
    while (ADS1256_GET_DRDY())
    {
    }

    while (ADS1256_GET_DRDY())
    {
    }
    ADS1256_SPI_transmit(cmd, sizeof(cmd) / sizeof(cmd[0]));

    HAL_Delay(1);
    ADS1256_SPI_receive(data, 3);
    ADS1256_SPI_CS_HIGH();

    // 2.
    //  SPI_CS_LOW;
    //  while (DRDY)
    //      ;
    //  HAL_SPI_Transmit(&hspi2, cmd, sizeof(cmd) / sizeof(cmd[0]), 1000);
    //  HAL_Delay(1);
    //  HAL_SPI_Receive(&hspi2, data, 3, 1000);
    //  SPI_CS_HIGH;

    rd = data[0] << 16 | data[1] << 8 | data[2];
    return rd;
}
float ADS1256_getVoltage(ADS1256_t *ads1256)
{

    int result;
    float voltage;
    result = ADS1256_ReadRawData();
    // result = ADS1256_GetRawData(2);
    if (result > 0x7FFFFF)
    {
        result = 0xFFFFFF - (unsigned int)result + 1; // ����ȡֵ
        result &= 0xFFFFFF;
        result = -result;
    }
    voltage = (float)result * 5 / (pow(2, 23) - 1);
    ads1256->voltage = voltage - ads1256->ofc_voltage;
    // printf("voltage = %lf v\n", ads1256->voltage);
    return ads1256->voltage;
}
float ADS1256_getVoltageFromChannel(ADS1256_t *ads1256, int channel)
{

    int result;
    float voltage;
    // result = ADS1256_ReadRawData();
    result = ADS1256_GetRawData(channel);
    if (result > 0x7FFFFF)
    {
        result = 0xFFFFFF - (unsigned int)result + 1; // ����ȡֵ
        result &= 0xFFFFFF;
        result = -result;
    }
    voltage = (float)result * 5 / (pow(2, 23) - 1);
    ads1256->voltage = voltage - ads1256->ofc_voltage;
    // printf("voltage = %lf v\n", ads1256->voltage);
    return ads1256->voltage;
}

float ADS1256_get2Voltage(ADS1256_t *ads1256)
{
    float voltage_ch1 = ADS1256_getVoltageFromChannel(ads1256, 1);
    float voltage_ch2 = ADS1256_getVoltageFromChannel(ads1256, 2);
    printf("%f, %f\n", voltage_ch1, voltage_ch2);

    return ads1256->voltage;
}
float ADS1256_getVoltage_with_cal(ADS1256_t *ads1256)
{
    int pga = 1;
    int vin = ADS1256_ReadRawData();
    float alpha = 0x3c0000;
    float beta = 1.7474;
    uint32_t output = (pga * vin / 5 - ads1256->rawOFC / alpha) * ads1256->rawFSC * beta;
    printf("output = %d\n", output);
    printf("ofc\t=%d\n", ads1256->rawOFC);
    printf("fsc\t=%d\n", ads1256->rawOFC);
}
void ADS1256_printVoltage(ADS1256_t *ads1256)
{
    float v = ADS1256_getVoltage(ads1256);
    printf("%f\n", v);
}

void AD_SELFCAL(void)
{
    SPI_CS_LOW;

    // uint8_t cmd[] = {SELFOCAL};
    // HAL_SPI_Transmit(&hspi2, cmd, sizeof(cmd)/sizeof(cmd[0]), 1000);
    // HAL_Delay(1000);
    // uint8_t cmd1[] = {SELFGCAL};
    // HAL_SPI_Transmit(&hspi2, cmd1, sizeof(cmd1)/sizeof(cmd1[0]), 1000);
    uint8_t cmd2[] = {SELFCAL};
    HAL_SPI_Transmit(&ads1256_spi, cmd2, sizeof(cmd2) / sizeof(cmd2[0]), 1000);

    SPI_CS_HIGH;
    HAL_Delay(1000);
}

void AD_CAL_RegRead(void)
{
    uint8_t ofc[3];
    uint8_t fsc[3];

    uint32_t fscc, ofcc;
    double voltage_fscc, voltage_ofcc;
    uint32_t alph = 0x3c0000;
    float beta = 1.74;
    ofc[0] = AD_READ(0X05);
    ofc[1] = AD_READ(0X06);
    ofc[2] = AD_READ(0X07);
    //  printf("ofc0:%x\n", ofc[0]);
    //  printf("ofc1:%x\n", ofc[1]);
    //  printf("ofc2:%x\n", ofc[2]);
    ofcc = ofc[0] | ofc[1] << 8 | ofc[2] << 16;
    voltage_ofcc = ofcc * 5 / (pow(2, 23) - 1);
    printf("ofc:%x\n", ofcc);
    printf("ofc_v:%lf V\n", voltage_ofcc);

    fsc[0] = AD_READ(0X08);
    fsc[1] = AD_READ(0X09);
    fsc[2] = AD_READ(0X0a);
    //  printf("fsc0:%x\n", fsc[0]);
    //  printf("fsc1:%x\n", fsc[1]);
    //  printf("fsc2:%x\n", fsc[2]);
    fscc = fsc[0] | fsc[1] << 8 | fsc[2] << 16;
    voltage_fscc = fscc * 5 / (pow(2, 23) - 1);
    printf("fsc:%x\n", fscc);
    printf("fsc_v:%lf V\n", voltage_fscc);
}

void SelectChannel(uint8_t c)
{
    switch (c)
    {
    case 1:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN0 | ADS1256_MUXN_AINCOM);
        break;
    case 2:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN1 | ADS1256_MUXN_AINCOM);
        break;
    case 3:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN2 | ADS1256_MUXN_AINCOM);
        break;
    case 4:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN3 | ADS1256_MUXN_AINCOM);
        break;
    case 5:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN4 | ADS1256_MUXN_AINCOM);
        break;
    case 6:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN5 | ADS1256_MUXN_AINCOM);
        break;
    case 7:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN6 | ADS1256_MUXN_AINCOM);
        break;
    case 8:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN7 | ADS1256_MUXN_AINCOM);
        break;
    case 12:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN0 | ADS1256_MUXN_AIN1);
        break;
    case 13:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN0 | ADS1256_MUXN_AIN2);
        break;
    default:
        AD_WRITE(ADS1256_MUX, ADS1256_MUXP_AIN0 | ADS1256_MUXN_AINCOM);
        break;
    }
}

uint32_t AD_ReadData(void)
{
    // SelectChannel(1); // 不影响采集
    uint8_t cmd[3] = {ADS1256_CMD_SYNC, ADS1256_CMD_WAKEUP, ADS1256_CMD_RDATA};
    uint8_t data[3];
    uint32_t rd;
    SPI_CS_LOW;
    while (DRDY)
        ;

    HAL_SPI_Transmit(&ads1256_spi, cmd, sizeof(cmd) / sizeof(cmd[0]), 1000);
    HAL_Delay(1);
    HAL_SPI_Receive(&ads1256_spi, data, 3, 1000);

    SPI_CS_HIGH;

    rd = data[0] << 16 | data[1] << 8 | data[2];
    return rd;
}

void Data_Print(void)
{

    int result;
    double voltage;
    // if(tick == 1){
    // 	result = AD_ReadData();
    // 	tick = 0;
    // }
    // 1.
    result = AD_ReadData();
    // 2.
    // result = ADS1256_ReadRawData();

    if (result > 0x7FFFFF)
    {
        result = 0xFFFFFF - (unsigned int)result + 1; // ����ȡֵ
        result &= 0xFFFFFF;
        result = -result;
    }
    voltage = result * 5 / (pow(2, 23) - 1);
    printf("voltage = %lf v\n", voltage);
    // printf("voltage2 = %lf v", voltage);
    // fflush(stdout);
    // printf("\n\r");
}

void Matlab_Print(void)
{
    int result;
    uint8_t flag = '*';
    uint8_t rdata[3];
    result = AD_ReadData();
    //	if(result > 0x7FFFFF){
    //		result = 0xFFFFFF - (unsigned int)result + 1; //����ȡֵ
    //		result &= 0xFFFFFF;
    ////		result = -result;
    //	}

    rdata[0] = (result & 0xff0000) >> 16;
    rdata[1] = (result & 0xff00) >> 8;
    rdata[2] = (result & 0xff);
    HAL_UART_Transmit(&HUART, &flag, 1, 1000);
    HAL_UART_Transmit(&HUART, rdata, 3, 1000);
    HAL_Delay(5);
    //	HAL_UART_Transmit(&huart1, &result, 3, 1000);
}

void Transmit_To_Python(void)
{
    int result;
    uint8_t flag = 0x19;
    uint8_t rdata[3];
    result = AD_ReadData();

    rdata[0] = (result & 0xff0000) >> 16;
    rdata[1] = (result & 0xff00) >> 8;
    rdata[2] = (result & 0xff);
    HAL_UART_Transmit(&HUART, &flag, 1, 1000);
    HAL_UART_Transmit(&HUART, rdata, 3, 1000);
    HAL_Delay(5);
}

void example_ads1256_main(void)
{
    ADS1256_t ads1256;

    // config for ads1256
    ads1256.configChannel = 1;
    ads1256.configGain = ADS1256_GAIN_1;
    ads1256.configDrate = ADS1256_DRATE_1000SPS;

    // 1.
    //  ADS1256_Init(ADS1256_GAIN_1, ADS1256_DRATE_1000SPS, 1);
    // 2.
    ADS1256_ConfigInit(&ads1256);
    HAL_Delay(1000);
    ADS1256_getInfo();
    // int last_t = 0;
    int counter = 0;
    int last_t =  HAL_GetTick();
    while (1)
    {
        // ADS1256_getVoltage(&ads1256);
        int now_t = HAL_GetTick();
        if (now_t - last_t < 1000)
        {
            ADS1256_printVoltage(&ads1256);
            counter++;
        }
        else
        {
            printf("last %d, now %d\n", last_t, now_t);
            printf("counter = %d\n", counter);
            counter=0;
            //1.
            HAL_Delay(1000);
            last_t = now_t+1000;
            //2.
            // last_t = now_t;

        }
        // HAL_Delay(1000);
        // printf("v = %f\n", ads1256.)
    }
}
