/*
 * bsp_spi.c
 *
 *  Created on: Oct 29, 2023
 *      Author: XWJ
 */

#include "lis3mdl.h"
// #include "dma.h"

// Write the data to a register
LIS3MDL_t lis3mdl;
void LIS3MDL_WriteReg(uint8_t reg, uint8_t value)
{
    SPI_CS_LOW;
    uint8_t data[2];
    data[0] = reg | LIS3MDL_WRITE;
    data[1] = value;
    HAL_SPI_Transmit(&HSPI, data, 2, HAL_MAX_DELAY);
    SPI_CS_HIGH;
}

// Read the data of a register
uint8_t LIS3MDL_ReadReg(uint8_t reg)
{
    SPI_CS_LOW;
    uint8_t data[2] = {0x00, 0x00};
    uint8_t value;
    data[0] = reg | LIS3MDL_READ;
    HAL_SPI_TransmitReceive(&HSPI, data, data, 2, HAL_MAX_DELAY);
    value = data[1];
    SPI_CS_HIGH;
    return value;
}

uint8_t FullScale = FS_4GAUSS;
// uint8_t FullScale;

void LIS3MDL_InitStruct(LIS3MDL_t *lis3_mdl)
{
    uint8_t data_reg1, data_reg2, data_reg3, data_reg4, data_reg5;
    data_reg1 = CTRL_REG1_OM_LP | CTRL_REG1_DO_80HZ | FAST_ODR_ENABLE | ST;
    data_reg2 = FullScale | REBOOT_NORMAL | SOFT_RST_0; // Full scale selection
    data_reg3 = LP | SIM | MD_CONTINUOUS;               // Read mode selection, single or continuous
    data_reg4 = OMZ_LP | BLE;
    data_reg5 = FAST_READ_DISABLE | BDU;

    // data_reg1 = 0x10|0x07;
    // data_reg2 = 0x00;
    // data_reg3 = 0x00;
    // data_reg4 = 0x00;
    // data_reg5 = 0x00;

    LIS3MDL_WriteReg(ADDR_CTRL_REG1, data_reg1);
    LIS3MDL_WriteReg(ADDR_CTRL_REG2, data_reg2);
    LIS3MDL_WriteReg(ADDR_CTRL_REG3, data_reg3);
    LIS3MDL_WriteReg(ADDR_CTRL_REG4, data_reg4);
    LIS3MDL_WriteReg(ADDR_CTRL_REG5, data_reg5);

    lis3_mdl->data_x = 0;
    lis3_mdl->data_y = 0;
    lis3_mdl->data_z = 0;
}
void LIS3MDL_Init(void)
{
    uint8_t data_reg1, data_reg2, data_reg3, data_reg4, data_reg5;
    data_reg1 = CTRL_REG1_OM_LP | CTRL_REG1_DO_80HZ | FAST_ODR_ENABLE | ST;
    data_reg2 = FullScale | REBOOT_NORMAL | SOFT_RST_0; // Full scale selection
    data_reg3 = LP | SIM | MD_CONTINUOUS;               // Read mode selection, single or continuous
    data_reg4 = OMZ_LP | BLE;
    data_reg5 = FAST_READ_DISABLE | BDU;

    // data_reg1 = 0x10|0x07;
    // data_reg2 = 0x00;
    // data_reg3 = 0x00;
    // data_reg4 = 0x00;
    // data_reg5 = 0x00;

    LIS3MDL_WriteReg(ADDR_CTRL_REG1, data_reg1);
    LIS3MDL_WriteReg(ADDR_CTRL_REG2, data_reg2);
    LIS3MDL_WriteReg(ADDR_CTRL_REG3, data_reg3);
    LIS3MDL_WriteReg(ADDR_CTRL_REG4, data_reg4);
    LIS3MDL_WriteReg(ADDR_CTRL_REG5, data_reg5);
}
// select
int Sensitivity(void)
{
    int sensitivity;
    switch (FullScale)
    {
    case FS_16GAUSS:
        sensitivity = 1711;
        break;
    case FS_12GAUSS:
        sensitivity = 2281;
        break;
    case FS_8GAUSS:
        sensitivity = 3421;
        break;
    case FS_4GAUSS:
        sensitivity = 6842;
        break;
    default:
        printf("sensitivity error!\n");
    }
    return sensitivity;
}
LIS3MDL_t LIS3MDL_getMagneticGauss(void)
{
    int datax, datay, dataz; // for showing negative number, the received data must be int type
    LIS3MDL_DATA data;
    LIS3MDL_t lis3mdl;
    int s = Sensitivity();
    // printf("s=%d\n",s);
    float Bx, By, Bz;
    datax = LIS3MDL_ReadReg(ADDR_OUT_X_H) << 8 | LIS3MDL_ReadReg(ADDR_OUT_X_L);
    datay = LIS3MDL_ReadReg(ADDR_OUT_Y_H) << 8 | LIS3MDL_ReadReg(ADDR_OUT_Y_L);
    dataz = LIS3MDL_ReadReg(ADDR_OUT_Z_H) << 8 | LIS3MDL_ReadReg(ADDR_OUT_Z_L);

    // printf("x:%d\n", datax);
    // printf("y:%d\n", datay);
    // printf("z:%d\n", dataz);
    if (datax > 0x7fff)
    {
        datax = ~(uint16_t)datax + 1;
        datax &= 0xffff;
        datax = -datax;
    }
    if (datay > 0x7fff)
    {
        datay = ~(uint16_t)datay + 1;
        datay &= 0xffff;
        datay = -datay;
    }
    if (dataz > 32767)
    {
        dataz = ~(uint16_t)dataz + 1; // complement code conversion
        dataz &= 0xffff;
        dataz = -dataz;
    }
    lis3mdl.data_x = (float)datax / s;
    lis3mdl.data_y = (float)datay / s;
    lis3mdl.data_z = (float)dataz / s;
    return lis3mdl;
    // printf("%f, %f, %f\n", Bx, By, Bz);
    // printf("Bx=%f gauss\n", Bx);
    // printf("By=%f gauss\n", By);
    // printf("Bz=%f gauss\n", Bz);
}
void LIS3MDL_DataUpdate(LIS3MDL_t *lis3mdl)
{
    int datax, datay, dataz; // for showing negative number, the received data must be int type
    LIS3MDL_DATA data;
    int s = Sensitivity();
    // printf("s=%d\n",s);
    float Bx, By, Bz;
    datax = LIS3MDL_ReadReg(ADDR_OUT_X_H) << 8 | LIS3MDL_ReadReg(ADDR_OUT_X_L);
    datay = LIS3MDL_ReadReg(ADDR_OUT_Y_H) << 8 | LIS3MDL_ReadReg(ADDR_OUT_Y_L);
    dataz = LIS3MDL_ReadReg(ADDR_OUT_Z_H) << 8 | LIS3MDL_ReadReg(ADDR_OUT_Z_L);

    // printf("x:%d\n", datax);
    // printf("y:%d\n", datay);
    // printf("z:%d\n", dataz);
    if (datax > 0x7fff)
    {
        datax = ~(uint16_t)datax + 1;
        datax &= 0xffff;
        datax = -datax;
    }
    if (datay > 0x7fff)
    {
        datay = ~(uint16_t)datay + 1;
        datay &= 0xffff;
        datay = -datay;
    }
    if (dataz > 32767)
    {
        dataz = ~(uint16_t)dataz + 1; // complement code conversion
        dataz &= 0xffff;
        dataz = -dataz;
    }
    Bx = (float)datax / s;
    By = (float)datay / s;
    Bz = (float)dataz / s;
    lis3mdl->data_x = Bx;
    lis3mdl->data_y = By;
    lis3mdl->data_z = Bz;
    // printf("%f, %f, %f\n", Bx, By, Bz);
    // printf("Bx=%f gauss\n", Bx);
    // printf("By=%f gauss\n", By);
    // printf("Bz=%f gauss\n", Bz);
}

void LIS3MDL_DataRead(void)
{
    int datax, datay, dataz; // for showing negative number, the received data must be int type
    LIS3MDL_DATA data;
    int s = Sensitivity();
    // printf("s=%d\n",s);
    float Bx, By, Bz;
    datax = LIS3MDL_ReadReg(ADDR_OUT_X_H) << 8 | LIS3MDL_ReadReg(ADDR_OUT_X_L);
    datay = LIS3MDL_ReadReg(ADDR_OUT_Y_H) << 8 | LIS3MDL_ReadReg(ADDR_OUT_Y_L);
    dataz = LIS3MDL_ReadReg(ADDR_OUT_Z_H) << 8 | LIS3MDL_ReadReg(ADDR_OUT_Z_L);

    // printf("x:%d\n", datax);
    // printf("y:%d\n", datay);
    // printf("z:%d\n", dataz);
    if (datax > 0x7fff)
    {
        datax = ~(uint16_t)datax + 1;
        datax &= 0xffff;
        datax = -datax;
    }
    if (datay > 0x7fff)
    {
        datay = ~(uint16_t)datay + 1;
        datay &= 0xffff;
        datay = -datay;
    }
    if (dataz > 32767)
    {
        dataz = ~(uint16_t)dataz + 1; // complement code conversion
        dataz &= 0xffff;
        dataz = -dataz;
    }
    Bx = (float)datax / s;
    By = (float)datay / s;
    Bz = (float)dataz / s;
    printf("%f, %f, %f\n", Bx, By, Bz);
    // printf("Bx=%f gauss\n", Bx);
    // printf("By=%f gauss\n", By);
    // printf("Bz=%f gauss\n", Bz);
}

void LIS3MDL_DataRead_Transmisson(void)
{
    LIS3MDL_DATA data;
    uint8_t Bx_L[DATA_SIZE], Bx_H[DATA_SIZE], By_L[DATA_SIZE], By_H[DATA_SIZE], Bz_L[DATA_SIZE],
        Bz_H[DATA_SIZE];
    // uint16_t s = Sensitivity();
    uint8_t Bx_start = 0x10;
    uint8_t Bx_end = 0x11;
    uint8_t By_start = 0x20;
    uint8_t By_end = 0x21;
    uint8_t Bz_start = 0x30;
    uint8_t Bz_end = 0x31;
    // printf("s=%d\n",s);

    for (int i = 0; i < DATA_SIZE; i++)
    {
        Bx_L[i] = LIS3MDL_ReadReg(ADDR_OUT_X_L);
        Bx_H[i] = LIS3MDL_ReadReg(ADDR_OUT_X_H);
        By_L[i] = LIS3MDL_ReadReg(ADDR_OUT_Y_L);
        By_H[i] = LIS3MDL_ReadReg(ADDR_OUT_Y_H);
        Bz_L[i] = LIS3MDL_ReadReg(ADDR_OUT_Z_L);
        Bz_H[i] = LIS3MDL_ReadReg(ADDR_OUT_Z_H);
        HAL_UART_Transmit(&HUART, &Bx_start, 1, HAL_MAX_DELAY);
        HAL_UART_Transmit(&HUART, (uint8_t *)Bx_H, DATA_SIZE, HAL_MAX_DELAY);
        HAL_UART_Transmit(&HUART, (uint8_t *)Bx_L, DATA_SIZE, HAL_MAX_DELAY);
        HAL_UART_Transmit(&HUART, &Bx_end, 1, HAL_MAX_DELAY);
        // HAL_Delay(10);

        // HAL_UART_Transmit(&HUART, &By_start, 1, HAL_MAX_DELAY);
        // HAL_UART_Transmit(&HUART, (uint8_t *)By_H, DATA_SIZE, HAL_MAX_DELAY);
        // HAL_UART_Transmit(&HUART, (uint8_t *)By_L, DATA_SIZE, HAL_MAX_DELAY);
        // HAL_UART_Transmit(&HUART, &By_end, 1, HAL_MAX_DELAY);

        // HAL_UART_Transmit(&HUART, &Bz_start, 1, HAL_MAX_DELAY);
        // HAL_UART_Transmit(&HUART, (uint8_t *)Bz_H, DATA_SIZE, HAL_MAX_DELAY);
        // HAL_UART_Transmit(&HUART, (uint8_t *)Bz_L, DATA_SIZE, HAL_MAX_DELAY);
        // HAL_UART_Transmit(&HUART, &Bz_end, 1, HAL_MAX_DELAY);
    }
    // HAL_UART_Transmit(&HUART, &start, 1, HAL_MAX_DELAY);
    // HAL_UART_Transmit(&HUART, Bx_H, 1, HAL_MAX_DELAY);
    // HAL_UART_Transmit(&HUART, Bx_L, DATA_SIZE, HAL_MAX_DELAY);
}
void LIS3MDL_TransmitToPython(void)
{
    LIS3MDL_DATA data;
    uint8_t Bx_L[DATA_SIZE], Bx_H[DATA_SIZE], By_L[DATA_SIZE], By_H[DATA_SIZE], Bz_L[DATA_SIZE],
        Bz_H[DATA_SIZE];
    uint16_t s = Sensitivity();
    uint8_t s_arr[2] = {(s & 0xff00) >> 8, s & 0xff};
    uint8_t s_start = 0x00;
    uint8_t s_end = 0x01;
    uint8_t Bx_start = 0x10;
    uint8_t Bx_end = 0x11;
    uint8_t By_start = 0x20;
    uint8_t By_end = 0x21;
    uint8_t Bz_start = 0x30;
    uint8_t Bz_end = 0x31;

    for (int i = 0; i < DATA_SIZE; i++)
    {
        Bx_L[i] = LIS3MDL_ReadReg(ADDR_OUT_X_L);
        Bx_H[i] = LIS3MDL_ReadReg(ADDR_OUT_X_H);
        By_L[i] = LIS3MDL_ReadReg(ADDR_OUT_Y_L);
        By_H[i] = LIS3MDL_ReadReg(ADDR_OUT_Y_H);
        Bz_L[i] = LIS3MDL_ReadReg(ADDR_OUT_Z_L);
        Bz_H[i] = LIS3MDL_ReadReg(ADDR_OUT_Z_H);
        HAL_UART_Transmit(&HUART, (uint8_t *)&s_start, 1, HAL_MAX_DELAY);
        HAL_UART_Transmit(&HUART, (uint8_t *)s_arr, 2, HAL_MAX_DELAY);

        HAL_UART_Transmit(&HUART, (uint8_t *)Bx_H, DATA_SIZE, HAL_MAX_DELAY);
        HAL_UART_Transmit(&HUART, (uint8_t *)Bx_L, DATA_SIZE, HAL_MAX_DELAY);

        HAL_UART_Transmit(&HUART, (uint8_t *)By_H, DATA_SIZE, HAL_MAX_DELAY);
        HAL_UART_Transmit(&HUART, (uint8_t *)By_L, DATA_SIZE, HAL_MAX_DELAY);

        HAL_UART_Transmit(&HUART, (uint8_t *)Bz_H, DATA_SIZE, HAL_MAX_DELAY);
        HAL_UART_Transmit(&HUART, (uint8_t *)Bz_L, DATA_SIZE, HAL_MAX_DELAY);
    }
}

uint8_t Bx_L[DATA_SIZE], Bx_H[DATA_SIZE], By_L[DATA_SIZE], By_H[DATA_SIZE], Bz_L[DATA_SIZE],
    Bz_H[DATA_SIZE];
void LIS3MDL_DataRead_Continuous(void)
{
    LIS3MDL_DATA data;
    uint8_t txFlag_x = 0x19;
    uint8_t txFlag_y = 0x18;
    uint8_t txFlag_z = 0x17;
    uint8_t a = 0x00;
    uint8_t b = 0x11;

    for (int i = 0; i < DATA_SIZE; i++)
    {
        Bx_L[i] = LIS3MDL_ReadReg(ADDR_OUT_X_L);
        Bx_H[i] = LIS3MDL_ReadReg(ADDR_OUT_X_H);
        By_L[i] = LIS3MDL_ReadReg(ADDR_OUT_Y_L);
        By_H[i] = LIS3MDL_ReadReg(ADDR_OUT_Y_H);
        Bz_L[i] = LIS3MDL_ReadReg(ADDR_OUT_Z_L);
        Bz_H[i] = LIS3MDL_ReadReg(ADDR_OUT_Z_H);
        // printf("Bx_H[%d]:%d\n", i, Bx_H[i]);
        // printf("Bx_L[%d]:%d\n", i, Bx_L[i]);
    }
    // HAL_UART_Transmit_DMA(&HUART, &txFlag_x, 1);
    // HAL_UART_Transmit_DMA(&HUART, (uint8_t *)Bx_H, DATA_SIZE);
    // HAL_UART_Transmit_DMA(&HUART, (uint8_t *)Bx_L, DATA_SIZE);

    // HAL_UART_Transmit_DMA(&HUART, &txFlag_y, 1);
    // HAL_UART_Transmit_DMA(&HUART, (uint8_t *)By_H, DATA_SIZE);
    // HAL_UART_Transmit_DMA(&HUART, (uint8_t *)By_L, DATA_SIZE);

    // HAL_UART_Transmit_DMA(&HUART, &txFlag_z, 1);
    // HAL_UART_Transmit_DMA(&HUART, (uint8_t *)Bz_H, DATA_SIZE);
    // HAL_UART_Transmit_DMA(&HUART, (uint8_t *)Bz_L, DATA_SIZE);

    HAL_UART_Transmit(&HUART, &txFlag_x, 1, HAL_MAX_DELAY);
    HAL_UART_Transmit(&HUART, (uint8_t *)Bx_H, DATA_SIZE, HAL_MAX_DELAY);
    HAL_UART_Transmit(&HUART, (uint8_t *)Bx_L, DATA_SIZE, HAL_MAX_DELAY);

    HAL_UART_Transmit(&HUART, &txFlag_y, 1, HAL_MAX_DELAY);
    HAL_UART_Transmit(&HUART, (uint8_t *)By_H, DATA_SIZE, HAL_MAX_DELAY);
    HAL_UART_Transmit(&HUART, (uint8_t *)By_L, DATA_SIZE, HAL_MAX_DELAY);

    HAL_UART_Transmit(&HUART, &txFlag_z, 1, HAL_MAX_DELAY);
    HAL_UART_Transmit(&HUART, (uint8_t *)Bz_H, DATA_SIZE, HAL_MAX_DELAY);
    HAL_UART_Transmit(&HUART, (uint8_t *)Bz_L, DATA_SIZE, HAL_MAX_DELAY);
}

void LIS3MDL_DataRead_Continuous_Print(void)
{
    int datax, datay, dataz; // for showing negative number, the received data must be int type
    LIS3MDL_DATA data;
    int s = Sensitivity();
    printf("s=%d\n", s);
    float Bx, By, Bz;
    for (int i = 0; i < DATA_SIZE; i++)
    {
        datax = Bx_H[i] << 8 | Bx_L[i];
        datay = By_H[i] << 8 | By_L[i];
        dataz = Bz_H[i] << 8 | Bz_L[i];
        printf("x:%d\n", datax);
        printf("y:%d\n", datay);
        printf("z:%d\n", dataz);
        if (datax > 0x7fff)
        {
            datax = ~(uint16_t)datax + 1;
            datax &= 0xffff;
            datax = -datax;
        }
        if (datay > 0x7fff)
        {
            datay = ~(uint16_t)datay + 1;
            datay &= 0xffff;
            datay = -datay;
        }
        if (dataz > 32767)
        {
            dataz = ~(uint16_t)dataz + 1; // complement code conversion
            dataz &= 0xffff;
            dataz = -dataz;
        }
        Bx = (float)datax / s;
        By = (float)datay / s;
        Bz = (float)dataz / s;
        printf("Bx=%f gauss\n", Bx);
        printf("By=%f gauss\n", By);
        printf("Bz=%f gauss\n", Bz);
    }
}

extern uint8_t RxUART;

uint8_t B1[B_DATA_SIZE * 6], B2[B_DATA_SIZE * 6], B3[B_DATA_SIZE * 6];
void LIS3MDL_DataRead_DMA(void)
{
    LIS3MDL_DATA data;
    uint8_t txFlag_x = 0x19;
    uint8_t txFlag_y = 0x18;
    uint8_t txFlag_z = 0x17;
    uint8_t a = 0x00;
    uint8_t b = 0x11;

    for (int i = 0; i < B_DATA_SIZE; i++)
    {
        B1[i * 6 + 0] = LIS3MDL_ReadReg(ADDR_OUT_X_H);
        B1[i * 6 + 1] = LIS3MDL_ReadReg(ADDR_OUT_X_L);
        B1[i * 6 + 2] = LIS3MDL_ReadReg(ADDR_OUT_Y_H);
        B1[i * 6 + 3] = LIS3MDL_ReadReg(ADDR_OUT_Y_L);
        B1[i * 6 + 4] = LIS3MDL_ReadReg(ADDR_OUT_Z_H);
        B1[i * 6 + 5] = LIS3MDL_ReadReg(ADDR_OUT_Z_L);
    }
    for (int i = 0; i < B_DATA_SIZE; i++)
    {
        B2[i * 6 + 0] = LIS3MDL_ReadReg(ADDR_OUT_X_H);
        B2[i * 6 + 1] = LIS3MDL_ReadReg(ADDR_OUT_X_L);
        B2[i * 6 + 2] = LIS3MDL_ReadReg(ADDR_OUT_Y_H);
        B2[i * 6 + 3] = LIS3MDL_ReadReg(ADDR_OUT_Y_L);
        B2[i * 6 + 4] = LIS3MDL_ReadReg(ADDR_OUT_Z_H);
        B2[i * 6 + 5] = LIS3MDL_ReadReg(ADDR_OUT_Z_L);
    }
    for (int i = 0; i < B_DATA_SIZE; i++)
    {
        B3[i * 6 + 0] = LIS3MDL_ReadReg(ADDR_OUT_X_H);
        B3[i * 6 + 1] = LIS3MDL_ReadReg(ADDR_OUT_X_L);
        B3[i * 6 + 2] = LIS3MDL_ReadReg(ADDR_OUT_Y_H);
        B3[i * 6 + 3] = LIS3MDL_ReadReg(ADDR_OUT_Y_L);
        B3[i * 6 + 4] = LIS3MDL_ReadReg(ADDR_OUT_Z_H);
        B3[i * 6 + 5] = LIS3MDL_ReadReg(ADDR_OUT_Z_L);
    }
    // HAL_UART_Transmit_DMA(&HUART, &txFlag_x, 1);
    // HAL_UART_Transmit_DMA(&HUART, (uint8_t *)Bx_H, DATA_SIZE);
    // HAL_UART_Transmit_DMA(&HUART, (uint8_t *)Bx_L, DATA_SIZE);

    // HAL_UART_Transmit_DMA(&HUART, &txFlag_y, 1);
    // HAL_UART_Transmit_DMA(&HUART, (uint8_t *)By_H, DATA_SIZE);
    // HAL_UART_Transmit_DMA(&HUART, (uint8_t *)By_L, DATA_SIZE);

    // HAL_UART_Transmit_DMA(&HUART, &txFlag_z, 1);
    // HAL_UART_Transmit_DMA(&HUART, (uint8_t *)Bz_H, DATA_SIZE);
    // HAL_UART_Transmit_DMA(&HUART, (uint8_t *)Bz_L, DATA_SIZE);

    // HAL_UART_Transmit(&HUART, &txFlag_x, 1, HAL_MAX_DELAY);
    // HAL_UART_Transmit(&HUART, (uint8_t *)Bx_H, DATA_SIZE, HAL_MAX_DELAY);
    // HAL_UART_Transmit(&HUART, (uint8_t *)Bx_L, DATA_SIZE, HAL_MAX_DELAY);

    // HAL_UART_Transmit(&HUART, &txFlag_y, 1, HAL_MAX_DELAY);
    // HAL_UART_Transmit(&HUART, (uint8_t *)By_H, DATA_SIZE, HAL_MAX_DELAY);
    // HAL_UART_Transmit(&HUART, (uint8_t *)By_L, DATA_SIZE, HAL_MAX_DELAY);

    // HAL_UART_Transmit(&HUART, &txFlag_z, 1, HAL_MAX_DELAY);
    // HAL_UART_Transmit(&HUART, (uint8_t *)Bz_H, DATA_SIZE, HAL_MAX_DELAY);
    // HAL_UART_Transmit(&HUART, (uint8_t *)Bz_L, DATA_SIZE, HAL_MAX_DELAY);
}

// int fputc(int ch, FILE* f){
// 	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1000);
//     return ch;
// }
void LIS3MDL_demo(void)
{
    LIS3MDL_Init();
    while (1)
    {
        LIS3MDL_DataRead();
    }
}
void example_lis3mdl_0(void)
{
    // UART_PrintfInit(&uart_printf);
    LIS3MDL_InitStruct(&lis3mdl);
    while (1)
    {
        LIS3MDL_DataUpdate(&lis3mdl);
        printf("%.3f,%.3f,%.3f\n", lis3mdl.data_x, lis3mdl.data_y, lis3mdl.data_z);
    }
}
