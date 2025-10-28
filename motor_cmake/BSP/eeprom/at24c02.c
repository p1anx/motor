//
// Created by xwj on 10/15/25.
//

#include "at24c02.h"
#include "string.h"

/**
 * @brief  检测AT24C02设备是否存在
 * @param  None
 * @retval 0: 设备存在, 1: 设备不存在
 */
uint8_t AT24C02_CheckDevice(void)
{
    uint8_t temp = 0;

    if (HAL_I2C_IsDeviceReady(&hi2c1, AT24C02_ADDR, 2, AT24C02_TIMEOUT) == HAL_OK)
    {
        return 0; // 设备存在
    }
    else
    {
        return 1; // 设备不存在
    }
}

/**
 * @brief  写入一个字节到AT24C02
 * @param  WriteAddr: 写入地址 (0-255)
 * @param  Data: 要写入的数据
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_WriteByte(uint16_t WriteAddr, uint8_t Data)
{
    uint8_t addr[2];

    if (WriteAddr >= AT24C02_TOTAL_SIZE)
        return 1; // 地址超出范围

    addr[0] = (uint8_t)(WriteAddr & 0xFF); // 地址低8位

    if (HAL_I2C_Mem_Write(&hi2c1, AT24C02_ADDR, addr[0], I2C_MEMADD_SIZE_8BIT,
                         &Data, 1, AT24C02_TIMEOUT) == HAL_OK)
    {
        HAL_Delay(5); // 等待写入完成
        return 0; // 写入成功
    }
    else
    {
        return 1; // 写入失败
    }
}

/**
 * @brief  从AT24C02读取一个字节
 * @param  ReadAddr: 读取地址 (0-255)
 * @param  Data: 存放读取数据的指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_ReadByte(uint16_t ReadAddr, uint8_t *Data)
{
    uint8_t addr;

    if (ReadAddr >= AT24C02_TOTAL_SIZE)
        return 1; // 地址超出范围

    addr = (uint8_t)(ReadAddr & 0xFF); // 地址低8位

    if (HAL_I2C_Mem_Read(&hi2c1, AT24C02_ADDR, addr, I2C_MEMADD_SIZE_8BIT,
                        Data, 1, AT24C02_TIMEOUT) == HAL_OK)
    {
        return 0; // 读取成功
    }
    else
    {
        return 1; // 读取失败
    }
}

/**
 * @brief  写入一页数据到AT24C02 (注意页面边界)
 * @param  WriteAddr: 写入起始地址
 * @param  pBuffer: 数据缓冲区
 * @param  NumByteToWrite: 要写入的字节数 (最大8字节，且不能跨页)
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_WritePage(uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumByteToWrite)
{
    uint8_t addr[2];

    if (WriteAddr >= AT24C02_TOTAL_SIZE || pBuffer == NULL)
        return 1;

    // 检查是否跨页 (AT24C02页面大小为8字节)
    if ((WriteAddr / AT24C02_PAGE_SIZE) != ((WriteAddr + NumByteToWrite - 1) / AT24C02_PAGE_SIZE))
    {
        return 1; // 跨页错误
    }

    if (NumByteToWrite > AT24C02_PAGE_SIZE)
        return 1; // 超出页面大小

    addr[0] = (uint8_t)(WriteAddr & 0xFF);

    if (HAL_I2C_Mem_Write(&hi2c1, AT24C02_ADDR, addr[0], I2C_MEMADD_SIZE_8BIT,
                         pBuffer, NumByteToWrite, AT24C02_TIMEOUT) == HAL_OK)
    {
        HAL_Delay(5); // 等待写入完成
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 * @brief  写入多个字节到AT24C02 (自动处理跨页问题)
 * @param  WriteAddr: 写入起始地址
 * @param  pBuffer: 数据缓冲区
 * @param  NumByteToWrite: 要写入的字节数
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_WriteNByte(uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumByteToWrite)
{
    uint16_t addr = WriteAddr;
    uint16_t NumByte = NumByteToWrite;
    uint8_t *pBuffer_temp = pBuffer;
    uint8_t NumToWrite;
    uint8_t Num;

    if (WriteAddr >= AT24C02_TOTAL_SIZE || pBuffer == NULL || NumByteToWrite == 0)
        return 1;

    while (NumByte)
    {
        // 计算当前页剩余字节数
        NumToWrite = AT24C02_PAGE_SIZE - (addr % AT24C02_PAGE_SIZE);
        if (NumToWrite > NumByte)
        {
            NumToWrite = NumByte;
        }

        // 写入当前页数据
        if (AT24C02_WritePage(addr, pBuffer_temp, NumToWrite) != 0)
        {
            return 1;
        }

        // 更新参数
        addr += NumToWrite;
        pBuffer_temp += NumToWrite;
        NumByte -= NumToWrite;
    }

    return 0;
}

/**
 * @brief  从AT24C02读取多个字节
 * @param  ReadAddr: 读取起始地址
 * @param  pBuffer: 数据缓冲区
 * @param  NumByteToRead: 要读取的字节数
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_ReadNByte(uint16_t ReadAddr, uint8_t *pBuffer, uint16_t NumByteToRead)
{
    uint8_t addr;

    if (ReadAddr >= AT24C02_TOTAL_SIZE || pBuffer == NULL || NumByteToRead == 0)
        return 1;

    addr = (uint8_t)(ReadAddr & 0xFF);

    if (HAL_I2C_Mem_Read(&hi2c1, AT24C02_ADDR, addr, I2C_MEMADD_SIZE_8BIT,
                        pBuffer, NumByteToRead, AT24C02_TIMEOUT) == HAL_OK)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 * @brief  AT24C02写测试函数
 */
void AT24C02_WriteTest(void)
{
    uint8_t test_data[] = "Hello AT24C02!";
    uint8_t result;

    result = AT24C02_WriteNByte(0, test_data, sizeof(test_data));
    if (result == 0)
    {
        // 写入成功
    }
    else
    {
        // 写入失败
    }
}

/**
 * @brief  AT24C02读测试函数
 */
void AT24C02_ReadTest(void)
{
    uint8_t read_buffer[20];
    uint8_t result;

    result = AT24C02_ReadNByte(0, read_buffer, sizeof(read_buffer));
    if (result == 0)
    {
        // 读取成功，数据在read_buffer中
    }
    else
    {
        // 读取失败
    }
}
// 在at24c02.c文件中添加以下函数实现：

/**
 * @brief  写入32位数据到AT24C02
 * @param  WriteAddr: 写入起始地址 (需要确保地址+3不超过255)
 * @param  Data: 要写入的32位数据
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_Write32Bit(uint16_t WriteAddr, uint32_t Data)
{
    uint8_t data_bytes[4];

    if (WriteAddr > (AT24C02_TOTAL_SIZE - 4))  // 确保不会超出存储范围
        return 1;

    // 将32位数据分解为4个字节 (大端模式，高字节在前)
    data_bytes[0] = (uint8_t)((Data >> 24) & 0xFF);  // 最高字节
    data_bytes[1] = (uint8_t)((Data >> 16) & 0xFF);
    data_bytes[2] = (uint8_t)((Data >> 8) & 0xFF);
    data_bytes[3] = (uint8_t)(Data & 0xFF);          // 最低字节

    return AT24C02_WriteNByte(WriteAddr, data_bytes, 4);
}

/**
 * @brief  从AT24C02读取32位数据
 * @param  ReadAddr: 读取起始地址
 * @param  pData: 存放读取数据的指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_Read32Bit(uint16_t ReadAddr, uint32_t *pData)
{
    uint8_t data_bytes[4];
    uint8_t result;

    if (ReadAddr > (AT24C02_TOTAL_SIZE - 4))  // 确保不会超出存储范围
        return 1;

    if (pData == NULL)
        return 1;

    result = AT24C02_ReadNByte(ReadAddr, data_bytes, 4);
    if (result != 0)
        return result;

    // 将4个字节重新组合为32位数据 (大端模式)
    *pData = ((uint32_t)data_bytes[0] << 24) |
             ((uint32_t)data_bytes[1] << 16) |
             ((uint32_t)data_bytes[2] << 8)  |
             ((uint32_t)data_bytes[3]);

    return 0;
}

/**
 * @brief  写入16位数据到AT24C02
 * @param  WriteAddr: 写入起始地址
 * @param  Data: 要写入的16位数据
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_Write16Bit(uint16_t WriteAddr, uint16_t Data)
{
    uint8_t data_bytes[2];

    if (WriteAddr > (AT24C02_TOTAL_SIZE - 2))  // 确保不会超出存储范围
        return 1;

    // 将16位数据分解为2个字节 (大端模式)
    data_bytes[0] = (uint8_t)((Data >> 8) & 0xFF);  // 高字节
    data_bytes[1] = (uint8_t)(Data & 0xFF);         // 低字节

    return AT24C02_WriteNByte(WriteAddr, data_bytes, 2);
}

/**
 * @brief  从AT24C02读取16位数据
 * @param  ReadAddr: 读取起始地址
 * @param  pData: 存放读取数据的指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_Read16Bit(uint16_t ReadAddr, uint16_t *pData)
{
    uint8_t data_bytes[2];
    uint8_t result;

    if (ReadAddr > (AT24C02_TOTAL_SIZE - 2))  // 确保不会超出存储范围
        return 1;

    if (pData == NULL)
        return 1;

    result = AT24C02_ReadNByte(ReadAddr, data_bytes, 2);
    if (result != 0)
        return result;

    // 将2个字节重新组合为16位数据 (大端模式)
    *pData = ((uint16_t)data_bytes[0] << 8) | ((uint16_t)data_bytes[1]);

    return 0;
}
// =======================float========================
// 在at24c02.c文件中添加以下函数实现：

/**
 * @brief  写入浮点数到AT24C02
 * @param  WriteAddr: 写入起始地址 (需要确保地址+3不超过255)
 * @param  Data: 要写入的float数据
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_WriteFloat(uint16_t WriteAddr, float Data)
{
    FloatUnion float_union;
    uint8_t data_bytes[4];

    if (WriteAddr > (AT24C02_TOTAL_SIZE - 4))  // 确保不会超出存储范围
        return 1;

    // 将float数据转换为联合体
    float_union.f = Data;

    // 将联合体的字节复制到数组中 (大端模式)
    data_bytes[0] = float_union.b[3];  // 最高字节 (IEEE 754标准下)
    data_bytes[1] = float_union.b[2];
    data_bytes[2] = float_union.b[1];
    data_bytes[3] = float_union.b[0];  // 最低字节

    return AT24C02_WriteNByte(WriteAddr, data_bytes, 4);
}

/**
 * @brief  从AT24C02读取浮点数
 * @param  ReadAddr: 读取起始地址
 * @param  pData: 存放读取数据的指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_ReadFloat(uint16_t ReadAddr, float *pData)
{
    FloatUnion float_union;
    uint8_t data_bytes[4];
    uint8_t result;

    if (ReadAddr > (AT24C02_TOTAL_SIZE - 4))  // 确保不会超出存储范围
        return 1;

    if (pData == NULL)
        return 1;

    result = AT24C02_ReadNByte(ReadAddr, data_bytes, 4);
    if (result != 0)
        return result;

    // 将字节数组转换为联合体 (大端模式)
    float_union.b[3] = data_bytes[0];  // 最高字节
    float_union.b[2] = data_bytes[1];
    float_union.b[1] = data_bytes[2];
    float_union.b[0] = data_bytes[3];  // 最低字节

    *pData = float_union.f;

    return 0;
}

/**
 * @brief  写入双精度浮点数到AT24C02
 * @param  WriteAddr: 写入起始地址 (需要确保地址+7不超过255)
 * @param  Data: 要写取的double数据
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_WriteDouble(uint16_t WriteAddr, double Data)
{
    DoubleUnion double_union;
    uint8_t data_bytes[8];
    uint8_t i;

    if (WriteAddr > (AT24C02_TOTAL_SIZE - 8))  // 确保不会超出存储范围
        return 1;

    // 将double数据转换为联合体
    double_union.d = Data;

    // 将联合体的字节复制到数组中 (大端模式)
    for (i = 0; i < 8; i++)
    {
        data_bytes[i] = double_union.b[7 - i];  // 高字节在前
    }

    return AT24C02_WriteNByte(WriteAddr, data_bytes, 8);
}

/**
 * @brief  从AT24C02读取双精度浮点数
 * @param  ReadAddr: 读取起始地址
 * @param  pData: 存放读取数据的指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t AT24C02_ReadDouble(uint16_t ReadAddr, double *pData)
{
    DoubleUnion double_union;
    uint8_t data_bytes[8];
    uint8_t result;
    uint8_t i;

    if (ReadAddr > (AT24C02_TOTAL_SIZE - 8))  // 确保不会超出存储范围
        return 1;

    if (pData == NULL)
        return 1;

    result = AT24C02_ReadNByte(ReadAddr, data_bytes, 8);
    if (result != 0)
        return result;

    // 将字节数组转换为联合体 (大端模式)
    for (i = 0; i < 8; i++)
    {
        double_union.b[7 - i] = data_bytes[i];  // 高字节在前
    }

    *pData = double_union.d;

    return 0;
}