/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_delay.c
作者	   : 刘有为
版本	   : V1.0
描述	   : 延时文件。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/4/28 刘有为创建
***************************************************************/

#include "bsp_delay.h"

static void delay_short(volatile unsigned int n)
{
	while (n--) {}
}

void delay(volatile unsigned int n)
{
	while (n--)
	{
		delay_short(0x7ff);
	}
}

