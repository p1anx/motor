#ifndef __ADS1256_H
#define __ADS1256_H

#include "stm32_hal.h"
#include "gpio.h"
#include "spi.h"
#include "usart.h"
#include <stdint.h>

// extern SPI_HandleTypeDef hspi2;

#define HUART huart1

#define ads1256_spi hspi1

#define AD_CS_GPIO_Port GPIOA
#define AD_CS_Pin GPIO_PIN_4
#define AD_DRDY_GPIO_Port GPIOC
#define AD_DRDY_Pin GPIO_PIN_3
#define AD_RST_GPIO_Port GPIOC
#define AD_RST_Pin GPIO_PIN_2

#define SPI_CS_LOW HAL_GPIO_WritePin(AD_CS_GPIO_Port, AD_CS_Pin, GPIO_PIN_RESET)
#define SPI_CS_HIGH HAL_GPIO_WritePin(AD_CS_GPIO_Port, AD_CS_Pin, GPIO_PIN_SET)
#define DRDY HAL_GPIO_ReadPin(AD_DRDY_GPIO_Port, AD_DRDY_Pin)

#define ADS1256_SPI_CS_LOW() HAL_GPIO_WritePin(AD_CS_GPIO_Port, AD_CS_Pin, GPIO_PIN_RESET)
#define ADS1256_SPI_CS_HIGH() HAL_GPIO_WritePin(AD_CS_GPIO_Port, AD_CS_Pin, GPIO_PIN_SET)
#define ADS1256_GET_DRDY() HAL_GPIO_ReadPin(AD_DRDY_GPIO_Port, AD_DRDY_Pin)

#define ADS1256_RST_LOW() HAL_GPIO_WritePin(AD_RST_GPIO_Port, AD_RST_Pin, GPIO_PIN_RESET); // ads1256 rst低有效
#define ADS1256_RST_HIGH() HAL_GPIO_WritePin(AD_RST_GPIO_Port, AD_RST_Pin, GPIO_PIN_SET);  // ads1256 rst低有效

typedef struct ADS1256_t ADS1256_t;
typedef enum ADS1256_Status ADS1256_Status;
typedef enum Drate_enum Drate_enum;
enum Drate_enum
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
};

enum ADS1256_Status
{
    ADS1256_OK = 0x00U,
    ADS1256_ERROR = 0x01U,
};

struct ADS1256_t
{
    int rawValue; // 24bit
    uint32_t rawOFC;
    uint32_t rawFSC;
    float ofc_voltage;
    float fsc_voltage;
    float voltage;
    // Drate_enum configDrate;
    uint8_t configDrate;
    uint8_t configGain;
    uint8_t Drate;
    uint8_t configChannel;

    uint8_t regGain;
    uint8_t regDrate;
    uint8_t regChannel;
};

#define SELFCAL 0xF0
#define SELFOCAL 0xF1
#define SELFGCAL 0xF2
#define SYSOCAL 0xF3
#define SYSGCAL 0xF4

#define ADS1256_CMD_WAKEUP 0x00
#define ADS1256_CMD_RDATA 0x01
#define ADS1256_CMD_RDATAC 0x03
#define ADS1256_CMD_SDATAC 0x0f
#define ADS1256_CMD_RREG 0x10
#define ADS1256_CMD_WREG 0x50
#define ADS1256_CMD_SELFCAL 0xf0
#define ADS1256_CMD_SELFOCAL 0xf1
#define ADS1256_CMD_SELFGCAL 0xf2
#define ADS1256_CMD_SYSOCAL 0xf3
#define ADS1256_CMD_SYSGCAL 0xf4
#define ADS1256_CMD_SYNC 0xfc
#define ADS1256_CMD_STANDBY 0xfd
#define ADS1256_CMD_REST 0xfe

// define the ADS1256 register values
#define ADS1256_STATUS 0x00
#define ADS1256_MUX 0x01
#define ADS1256_ADCON 0x02
#define ADS1256_DRATE 0x03
#define ADS1256_IO 0x04
#define ADS1256_OFC0 0x05
#define ADS1256_OFC1 0x06
#define ADS1256_OFC2 0x07
#define ADS1256_FSC0 0x08
#define ADS1256_FSC1 0x09
#define ADS1256_FSC2 0x0A

#define ADS1256_IO_OFF 0x00

#define CLKOUT_OFF (0x00 << 5)
#define CLKOUT_CLKIN (0x01 << 5)
#define DETECT_OFF (0x00 << 3)
#define DETECT_ON_2UA (0x02 << 3)

// define multiplexer codes
#define ADS1256_MUXP_AIN0 0x00
#define ADS1256_MUXP_AIN1 0x10
#define ADS1256_MUXP_AIN2 0x20
#define ADS1256_MUXP_AIN3 0x30
#define ADS1256_MUXP_AIN4 0x40
#define ADS1256_MUXP_AIN5 0x50
#define ADS1256_MUXP_AIN6 0x60
#define ADS1256_MUXP_AIN7 0x70
#define ADS1256_MUXP_AINCOM 0x80

#define ADS1256_MUXN_AIN0 0x00
#define ADS1256_MUXN_AIN1 0x01
#define ADS1256_MUXN_AIN2 0x02
#define ADS1256_MUXN_AIN3 0x03
#define ADS1256_MUXN_AIN4 0x04
#define ADS1256_MUXN_AIN5 0x05
#define ADS1256_MUXN_AIN6 0x06
#define ADS1256_MUXN_AIN7 0x07
#define ADS1256_MUXN_AINCOM 0x08

// define gain codes
#define ADS1256_GAIN_1 0x00
#define ADS1256_GAIN_2 0x01
#define ADS1256_GAIN_4 0x02
#define ADS1256_GAIN_8 0x03
#define ADS1256_GAIN_16 0x04
#define ADS1256_GAIN_32 0x05
#define ADS1256_GAIN_64 0x06
// #define ADS1256_GAIN_64     0x07

// define drate codes
#define ADS1256_DRATE_30000SPS 0xF0
#define ADS1256_DRATE_15000SPS 0xE0
#define ADS1256_DRATE_7500SPS 0xD0
#define ADS1256_DRATE_3750SPS 0xC0
#define ADS1256_DRATE_2000SPS 0xB0
#define ADS1256_DRATE_1000SPS 0xA1
#define ADS1256_DRATE_500SPS 0x92
#define ADS1256_DRATE_100SPS 0x82
#define ADS1256_DRATE_60SPS 0x72
#define ADS1256_DRATE_50SPS 0x63
#define ADS1256_DRATE_30SPS 0x53
#define ADS1256_DRATE_25SPS 0x43
#define ADS1256_DRATE_15SPS 0x33
#define ADS1256_DRATE_10SPS 0x23
#define ADS1256_DRATE_5SPS 0x13
#define ADS1256_DRATE_2_5SPS 0x03

#define BufferON 0x02
#define BufferOFF 0x00

#define ACALON 0x04
#define ACALOFF 0x00

void AD_WRITE(uint8_t addr, uint8_t data);
void ADS1256_Init(uint8_t gain, uint8_t drate, uint8_t c);
void AD_RST(int a);
uint32_t AD_ReadData(void);

void SelectChannel(uint8_t c);
void Data_Print(void);
void Matlab_Print(void);
void Transmit_To_Python(void);

void AD_SYSCAL(void);
void AD_SELFCAL(void);
void AD_CAL_RegRead(void);
uint8_t AD_READ(uint8_t addr);

ADS1256_Status ADS1256_SPI_receiveByte(uint8_t *data);
ADS1256_Status ADS1256_SPI_transmitByte(uint8_t *data);
ADS1256_Status ADS1256_SPI_receive(uint8_t *data, int n);
ADS1256_Status ADS1256_SPI_transmit(uint8_t *data, int n);

void ADS1256_ConfigInit(ADS1256_t *ads1256);
void ADS1256_read(uint8_t addr, uint8_t *data);
void ADS1256_write(uint8_t addr, uint8_t *data);
void ADS1256_setChannel(uint8_t channel);
void ADS1256_setGain(int gain);
void ADS1256_setDrate(Drate_enum drate);
void ADS1256_getInfo(void);
void ADS1256_SPI_init(void);
uint32_t ADS1256_ReadRawData(void);
float ADS1256_getVoltage(ADS1256_t *ads1256);
void ADS1256_printVoltage(ADS1256_t *ads1256);
void ADS1256_getOFC(void);
uint32_t ADS1256_OFC(ADS1256_t *ads1256);
float ADS1256_getFSC(ADS1256_t *ads1256);
void ADS1256_SYSGCAL(void);
void ADS1256_SELFCAL(void);
float ADS1256_getVoltage_with_cal(ADS1256_t *ads1256);
void example_ads1256_main(void);
uint32_t ADS1256_GetRawData(int channel);
float ADS1256_getVoltageFromChannel(ADS1256_t *ads1256, int channel);
float ADS1256_get2Voltage(ADS1256_t *ads1256);
#endif /* _ADS1256_H_ */
