#include "OLED_GPIO.h"

void OLED_HAL_GPIO_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
	uint32_t i, j;
	
	/*在初始化前，加入适量延时，待OLED供电稳定*/
	for (i = 0; i < 1000; i ++)
	{
		for (j = 0; j < 1000; j ++);
	}
    /*定义一个GPIO_InitTypeDef类型的结构体*/
	/*将SCL和SDA引脚初始化为开漏模式*/


    /*开启LED相关的GPIO外设时钟*/
    OLED_SCL_GPIO_CLK_ENABLE();
    OLED_SDA_GPIO_CLK_ENABLE();
  
	/*将SCL和SDA引脚初始化为开漏模式*/

    /*选择要控制的GPIO引脚*/															   
    GPIO_InitStruct.Pin =OLED_SCL_GPIO_PIN;	

    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_OD;  

    /*设置引脚为上拉模式*/
    GPIO_InitStruct.Pull  = GPIO_PULLUP;

    /*设置引脚速率为高速 */   
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    /*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
    HAL_GPIO_Init(OLED_SCL_GPIO_PORT, &GPIO_InitStruct);	

    /*选择要控制的LED2 GPIO引脚*/															   
    GPIO_InitStruct.Pin = OLED_SDA_GPIO_PIN;	
    HAL_GPIO_Init(OLED_SDA_GPIO_PORT, &GPIO_InitStruct);	
	
}