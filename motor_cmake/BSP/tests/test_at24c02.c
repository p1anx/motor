//
// Created by xwj on 10/15/25.
//

#include "test_at24c02.h"

#include <stdio.h>
#include "at24c02.h"
#include "common.h"

void test_write(void);

void test_float(void) ;
void test_32bits(void);
void test_at24c02(void) {

    //1.
    // test_write();
    //2.
    // test_32bits();
    //3.
    test_float();

}
void test_float(void) {
    float read_float = 0.0f;       // 读取的浮点数
    uint8_t result;

    while (1) {
        float write_float = 3.141f;
        result = AT24C02_WriteFloat(0, write_float);
        if (result == 0) {
        printf("write ok\n");
        }
        result = AT24C02_ReadFloat(0, &read_float);
        if (result == 0) {
            printf("write data = %f\n", write_float);
            printf("read float data = %f\n", read_float);

        }
        delay_ms(1000);

    }

}
void test_write(void) {
    uint8_t data;
    while (1) {
        if (AT24C02_CheckDevice() == 0) {
            printf("e2prom is ok\n");
            AT24C02_WriteByte(0, 10);
            delay_ms(1000);
            AT24C02_ReadByte(0, &data);
            delay_ms(10);
            printf("data = %d\n", data);
        }

        // printf("test_at24c02\n");
    }

}

void test_32bits(void) {
    int write_data =  15532;
    int read_data;
    while (1) {
        if (AT24C02_CheckDevice() == 0) {
            AT24C02_Write32Bit(0, write_data);

            delay_ms(1000);
            AT24C02_Read32Bit(0, &read_data);
            printf("read_data = %d\n", read_data);
        }
    }

}