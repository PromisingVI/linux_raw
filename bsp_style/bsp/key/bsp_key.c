/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_key.c
作者	   : 刘有为
版本	   : V1.0
描述	   : 按键驱动文件。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/4/28 刘有为创建
***************************************************************/

#include "bsp_key.h"
#include "bsp_gpio.h"
#include "bsp_delay.h"

void key_init(void)
{
	GPIO_PIN_CONFIG key_config = {0};

	/* 1、初始化IO复用, 复用为GPIO1_IO18 */
	IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18,0);

	/* 2、、配置UART1_CTS_B的IO属性	
	 *bit 16:0 HYS关闭
	 *bit [15:14]: 11 默认22K上拉
	 *bit [13]: 1 pull功能
	 *bit [12]: 1 pull/keeper使能
	 *bit [11]: 0 关闭开路输出
	 *bit [7:6]: 10 速度100Mhz
	 *bit [5:3]: 000 关闭输出
	 *bit [0]: 0 低转换率
	 */
	 IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18,0xF080);

	 /* 3、初始化GPIO */
	//GPIO1->GDIR &= ~(1 << 18);	/* GPIO1_IO18设置为输入 */	
	key_config.direction = KGPIO_DIGITAL_INPUT;
	gpio_init(GPIO1,18, &key_config);
}

int key_get_value(void)
{
	int ret = 0;
	static unsigned char release = 1;	/* 按键松开 */

	if ((1 == release) && (0 == gpio_pin_read(GPIO1, 18)))
	{
		delay(10);		/* 延时消抖 */
		release = 0;	/* 标记按键按下 */
		if (0 == gpio_pin_read(GPIO1, 18))
		{
			return KEY0_VALUE;
		}
	}
	else if (1 == gpio_pin_read(GPIO1, 18))
	{
		ret = 0;
		release = 1;	/* 标记按键释放 */
	}

	return ret;
}

