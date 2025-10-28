/*
 * bsp_spi.h
 *
 *  Created on: Oct 29, 2023
 *      Author: XWJ
 */

#ifndef LIS3MDL_H
#define LIS3MDL_H
// #include "stm32f1xx.h"
#include <stdio.h>
#include "spi.h"
#include "usart.h"
// #include "mymain.h"
// #include "ESP8266.h"

#define HUART huart1

#define HSPI hspi2

#define SPI_CS_PIN GPIO_PIN_3
#define SPI_CS_PORT GPIOA

#define W25X_PageProgram 0x02
#define W25X_WriteEnable 0x06
#define W25X_WriteDisable 0x04

#define SPI_CS_LOW HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_RESET)
#define SPI_CS_HIGH HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_SET)

// #define SPI_CS_LOW 		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
// #define SPI_CS_HIGH 	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

#define LIS3MDL_WRITE 0 << 7
#define LIS3MDL_READ 1 << 7

#define WHO_I_AM 0x0F // READ DATA (default: 0x3d)

#define ADDR_CTRL_REG1 0X20
#define ADDR_CTRL_REG2 0X21
#define ADDR_CTRL_REG3 0X22
#define ADDR_CTRL_REG4 0X23
#define ADDR_CTRL_REG5 0X24

//*****REG1配置******
// x和y轴的工作模式
#define CTRL_REG1_OM_LP 0X00 << 5 // 低耗能
#define CTRL_REG1_OM_MP 0X01 << 5
#define CTRL_REG1_OM_HP 0X02 << 5
#define CTRL_REG1_OM_UHP 0X03 << 5

// 输出数据速率
#define CTRL_REG1_DO_80HZ 0X07 << 2
#define CTRL_REG1_DO_40HZ 0X06 << 2
#define CTRL_REG1_DO_20HZ 0X05 << 2

#define TEMP_EN_DISABLE 0 << 7
#define FAST_ODR_ENABLE 1 << 1
#define FAST_ODR_DISABLE 0 << 1
#define ST 1

#define CTRL_REG1 CTRL_REG1_OM_LP | CTRL_REG1_DO_80HZ | 0 << 7
//*******************

//*****REG2配置******
#define FS_4GAUSS 0b00 << 5
#define FS_8GAUSS 0b01 << 5
#define FS_12GAUSS 0b10 << 5
#define FS_16GAUSS 0b11 << 5

#define REBOOT_NORMAL 0 << 3
#define SOFT_RST_0 0 << 2
#define SOFT_RST_1 1 << 2

#define CTRL_REG2 FS_16GAUSS | REBOOT_NORMAL | SOFT_RST_0 | 0 << 7
//*******************

//*****REG3配置******
#define LP 0 << 5
#define SIM 0 << 2
// #define MD  0 << 1 | 0
#define MD_CONTINUOUS 0b00
#define MD_SINGLE 0B01
#define MD_PWR_DOWN_1 0B11
#define MD_PWR_DOWN_0 0B10

#define CTRL_REG3 LP | SIM | MD_CONTINUOUS | 0 << 7
//*******************

//*****REG4配置******
#define OMZ_LP 0B00 << 2
#define OMZ_MP 0B01 << 2
#define OMZ_HP 0B10 << 2
#define OMZ_UHP 0B11 << 2

#define BLE 0 << 1

#define CTRL_REG4 OMZ_LP | BLE | 0 << 7

//*******************

//*****REG5配置******
#define FAST_READ_DISABLE 0 << 7
#define FAST_READ_ENABLE 1 << 7
#define BDU 0 << 6

#define CTRL_REG5 FAST_READ | BDU | 0 << 7
//*******************

//*****configuration of read data******
#define ADDR_OUT_X_L 0X28 // output data of x axis
#define ADDR_OUT_X_H 0X29

#define ADDR_OUT_Y_L 0X2A
#define ADDR_OUT_Y_H 0X2B

#define ADDR_OUT_Z_L 0X2C
#define ADDR_OUT_Z_H 0X2D

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t z;
} LIS3MDL_DATA;

typedef struct
{
    float data_x;
    float data_y;
    float data_z;
} LIS3MDL_t;

#define DATA_SIZE 1
#define B_DATA_SIZE 300

void LIS3MDL_WriteReg(uint8_t reg, uint8_t value);
uint8_t LIS3MDL_ReadReg(uint8_t reg);
void LIS3MDL_Init(void);
void LIS3MDL_DataRead(void);
LIS3MDL_DATA LIS3MDL_DataRead_Continue(void);
void LIS3MDL_DataRead_Continue_Transmission(void);
void LIS3MDL_DataRead_Transmisson(void);
int Sensitivity(void);
void LIS3MDL_TransmitToPython(void);
void LIS3MDL_DataRead_Continuous(void);
void LIS3MDL_DataRead_Continuous_Print(void);
void LIS3MDL_DataRead_DMA(void);
// int LIS3MDL_getMagneticGauss(LIS3MDL_t *lis3mdl);

LIS3MDL_t LIS3MDL_getMagneticGauss(void);
void LIS3MDL_demo(void);
void LIS3MDL_InitStruct(LIS3MDL_t *lis3_mdl);
void LIS3MDL_DataUpdate(LIS3MDL_t *lis3mdl);
void example_lis3mdl_0(void);
#endif /* BSP_BSP_SPI_H_ */