//
// Created by xwj on 10/15/25.
//

#ifndef __AT24C02_H
#define __AT24C02_H

#include "main.h"
#include <stdint.h>

/* AT24C02配置 */
#define AT24C02_ADDR            0xA0    // AT24C02的I2C地址 (根据A0,A1,A2引脚接法可能不同)
#define AT24C02_PAGE_SIZE       8       // AT24C02页面大小为8字节
#define AT24C02_TOTAL_SIZE      256     // AT24C02总容量为256字节
#define AT24C02_TIMEOUT         100     // I2C超时时间

/* I2C句柄 (需要在main.c中定义) */
extern I2C_HandleTypeDef hi2c1; // 根据你的实际I2C句柄修改

/* 函数声明 */
uint8_t AT24C02_CheckDevice(void);
uint8_t AT24C02_WriteByte(uint16_t WriteAddr, uint8_t Data);
uint8_t AT24C02_ReadByte(uint16_t ReadAddr, uint8_t *Data);
uint8_t AT24C02_WritePage(uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumByteToWrite);
uint8_t AT24C02_WriteNByte(uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumByteToWrite);
uint8_t AT24C02_ReadNByte(uint16_t ReadAddr, uint8_t *pBuffer, uint16_t NumByteToRead);
void AT24C02_WriteTest(void);
void AT24C02_ReadTest(void);
// 在at24c02.h文件中添加以下函数声明：

/**
 * @brief  写入32位数据到AT24C02
 * @param  WriteAddr: 写入起始地址 (需要确保地址+3不超过255)
 * @param  Data: 要写入的32位数据
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_Write32Bit(uint16_t WriteAddr, uint32_t Data);

/**
 * @brief  从AT24C02读取32位数据
 * @param  ReadAddr: 读取起始地址
 * @param  pData: 存放读取数据的指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_Read32Bit(uint16_t ReadAddr, uint32_t *pData);

/**
 * @brief  写入16位数据到AT24C02
 * @param  WriteAddr: 写入起始地址
 * @param  Data: 要写入的16位数据
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_Write16Bit(uint16_t WriteAddr, uint16_t Data);

/**
 * @brief  从AT24C02读取16位数据
 * @param  ReadAddr: 读取起始地址
 * @param  pData: 存放读取数据的指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_Read16Bit(uint16_t ReadAddr, uint16_t *pData);
// 在at24c02.h文件中添加以下函数声明：

/**
 * @brief  写入浮点数到AT24C02
 * @param  WriteAddr: 写入起始地址 (需要确保地址+3不超过255)
 * @param  Data: 要写入的float数据
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_WriteFloat(uint16_t WriteAddr, float Data);

/**
 * @brief  从AT24C02读取浮点数
 * @param  ReadAddr: 读取起始地址
 * @param  pData: 存放读取数据的指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_ReadFloat(uint16_t ReadAddr, float *pData);

/**
 * @brief  写入双精度浮点数到AT24C02
 * @param  WriteAddr: 写入起始地址 (需要确保地址+7不超过255)
 * @param  Data: 要写入的double数据
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_WriteDouble(uint16_t WriteAddr, double Data);

/**
 * @brief  从AT24C02读取双精度浮点数
 * @param  ReadAddr: 读取起始地址
 * @param  pData: 存放读取数据的指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_ReadDouble(uint16_t ReadAddr, double *pData);

/**
 * @brief  联合体用于float和uint32_t之间的转换
 */
typedef union {
    float f;
    uint32_t i;
    uint8_t b[4];
} FloatUnion;

/**
 * @brief  联合体用于double和uint64_t之间的转换
 */
typedef union {
    double d;
    uint64_t i;
    uint8_t b[8];
} DoubleUnion;

#endif //MOTOR_CMAKE_AT24C02_H