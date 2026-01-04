//
// Created by xwj on 10/31/25.
//

#include "as5600.h"
#include <stdio.h>
// extern AS5600_t magneticEncoder;
// // 初始化示例
// void AS5600_Example_Init(void)
// {
//     // 初始化I2C（假设已经在CubeMX中配置）
//     extern I2C_HandleTypeDef hi2c2;
//
//     // ========== AS5600 初始化 ==========
//     printf("Initializing AS5600...\n");
//
//     // 方法1：不使用方向引脚（软件控制方向）
//     if (AS5600_Init(&magneticEncoder, &hi2c2, AS5600_DEFAULT_ADDRESS))
//     {
//         printf("AS5600 initialized successfully\n");
//     }
//     else
//     {
//         printf("AS5600 initialization failed\n");
//         return;
//     }
//
//     // 方法2：使用方向引脚（硬件控制方向）
//     // if (AS5600_InitWithPin(&magneticEncoder, &hi2c1, AS5600_DEFAULT_ADDRESS,
//     //                        GPIOA, 5)) {  // PA5作为方向引脚
//     //     printf("AS5600 initialized with direction pin\n");
//     // }
//
//     // 检查磁铁
//     if (AS5600_DetectMagnet(&magneticEncoder))
//     {
//         printf("Magnet detected\n");
//
//         if (AS5600_MagnetTooStrong(&magneticEncoder))
//         {
//             printf("Warning: Magnet too strong\n");
//         }
//         if (AS5600_MagnetTooWeak(&magneticEncoder))
//         {
//             printf("Warning: Magnet too weak\n");
//         }
//     }
//     else
//     {
//         printf("No magnet detected\n");
//     }
//
//     // 配置AS5600
//     AS5600_SetPowerMode(&magneticEncoder, AS5600_POWERMODE_NOMINAL);
//     AS5600_SetHysteresis(&magneticEncoder, AS5600_HYST_LSB1);
//     AS5600_SetSlowFilter(&magneticEncoder, AS5600_SLOW_FILT_4X);
//     AS5600_SetFastFilter(&magneticEncoder, AS5600_FAST_FILT_LSB6);
//
//     // 设置零位
//     uint16_t currentAngle = AS5600_RawAngle(&magneticEncoder);
//     AS5600_SetZPosition(&magneticEncoder, currentAngle);
//     printf("Zero position set to: %d\n", currentAngle);
//
// }

void test_as5600()
{
    // AS5600_Example_Init();
    // while (1)
    // {
    //     AS5600_Example_ReadAngle();
    //     delay_ms(1000);
    // }
    as5600_example_0();
}
