//
// Created by xwj on 10/15/25.
//
#include <stdio.h>

#include "common.h"
#include "mt6835.h"
#include  "eeprom.h"

void test_init(void);
void test_save_data(void);

void test_read_data(void) ;

extern int gKey_flag_0;

void test_mt6835() {
    //1.
    test_init();
    //2.
    // test_save_data();
    //3.
    // test_read_data();

}

void test_read_data(void) {
    float last_data = 0.0f;
    while (1) {
        eeprom_ReadFloat(0, &last_data);
        printf("last_data: %f\r\n", last_data);
        delay_ms(1000);
    }

}

void test_save_data(void)
{
    // extern SPI_HandleTypeDef hspi1;

    /* 全局变量 */
    MT6835_Handle mt6835;              // 编码器句柄

    // 初始化编码器
    MT6835_Init(&mt6835, &mt6835_spi, MT6835_SPI_CS_PORT, MT6835_SPI_CS_PIN);

    // 初始化角度计算器 (减速比10:1)
    // Motor_AngleCalc_Init(&motor, 10.0f);

    printf("[ok] init\r\n");

    float read_data = 0.0f;
    while(1)
    {
        // // 1. 读取编码器角度
        // MT6835_ReadAngle(&mt6835);
        // float mt6835_angle = MT6835_GetAngleDegrees(&mt6835);
        //
        // // 2. 更新角度计算
        // Motor_AngleCalc_Update(&mt6835.motor_angle_calc, mt6835_angle);

        MT6835_Angle_Update(&mt6835);
        // 3. 获取输出轴角度
        float output_angle = MT6835_GetMotorAngleDegrees(&mt6835);
        float result = eeprom_WriteFloat(0, output_angle);
        if (result == 0) {
            printf("[ok] save data\n");
        }



        if (gKey_flag_0 == 1) {
            gKey_flag_0 = 0;
            eeprom_ReadFloat(0, &read_data);
            printf("[ok] read saved data = %f\n", read_data);
            delay_ms(6000);
        }


        // 4. 显示结果
        printf("motor angle: %.2f | actual angle: %.2f\r\n",
               mt6835.angle_deg, output_angle);

        HAL_Delay(10);
    }
}
void test_init(void) {
    while (1) {
        printf("Testing MT6835\n");
        Example1_BasicUsage();

    }
}

