/**************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 main.c
作者	   : 刘有为
版本	   : V1.0
描述	   : I.MX6U开发板裸机实验5 BSP形式的LED驱动
其他	   : 本实验学习目的:
		 1、将各个不同的文件进行分类，学习如何整理工程、就
		 和学习STM32一样创建工程的各个文件夹分类，实现工程文件
		 的分类化和模块化，便于管理。
		 2、深入学习Makefile，学习Makefile的高级技巧，学习编写
		 通用Makefile。
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/4/28 刘有为创建
**************************************************************/

#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_beep.h"
#include "bsp_key.h"

int main(void)
{
	int i = 0;
	int key_value = 0;
	unsigned char led_state = OFF;
	unsigned char beep_state = OFF;

	imx6u_clk_init();	/* 初始化系统时钟 */
	clk_enable();		/* 使能所有的时钟 			*/
	led_init();			/* 初始化led 			*/
	beep_init();		/* 初始化beep	 		*/
	key_init();			/* 初始化 key */

	while(1)			
	{	
		key_value = key_get_value();
		if (0 != key_value)
		{
			switch (key_value)
			{
				case KEY0_VALUE:
					beep_state = !beep_state;
					beep_switch(beep_state);
					break;
			}
		}
		i++;
		if (50 == i)
		{
			i = 0;
			led_state = !led_state;
			led_switch(LED0, led_state);
		}
		delay(10);
	}

	return 0;
}

