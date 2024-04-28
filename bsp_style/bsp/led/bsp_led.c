/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_led.c
作者	   : 刘有为
版本	   : V1.0
描述	   : LED驱动文件。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/4/28 刘有为创建
***************************************************************/

#include "bsp_led.h"

void led_init(void)
{
	/* 1、初始化IO复用 */
	IOMUXC_SetPinMux(IOMUXC_GPIO1_IO03_GPIO1_IO03,0);		/* 复用为GPIO1_IO03 */

	/* 2、、配置GPIO1_IO03的IO属性	
	 *bit 16:0 HYS关闭
	 *bit [15:14]: 00 默认下拉
	 *bit [13]: 0 kepper功能
	 *bit [12]: 1 pull/keeper使能
	 *bit [11]: 0 关闭开路输出
	 *bit [7:6]: 10 速度100Mhz
	 *bit [5:3]: 110 R0/6驱动能力
	 *bit [0]: 0 低转换率
	 */
	IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO03_GPIO1_IO03,0X10B0);
	
	/* 3、初始化GPIO,GPIO1_IO03设置为输出*/
	GPIO1->GDIR |= (1 << 3);	 

	/* 4、设置GPIO1_IO03输出低电平，打开LED0*/
	GPIO1->DR &= ~(1 << 3);		
}

void led_switch(int led, int status)
{	
	switch(led)
	{
		case LED0:
			if(status == ON)
				GPIO1->DR &= ~(1<<3);	/* 打开LED0 */
			else if(status == OFF)
				GPIO1->DR |= (1<<3);	/* 关闭LED0 */
			break;
	}
}
