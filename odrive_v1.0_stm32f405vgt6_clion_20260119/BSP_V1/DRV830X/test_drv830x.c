//
// Created by xwj on 12/21/25.
//
#include "DRV830X.h"
#include "main.h"
#include <stdio.h>

__attribute__((aligned(4))) static char drv8301Memory[sizeof(DRV8301_Obj)];
void test_drv830x(void) {
  // drv8301_example0();

  // DRV8301_Handle drv8301;
  // DRV8301_Setup(drv8301);
  DRV_SPI_8301_Vars_t drv8301_vars;
  DRV8301_Handle drv8301 = DRV8301_init(&drv8301Memory, sizeof(drv8301Memory)); // 声明句柄变量
  // 3. 务必检查句柄是否有效
  if(drv8301 == NULL) {
    // 初始化失败，通常是内存不足，需进行错误处理（如死循环+报错）
    printf("drv8301 init failed\n");
    while(1);
  }

  DRV8301_InitStruct(drv8301);
  DRV8301_enable(drv8301);

  // drv8301->EngpioHandle = EN_GATE_GPIO_Port;
  // drv8301->EngpioNumber = EN_GATE_Pin;
  // drv8301->nCSgpioHandle = M0_NCS_GPIO_Port;
  // drv8301->nCSgpioNumber = M0_NCS_Pin;
  // drv8301->spiHandle = &DRV8301_SPI;
  printf("hello odrive\n");
  uint16_t gain, setGain;
  DRV8301_setShuntAmpGain(drv8301, DRV8301_ShuntAmpGain_20VpV);

  DRV8301_setupSpi(drv8301, &drv8301_vars);
  HAL_Delay(1000);

  while (1) {
    uint16_t drv8301_id = DRV8301_getId(drv8301);
    bool isFault = DRV8301_isFault(drv8301);

  // DRV8301_setShuntAmpGain(drv8301, DRV8301_ShuntAmpGain_40VpV);
    setGain = DRV8301_getShuntAmpGain(drv8301);
    // clear the bits
    // setGain &= (DRV8301_CTRL2_GAIN_BITS);
    // setGain = setGain >> 2;


    printf("drv8301_id = %d\n", drv8301_id);
    printf("drv8301_device_id = %d\n", drv8301_vars.Stat_Reg_2.DeviceID);
    printf("drv8301_isFault = %d\n", isFault);
    printf("drv8301_gain set = %d\n", drv8301_vars.Ctrl_Reg_2.GAIN);
    printf("drv8301_stat 1: %d, %d\n", drv8301_vars.Stat_Reg_1.FETHA_OC, drv8301_vars.Stat_Reg_1.FETHB_OC);
    HAL_Delay(1000);
  }

}