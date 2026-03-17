#ifndef  __KEY_H
#define __KEY_H
#include "stm32_hal.h"

#define KEY_ON	1
#define KEY_OFF	0

//pe0
#define KEY0_GPIO_Port GPIOE
#define KEY0_GPIO_Pin GPIO_PIN_0

#define KEY1_GPIO_Port GPIOE
#define KEY1_GPIO_Pin GPIO_PIN_1

typedef struct KEY_t KEY_t;
struct KEY_t {
  GPIO_TypeDef* GPIO_Port;
  uint16_t GPIO_Pin;
  int isPressed;
  int enableMotor;
  int flag;

};



uint8_t Key_Scan(int key_num);
// void KEY_MotorControl(KEY_t* key, uint16_t GPIO_Pin);
void KEY_enableMotor(KEY_t* key, uint16_t GPIO_Pin);
// uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);
#endif
